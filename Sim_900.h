#pragma once
#include <string>

#include "structs.h"
// библиотека функций работы с модулейм Sim900
using namespace std;
int Sim900_conn_to_server();  ///0 - подключился
int Sim900_autotification();  // аутотификация на сервере
int Sim900_behaviour_hyber_terminal(); ///режим HyberTerminal


//модуль Sim900
VOID WINAPI thr_menu(PVOID* dummy); //меню работы с модулем
									  ///работа со списком команд и ответов
void  add_cmd(char* cmd);
void  add_answer(char* answer);
void  init_cmds_and_answs_AT();
char* cmd(char* buffer);
char* answer(char* buffer);

void Sim900_recv_handler();

int Sim900_write_cmd(char* cmd);
int Sim900_write_with_size(char* buff, int size);
int Sim900_read_COM(char* answerCOM, int timeout_ms);
BOOL Sim900_all_read(char* buffer, int count_word, int timeout_ms);
int Sim900_cmp_strs(char* NeedAnswer);
int Sim900_cmp_strs_with_time_out(char* answer);

int Sim900_send_message(stMainHeader* msg);
stMainHeader* Sim900_create_struct_message(int cmd, BYTE data);
int Sim900_put_char(int ASCIIch);
void Sim900_add_end_symbol(char* cmd);

int Sim900_сlose_app();

//обработчики сообщений
BOOL Sim900_set_parametrs(stMainHeader* msg);
BOOL Sim900_on_light();
BOOL Sim900_off_light();

//прочие
BOOL Sim900_set_ATV0();
BOOL Sim900_set_ATV1();
BOOL Sim900_set_echo_behaviour();


void Sim900_answer();
///void _print_answer(string foo, bool from_thread);

BOOL _send_to_com_port_(const char * Data, const int DataSize);
BOOL _recieve_from_com_port_(char* buffer, int timeout_ms);