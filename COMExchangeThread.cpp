
#include "stdafx.h"
#include "resource.h"
#include "COMExchangeThread.h"


#define STANDART_COM_BITRATE	115200
#define STANDART_COM_STOPBITS	ONESTOPBIT
#define INFINITE_TIMEOUT		0
#define STANDART_COM_TIMEOUTS	INFINITE_TIMEOUT


CCOMExchangeThread::CCOMExchangeThread(void):
	CCustomExchangeThread(),
	m_hCOM(NULL),
	m_PortName("")
{	
	///////////////////////////
	memset(&m_ovRx, 0, sizeof(m_ovRx));
	m_ovRx.hEvent = CreateEvent(NULL, true, false, NULL);	// ������� ������� ������������ � ������� RecieveFunc
	
	memset(&m_ovTx, 0, sizeof(m_ovTx));	
	m_ovTx.hEvent = CreateEvent(NULL, false, false, NULL);	// ������� ������� ������������ � ������� Send
}

CCOMExchangeThread::~CCOMExchangeThread(void)
{

	if (m_ovRx.hEvent)	CloseHandle(m_ovRx.hEvent);
	if (m_ovTx.hEvent)	CloseHandle(m_ovTx.hEvent);
}

bool CCOMExchangeThread::SetBitRate(DWORD BitRate)
{
	DCB dcb;

	/* Prepare DCB structure */
	SecureZeroMemory(&dcb, sizeof(DCB));  /* PRQA S 3200 */
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = BitRate;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fTXContinueOnXoff = TRUE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE; 
	dcb.fRtsControl = RTS_CONTROL_ENABLE; 
	dcb.fAbortOnError = FALSE;
	dcb.XonLim = 0; 
	dcb.XoffLim = 0;  
	dcb.ByteSize = 8; 
	dcb.Parity = NOPARITY; 
	dcb.StopBits = STANDART_COM_STOPBITS;

	/* Apply the settings */
	if (SetCommState(m_hCOM, &dcb) == FALSE)
		return false;

	/* Set DTR, some boards needs a DTR = 1 level */
	if (EscapeCommFunction(m_hCOM, SETDTR) == FALSE)
		return false;
	
	return true;
}

bool CCOMExchangeThread::RecieveFunc()
{
	BOOL		status;
	DWORD		dwBytesRead = 0;
	DWORD		dwErrors;
	COMSTAT		abComstat;
	DWORD		dummy, mask, signal;
	int			btr;
	
	if (!m_PortOpen || !m_hCOM)
		return false;
	
	status = WaitCommEvent(m_hCOM, &mask, &m_ovRx);	// ������� ������� ����� ����� (��� � ���� ������������� ��������)
	dwErrors = GetLastError();
	if (status || (!status && dwErrors == ERROR_IO_PENDING))
	{
		do
		{
			// ������������� ����� �� ������� ����-������ � ���� ��� �� ��������� �������� �������� ������� ����� �� �����
			signal = WaitForSingleObject(m_ovRx.hEvent, INFINITE);
			switch (signal)
			{
			case WAIT_OBJECT_0: // ���� ������� ������� ����� ���������
				if (GetOverlappedResult(m_hCOM, &m_ovRx, &dummy, true)) //���������, ������� �� ����������� ������������� �������� WaitCommEvent
				{
					if (mask & EV_RXCHAR)   //���� ��������� ������ ������� ������� �����
					{	// ����� ��������� ��������� COMSTAT
						if (ClearCommError(m_hCOM, &dwErrors, &abComstat) != FALSE)
						{
							/* �� ��������� ������ �� MSDN ������� ReadFile �� ���������� ����� ����������� ���� ��� ������ � ����������� ������,
							 * �������� ����� ����������� ���� ����� ������ � ������� ���� abComstat.cbInQue */
							btr = abComstat.cbInQue;
							if (btr)
							{	//��������� ����� �� ����� � ����� ���������
								do
								{
									dwBytesRead = min(btr, sizeof(m_RBuf) / sizeof(m_RBuf[0]));
									ReadFile(m_hCOM, m_RBuf, dwBytesRead, NULL, &m_ovRx);
									
									
									btr -= sizeof(m_RBuf) / sizeof(m_RBuf[0]);
								}
								while (btr > 0);
							}
						}
						else
							return false;
					}
				}
				else
					return false;
				break;

			default:
				return false;
			}
		} while (signal == WAIT_TIMEOUT);
	}
	else
		return false;

	return true;
}

