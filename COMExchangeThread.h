
/* Модуль обмена по COM или USB */

#pragma once

#include "CustomExchangeThread.h"

class CCOMExchangeThread : public CCustomExchangeThread
{
private:
	HANDLE m_hCOM;
	OVERLAPPED m_ovTx, m_ovRx;	/* Для операций Send и RecieveFunc */
	CString m_PortName;
		
	virtual bool RecieveFunc();

	/* Внутрення функция открытия порта */
	bool OpenPort_Int(CString * pPortName, PHANDLE phCOM);

	/* Внутрення функция закрытия порта */
	void ClosePort_Int(PHANDLE phCOM);

	/* Регистрируем факт отключения терминала */
	virtual void DisconnectController();

	/*
		Установка скорости соединения.
		Помимо этого задаются другие параметры порта с помощью структуры DCB.
	*/
	bool SetBitRate(DWORD BitRate);
	
public:
	CCOMExchangeThread(void);
	virtual ~CCOMExchangeThread(void);

	/* Передать данные в порт */
	virtual bool Send(const BYTE * Data, const int DataSize, const DWORD TimeoutMs, const bool DisconnectOnTimeout = true);

	/* Открываем порт. Открывает порт только в том случае, если он закрыт */
	virtual bool OpenPort(void * pParams	/* Это указатель на CString - название порта */
					);
	/* Закрываем текущий порт. */
	virtual void ClosePort(bool NeedDisconnectController		/* [In] Нужно ли чтобы функция посылала команду контроллеру о закрытии соединения */
							);

	/*
		Проверяем, есть ли на приемной строне кто-то.
		Если *pParams == m_PortName, то возвращаем true,
		в противном случае осуществляется попытка открытия порта, затем задания скорости для заданного порта и закрытие порта.
	*/
	virtual bool CheckRecvConnected(void * pParams	/* Это указатель на CString - название порта. Если NULL, то проверяем поключения по текущему порту */
									);

	CString GetPortName()	{ return m_PortName; };
};
