// SimplePP.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"

#include "Sim_900.h"
#include "GSM.h"
#include "structs.h"
using namespace std;

HANDLE h_event;
COMMTIMEOUTS time_outs;

//������
DWORD bytes_read, bWritten;
BOOL f_success;
HANDLE h_thread, h_com;

//��� COM
DWORD dw_param, dw_thread_id;
DCB dcb;
FILE* f_out;
OVERLAPPED m_ovTx, m_ovRx;

RTL_CRITICAL_SECTION my_cs;

int main()
{
	if (!init())
		return 0;

	while (true)
	{
		int choice = 0;
			// ���������� ���� ������������
		printf("��� �������? ��� �������� ��������� ������� 1\n\r���� �� ������ ��������� ��������� ������� - ����� 2\n\r��� ������ �� ��������� ������� 3\n\r");
		choice = _get_console_data("��� �����?\n\r");
		switch (choice)
		{
		case 1:
			send_new_command_server();
			break;
		case 2:
		//	send_new_command_lamps();
			break;
		case 3:
		//	close_programm();
			return 0;
		default:
			printf("�� ����� ��� �� �����������... ��������� �������...\n");
			break;
		}
		
	}
	//close_programm();
    return 0;
}