bool CCOMExchangeThread::Send(const BYTE * Data, const int DataSize, const DWORD TimeoutMs, const  bool DisconnectOnTimeout /*= true*/)
{	
	DWORD dwBytesWritten;
	DWORD err = GetLastError();
	EnterCriticalSection(&m_csMethodsAccess);
	
	if (!m_hCOM || !m_PortOpen)
	{
		LeaveCriticalSection(&m_csMethodsAccess);
		return false;
	}

	PurgeComm(m_hCOM, PURGE_TXCLEAR);	//�������� ���������� ����� �����	
	PurgeComm(m_hCOM, PURGE_RXCLEAR);	//�������� ����������� ����� �����
	PurgeComm(m_hCOM, PURGE_RXABORT);	// ��������� ��� �������� �����
	PurgeComm(m_hCOM, PURGE_TXABORT);	// ��������� ��� �������� ��������
			
	BOOL WriteRes = WriteFile(m_hCOM, Data, DataSize, &dwBytesWritten, &m_ovTx);  // �������� ����� � ���� (������������� ��������!)
	DWORD signal = WaitForSingleObject(m_ovTx.hEvent, TimeoutMs);	  // ������������� �����, ���� �� ���������� ������������� �������� WriteFile
	if (signal == WAIT_TIMEOUT)
	{
		PurgeComm(m_hCOM, PURGE_TXCLEAR);	//�������� ���������� ����� �����	
		PurgeComm(m_hCOM, PURGE_TXABORT);	// ��������� ��� �������� ��������
		PurgeComm(m_hCOM, PURGE_TXCLEAR);	//�������� ���������� ����� �����	
		if (DisconnectOnTimeout)
			DisconnectController();
		LeaveCriticalSection(&m_csMethodsAccess);
		return false;
	}
	else
	{
		BOOL OverlappedRes = GetOverlappedResult(m_hCOM, &m_ovTx, &dwBytesWritten, true);

		/* General error check */
		if ((signal != WAIT_OBJECT_0) || (!WriteRes && (err != ERROR_IO_PENDING)) || !OverlappedRes)
		{		
			PurgeComm(m_hCOM, PURGE_TXCLEAR);     //�������� ���������� ����� �����	
			PurgeComm(m_hCOM, PURGE_TXABORT);	// ��������� ��� �������� ��������
			PurgeComm(m_hCOM, PURGE_TXCLEAR);     //�������� ���������� ����� �����	
			if (DisconnectOnTimeout)
				DisconnectController();
			LeaveCriticalSection(&m_csMethodsAccess);
			return false;
		}
		else
			LeaveCriticalSection(&m_csMethodsAccess);
	}

	/* Error check */
	return (dwBytesWritten == DataSize);
}

bool CCOMExchangeThread::OpenPort(void * pParams)
{
	CString * pPortName = (CString *)pParams;
	bool res = false;

	EnterCriticalSection(&m_csMethodsAccess);
	
	if (m_PortOpen || m_hCOM)
	{
		LeaveCriticalSection(&m_csMethodsAccess);
		return res;
	}

	res = m_PortOpen = false;

	if (OpenPort_Int(pPortName, &m_hCOM))
	{	/* ������� ������� ���� */
		ClearDataQueues();

		m_PortName = *pPortName;
		m_RecvState &= ~RECV_STATE_DISCONNECTED;
		m_RecvState |= RECV_STATE_CONNECTED;
		ResetEvent(m_hRecvDisconnectedEvent);
		SetEvent(m_hRecvConnectedEvent);
		res = m_PortOpen = true;

		StartRThread(); /* ���� ������ => ����� ��������� ����� ������ */
	}

	LeaveCriticalSection(&m_csMethodsAccess);
	return res;
}

