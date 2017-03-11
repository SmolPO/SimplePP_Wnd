#include "stdafx.h"

#include "GSM.h"
#include "Sim_900.h"
#include "commands_AT.h"

BOOL Sim900_set_parametrs(stMainHeader* main_header)
{
	cout << "...set new paravetrs..." << endl;
	cout << main_header->data << endl;
	return true;
}

BOOL Sim900_on_light()
{
	cout << "... on light...." << endl;
	Sim900_write_cmd(cmd(AT_ON_LIGHT));///выключить лампочку
	return true;
}

BOOL Sim900_off_light()
{
	cout << "... off light...." << endl;
	Sim900_write_cmd(cmd(AT_OFF_LIGHT));///выключить лампочку
	return true;
}