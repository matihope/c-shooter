/**
 * @file server.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#ifndef SERVER_H
#define SERVER_H

#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void print_addr(uint32_t num);

typedef struct {
	int socket_fd;
} CNG_Server;

#define CNG_BUFFER_SIZE 256

typedef struct {
	char    buffer[CNG_BUFFER_SIZE];
	ssize_t size;
} CNG_ServerMessageBuffer;

typedef struct {
	struct sockaddr_in addr;
	socklen_t          addr_size;
} CNG_Server_Address;

int  CNG_Server_init(CNG_Server *server);
int  CNG_Server_host(CNG_Server *server, uint16_t port);
void CNG_Server_close(CNG_Server *server);
int  CNG_Server_createConnection(
	 CNG_Server_Address *server, const char *hostname, uint16_t port
 );

void CNG_Server_receive(
	CNG_Server              *server,
	CNG_ServerMessageBuffer *buffer,
	CNG_Server_Address      *addr
);

void CNG_Server_send(
	CNG_Server              *server,
	CNG_ServerMessageBuffer *message,
	CNG_Server_Address      *addr
);

#endif