bool CCOMExchangeThread::OpenPort_Int(CString * pPortName, PHANDLE phCOM)
{
	COMMTIMEOUTS cto;
	bool res = false;

	CString FullPortName = L"\\\\.\\" + *pPortName;	/* ������ ��� COM-����� */

	/* Open port */
	*phCOM = CreateFile(
		FullPortName,					/* Serial port file name. */
		GENERIC_READ | GENERIC_WRITE,	/* Access (read-write) mode. */
		0,								/* Share mode. */
		NULL,							/* Pointer to the security attribute. */
		OPEN_EXISTING,					/* How to open the serial port. */
		FILE_FLAG_OVERLAPPED,			/* Port attributes. */
		NULL);							/* Handle port in async mode. */

	/* Error Checking */
	if (*phCOM == INVALID_HANDLE_VALUE || *phCOM == NULL)
	{
		*phCOM = NULL;
		return res;
	}

	/* ������ ��������� ����� */
	if (!SetBitRate(STANDART_COM_BITRATE))
	{
		CloseHandle(*phCOM);  /* PRQA S 3200 */
		*phCOM = NULL;
		return res;
	}
			
	/* Set the communication timeout */
	/* Prepare CTO structure */
	cto.ReadTotalTimeoutConstant = 1000;//STANDART_COM_TIMEOUTS;
	cto.ReadTotalTimeoutMultiplier = 0;
	cto.ReadIntervalTimeout = 0;
	cto.WriteTotalTimeoutConstant = STANDART_COM_TIMEOUTS;
	cto.WriteTotalTimeoutMultiplier = 0;

	/* Apply timeouts */
	if (SetCommTimeouts(*phCOM, &cto) == FALSE)
	{
		CloseHandle(*phCOM);  /* PRQA S 3200 */
		*phCOM = NULL;
		return res;
	}

	PurgeComm(*phCOM, PURGE_RXCLEAR);	// �������� ����������� ����� �����
	PurgeComm(*phCOM, PURGE_TXCLEAR);	// �������� ����������� ����� �����
	PurgeComm(*phCOM, PURGE_RXABORT);	// ��������� ��� �������� �����
	PurgeComm(*phCOM, PURGE_TXABORT);	// ��������� ��� �������� ��������

	SetCommMask(*phCOM, EV_RXCHAR);		// ���������� ����� �� ������������ �� ������� ����� ����� � ����

	res = true;

	return res;
}

void CCOMExchangeThread::ClosePort_Int(PHANDLE phCOM)
{
	EnterCriticalSection(&m_csMethodsAccess);

	PurgeComm(*phCOM, PURGE_RXCLEAR);	// �������� ����������� ����� �����
	PurgeComm(*phCOM, PURGE_TXCLEAR);	// �������� ����������� ����� �����
	PurgeComm(*phCOM, PURGE_RXABORT);	// ��������� ��� �������� �����
	PurgeComm(*phCOM, PURGE_TXABORT);	// ��������� ��� �������� ��������
			
	CloseHandle(*phCOM);
	*phCOM = NULL;

	LeaveCriticalSection(&m_csMethodsAccess);
}

void CCOMExchangeThread::ClosePort(bool NeedDisconnectController)
{
	EnterCriticalSection(&m_csMethodsAccess);

	StopRThread();
	if (m_PortOpen)
	{		
		if (m_hCOM)
			/* ��������� ��� ������������� �������� ������ � ������ */
			ClosePort_Int(&m_hCOM);

		WaitForRTThreadWaiting(THREAD_WAIT_TO_CLOSE_TIME);
		
		CCustomExchangeThread::ClosePort(NeedDisconnectController);

		m_PortOpen = false;
	}
	m_PortName = "";

	LeaveCriticalSection(&m_csMethodsAccess);
}

bool CCOMExchangeThread::CheckRecvConnected(void * pParams
									)
{
	CString * pPortName = (CString *)pParams;
	
	if (pPortName && pPortName->Compare(m_PortName) && !pPortName->IsEmpty())
	{	/* ��� �� ������� ���� � �������� �� ������ ������ */
		HANDLE hCOM = NULL;
		
		if (!OpenPort_Int(pPortName, &hCOM))
			/* �� ������� ������� ���� */
			return false;

		ClosePort_Int(&hCOM);

		return true;
	}
	else
	{	/* ���������, �� ���������� �� ������ */
		return SetBitRate(STANDART_COM_BITRATE);
	}
}

void CCOMExchangeThread::DisconnectController()
{
	StopRThread();
	CCustomExchangeThread::DisconnectController();
}
