#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<Winsock2.h>
#include<ws2tcpip.h>
#include<windows.h>

#define IPPROT_TCP 6
#define port 8888
#define data_length 81

FILE* file;

SOCKET server_socket , client_socket;
struct sockaddr_in server_addr , client_addr;

typedef struct{
	int operation;
	char collection[data_length];
	char *data[100];
	char *conditions[100];
	char *fields[100];
}TRAME;
TRAME trame;
