
#include "stdafx.h"

#include "GSM.h"
#include "Sim_900.h"
#include "config.h"


void connect_to_COM(DCB* dcb)
{
	GetCommState(h_com, dcb);
	dcb->BaudRate = RASE;       //����� �������� �������� �� ������� ��� 115200
	dcb->fBinary = TRUE;                                    //�������� �������� ����� ������
	dcb->fOutxCtsFlow = FALSE;                              //��������� ����� �������� �� �������� CTS
	dcb->fOutxDsrFlow = FALSE;                              //��������� ����� �������� �� �������� DSR
	dcb->fDtrControl = DTR_CONTROL_DISABLE;                 //��������� ������������� ����� DTR
	dcb->fDsrSensitivity = FALSE;                           //��������� ��������������� �������� � ��������� ����� DSR
	dcb->fNull = FALSE;                                     //��������� ���� ������� ������
	dcb->fRtsControl = RTS_CONTROL_DISABLE;                 //��������� ������������� ����� RTS
	dcb->fAbortOnError = FALSE;                             //��������� ��������� ���� �������� ������/������ ��� ������
	dcb->ByteSize = 8;                                      //����� 8 ��� � �����
															//��������� �������� ��������
															//����� ���� ����-���
	dcb->Parity = NOPARITY;
	dcb->StopBits = ONESTOPBIT;
	SetupComm(h_com, 50, 50);

	memset(&m_ovRx, 0, sizeof(m_ovRx));
	m_ovRx.hEvent = CreateEvent(NULL, true, false, NULL);	// ������� ������� ������������ � ������� RecieveFunc

	memset(&m_ovTx, 0, sizeof(m_ovTx));
	m_ovTx.hEvent = CreateEvent(NULL, false, false, NULL);	// ������� ������� ������������ � ������� Send
}

void set_time_outs()
{
	SetCommTimeouts(h_com, &time_outs);
	time_outs.ReadTotalTimeoutConstant = 1000;//STANDART_COM_TIMEOUTS;
	time_outs.ReadTotalTimeoutMultiplier = MAXDWORD;
	time_outs.ReadIntervalTimeout = MAXDWORD;
	time_outs.WriteTotalTimeoutConstant = 0;
	time_outs.WriteTotalTimeoutMultiplier = 0;
	SetCommMask(h_com, EV_RXCHAR);
}

int init()
{
	setlocale(LC_ALL, "Russian");
	char* pcComPort = COM_PORT;
	is_echo = false;

	h_com = CreateFile(TEXT(COM_PORT),
		GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (h_com == INVALID_HANDLE_VALUE)
	{
		printf("������ �������� COM!\n");
		return 0;
		while (!_kbhit());
		return 0;
	}
	connect_to_COM(&dcb);
	
	f_success = SetCommState(h_com, &dcb);
	if (!f_success) {
		printf("������� ������� SetCommState �����������!\n");
		while (!_kbhit());
		return 0;
	}
		
	printf("COM ���� %s ������� ���������������\n", pcComPort);
	GetCommState(h_com, &dcb);
	printf("�������� ����� %s ����� %d\n", pcComPort, dcb.BaudRate);
	h_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	set_time_outs();

	InitializeCriticalSection(&my_cs);
	init_cmds_and_answs_AT();
	cout << "������������� ���������\n";
	is_init = true;

	//���������� ���-������
	Sim900_set_echo_behaviour();
	Sim900_set_ATV1();
	
	//����������� � �������
	//if (!Sim900_conn_to_server())
	//	return 0;
	is_connect_to_server = true;
	
	// ����� ��������� �����
	h_thread = CreateThread(NULL, 0, listen_server, NULL, NULL, NULL);
	return 1;
}