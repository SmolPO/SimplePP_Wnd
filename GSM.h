#pragma once
#include <vector>
#include <queue>
#include <WinSock2.h>
#include <stddef.h>

#include "structs.h"
///����
typedef unsigned char BYTE;
using namespace std;

//���������� ������
static SOCKET Server;

static BOOL is_connect_to_server = false;
///�����

//--------------------------//
///���������� ����������
static BOOL is_thread_prog = true;

///���������������
#define ZERO_STRUCT_MEMORY(pst) ZeroMemory(pst, sizeof(*(pst))) 
#define ZERO_STRUCT_ARRAY_MEMORY(pst, count) ZeroMemory(pst, count*sizeof(pst[0])) 
#define COPY_STRUCTS_ARRAY(pDst, pSrc, count) CopyMemory(pDst, pSrc, count*sizeof(pDst[0])) 
#define SAFE_DELETE_POINTER(p) { if (p) delete p; p = NULL; } 
#define SAFE_DELETE_ARRAY_POINTER(p) { if (p) delete [] p; p = NULL; }
#define CMD_UNKNOW  -2
#define MY_ERROR  perror("������");
#define RETURN_SOCK_ERROR return -1;
#define S_ERROR -1
#define STR_LEN(x) sizeof(char)*strlen(x)
#define GL_MEM_SET(x, y, z) MEM_SET(x) MEM_SET(y) MEM_SET(z)
#define MEM_SET(x) memset(x, 0, sizeof(char) * SIZE_BUFFER);

///COM-���� -> config


//������
#define MY_MY_ERROR -1
#define ENTER_CRITICAL_SECTION EnterCriticalSection(&my_cs); 
#define LEAVE_CRITICAL_SECTION LeaveCriticalSection(&my_cs); 
#define char_END_SYMBOL_26 char EndSymbol = END_SEND_SYMBOL_26;
#define SIZE_CMD_OR_ANSWER 100

///��������
static int cnt_command = 0;
static int cnt_answer = 0;
static int cnt_servers_msgs = 0;
static int next_id = 0;

//�������
extern HANDLE  h_event;
extern RTL_CRITICAL_SECTION my_cs;

//��� COM
extern DWORD dw_param, dw_thread_id;
extern DCB dcb;
extern FILE* f_out;
extern COMMTIMEOUTS time_outs;
extern OVERLAPPED m_ovTx, m_ovRx;

//������
extern DWORD bytes_read, bWritten;
extern BOOL f_success;
extern HANDLE h_thread, h_com;
static BOOL is_echo = false;
static BOOL is_init = false;
static string self_ip = "0.0.0.0";
static BOOL close = false;

//������ init
int init();
void set_time_outs();
void connect_to_COM(DCB* dcb);

//����������� ���������
BOOL send_new_command_server();
//bool send_new_command_lamps();

// ����� �������
//bool connect_to_server();
DWORD WINAPI listen_server(void*);
BOOL listen_lamps();
BOOL close_programm(); // TODO!
int _get_console_data(const char* mess);
BOOL _get_cmd_and_data(int* cmd, int* data);

//��������� ������