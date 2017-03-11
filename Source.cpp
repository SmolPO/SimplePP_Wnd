#include "stdafx.h"

#include "GSM.h"
#include "Sim_900.h"

DWORD WINAPI listen_server(void*)
{
	Sim900_recv_handler();
	return true;
}

BOOL listen_lamps()
{
	cout << "listen lamps..." << endl;
	return true;
}

BOOL send_new_command_server()
{
	int cmd = 0;
	int data = 0;
	stMainHeader* next_message = new stMainHeader();

	BYTE* next_message_in_bytes;
	_get_cmd_and_data(&cmd, &data);

	next_message = Sim900_create_struct_message(cmd, BYTE(data));

	next_message_in_bytes = (BYTE*)next_message;
	if (Sim900_send_message((stMainHeader*)next_message_in_bytes)) {
		return true;
	}
	else {
		return false;
	}
}

int _get_console_data(const char* mess)
{
	char* _answer = new char(10);
	int answer = -1;
	cout << mess << endl;
	cin >> _answer;
	try
	{
		answer = atoi(_answer);
	}
	catch (...)
	{
		cout << "Вы ввели неверные данные" << endl;
		return answer;
	}
	return answer;
}

BOOL _get_cmd_and_data(int* _cmd, int* _data)
{
	while (true)
	{
		*_cmd = _get_console_data("Введите номер команды:\n");
		*_data = _get_console_data("Введите данные\n");
		if (*_cmd == -1 || *_data == -1)
		{
			cout << "Данные не корректны" << endl;
			*_cmd = -1;
			*_data = -1;
			continue;
		}
		else
			break;
	}
	return true;
}

BOOL close_programm()
{
	CloseHandle(h_thread);
	if (m_ovRx.hEvent)	CloseHandle(m_ovRx.hEvent);
	if (m_ovTx.hEvent)	CloseHandle(m_ovTx.hEvent);
	return true;
}

