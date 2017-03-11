#pragma once
#define AT "\r\nAT\r\n"
#define ATE0 "\r\nATE0\r\n"
#define ATE1 "\r\nATE1\r\n"
#define ATV0 "\r\nATV0\r\n"
#define ATV1 "\r\nATV1\r\n"
#define CIPMUX "\r\nAT+CIPMUX=0\r\n"
#define CIPSHUT "\r\nAT+CIPSHUT\r\n"
#define IS_TRANSP "\r\nAT+CIPMODE=1\r\n"
#define NOT_TRANSP "\r\nAT+CIPMODE=0\r\n"
#define CIPSTATUS "\r\nAT+CIPSTATUS\r\n"
#define CONNECT_INET "\r\nAT+CSTT=\"internet.tele2.ru\",\"tele2\",\"tele2\"\r\n" 
//at+cstt="internet.tele2.ru","tele2","tele2"
#define CIPSTART "\r\nAT+CIPSTART=\"TCP\",\"5.145.160.40\",\"27000\"\r\n" 
//at+cipstart="tcp","5.145.160.40","27000"
#define CIICR "\r\nAT+CIICR\r\n"
#define GET_IP "\r\nAT+CIFSR\r\n"
#define CIPSEND "\r\nAT+CIPSEND=8\r\n"
//#define CIPSEND(cmd, x,y) sprintf(cmd, "\r\nAT+CIPSEND=%d,%d\r\n", x, y)
#define END_SEND_SYMBOL_26 26
#define END_SYMBOL_13 13
#define END_SYMBOL_10 10
#define AT_ON_LIGHT "\r\nAT+CNETLIGHT=1\r\n"
#define AT_OFF_LIGHT "\r\nAT+CNETLIGHT=0\r\n"

#define WRITE_MESSAGE "\r\n> " 
#define OK "\r\nOK\r\n"
#define ECHO_OK "\r\nAT\r\n\r\nOK\r\n"
#define CONN_OK "\r\nCONNECT\r\n"
#define ST_INITIAL "\r\nSTATE: IP INITIAL\r\n"
#define SEND_OK "\r\nSEND OK\r\n" 
#define SHUT_OK "\r\nSHUT OK\r\n"

#define W_ONE 1
#define W_TWO 2
#define W_THREE 3

static char* ñommands_AT[100] = { 0 };
static char* answer_AT[100] = { 0 };
