
#include "stdafx.h"

#include "GSM.h"
#include "Sim_900.h"
#include "config.h"


void connect_to_COM(DCB* dcb)
{
	GetCommState(h_com, dcb);
	dcb->BaudRate = RASE;       //задаём скорость передачи по дефолту там 115200
	dcb->fBinary = TRUE;                                    //включаем двоичный режим обмена
	dcb->fOutxCtsFlow = FALSE;                              //выключаем режим слежения за сигналом CTS
	dcb->fOutxDsrFlow = FALSE;                              //выключаем режим слежения за сигналом DSR
	dcb->fDtrControl = DTR_CONTROL_DISABLE;                 //отключаем использование линии DTR
	dcb->fDsrSensitivity = FALSE;                           //отключаем восприимчивость драйвера к состоянию линии DSR
	dcb->fNull = FALSE;                                     //разрешить приём нулевых байтов
	dcb->fRtsControl = RTS_CONTROL_DISABLE;                 //отключаем использование линии RTS
	dcb->fAbortOnError = FALSE;                             //отключаем остановку всех операций чтения/записи при ошибке
	dcb->ByteSize = 8;                                      //задаём 8 бит в байте
															//отключаем проверку чётности
															//задаём один стоп-бит
	dcb->Parity = NOPARITY;
	dcb->StopBits = ONESTOPBIT;
	SetupComm(h_com, 50, 50);

	memset(&m_ovRx, 0, sizeof(m_ovRx));
	m_ovRx.hEvent = CreateEvent(NULL, true, false, NULL);	// создать событие используемое в функции RecieveFunc

	memset(&m_ovTx, 0, sizeof(m_ovTx));
	m_ovTx.hEvent = CreateEvent(NULL, false, false, NULL);	// создать событие используемое в функции Send
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
		printf("Ошибка открытия COM!\n");
		return 0;
		while (!_kbhit());
		return 0;
	}
	connect_to_COM(&dcb);
	
	f_success = SetCommState(h_com, &dcb);
	if (!f_success) {
		printf("Попытка вызвать SetCommState провалилась!\n");
		while (!_kbhit());
		return 0;
	}
		
	printf("COM порт %s успешно сконфигурирован\n", pcComPort);
	GetCommState(h_com, &dcb);
	printf("Скорость порта %s равна %d\n", pcComPort, dcb.BaudRate);
	h_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	set_time_outs();

	InitializeCriticalSection(&my_cs);
	init_cmds_and_answs_AT();
	cout << "иниуиализация завершена\n";
	is_init = true;

	//выключение эхо-режима
	Sim900_set_echo_behaviour();
	Sim900_set_ATV1();
	
	//Подключение к серверу
	//if (!Sim900_conn_to_server())
	//	return 0;
	is_connect_to_server = true;
	
	// поток прослушки порта
	h_thread = CreateThread(NULL, 0, listen_server, NULL, NULL, NULL);
	return 1;
}