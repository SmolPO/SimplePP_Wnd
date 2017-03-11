
/* ������ ������ �� COM ��� USB */

#pragma once

#include "CustomExchangeThread.h"

class CCOMExchangeThread : public CCustomExchangeThread
{
private:
	HANDLE m_hCOM;
	OVERLAPPED m_ovTx, m_ovRx;	/* ��� �������� Send � RecieveFunc */
	CString m_PortName;
		
	virtual bool RecieveFunc();

	/* ��������� ������� �������� ����� */
	bool OpenPort_Int(CString * pPortName, PHANDLE phCOM);

	/* ��������� ������� �������� ����� */
	void ClosePort_Int(PHANDLE phCOM);

	/* ������������ ���� ���������� ��������� */
	virtual void DisconnectController();

	/*
		��������� �������� ����������.
		������ ����� �������� ������ ��������� ����� � ������� ��������� DCB.
	*/
	bool SetBitRate(DWORD BitRate);
	
public:
	CCOMExchangeThread(void);
	virtual ~CCOMExchangeThread(void);

	/* �������� ������ � ���� */
	virtual bool Send(const BYTE * Data, const int DataSize, const DWORD TimeoutMs, const bool DisconnectOnTimeout = true);

	/* ��������� ����. ��������� ���� ������ � ��� ������, ���� �� ������ */
	virtual bool OpenPort(void * pParams	/* ��� ��������� �� CString - �������� ����� */
					);
	/* ��������� ������� ����. */
	virtual void ClosePort(bool NeedDisconnectController		/* [In] ����� �� ����� ������� �������� ������� ����������� � �������� ���������� */
							);

	/*
		���������, ���� �� �� �������� ������ ���-��.
		���� *pParams == m_PortName, �� ���������� true,
		� ��������� ������ �������������� ������� �������� �����, ����� ������� �������� ��� ��������� ����� � �������� �����.
	*/
	virtual bool CheckRecvConnected(void * pParams	/* ��� ��������� �� CString - �������� �����. ���� NULL, �� ��������� ���������� �� �������� ����� */
									);

	CString GetPortName()	{ return m_PortName; };
};
