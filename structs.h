#pragma once
typedef unsigned char BYTE;
struct stMainHeader
{
	int id;
	int cmd;
	int sender;
	int reciever; // по умолчанию сервер. Сервер сам оповещает всех кого надо
	int size_next_msg;
	BYTE data;
};