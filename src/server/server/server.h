/**
 * @file server.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#pragma once

#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void print_i32(uint32_t num);

typedef struct {
	int socket_fd;
} CNG_Server;

#define CNG_BUFFER_SIZE 1024

typedef struct {
	char    buffer[CNG_BUFFER_SIZE];
	ssize_t size;
} CNG_ServerMessageBuffer;

typedef struct {
	struct sockaddr_in addr;
	socklen_t          addr_size;
} CNG_ClientAddress;

bool CNG_ServerInit(CNG_Server *server, uint16_t port);
void CNG_ServerClose(CNG_Server *server);

void CNG_ServerReceive(
	CNG_Server              *server,
	CNG_ServerMessageBuffer *buffer,
	CNG_ClientAddress       *new_client
);

void CNG_ServerSend(
	CNG_Server              *server,
	CNG_ServerMessageBuffer *message,
	CNG_ClientAddress       *client
);
