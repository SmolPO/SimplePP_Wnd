#pragma once
typedef unsigned char BYTE;
struct stMainHeader
{
	int id;
	int cmd;
	int sender;
	int reciever; // �� ��������� ������. ������ ��� ��������� ���� ���� ����
	int size_next_msg;
	BYTE data;
};