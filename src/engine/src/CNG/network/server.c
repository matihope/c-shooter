/**
 * @file server.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#include "CNG/network/server.h"

#include <assert.h>
#include <netdb.h>

int CNG_Server_init(CNG_Server *server) {
	server->socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (server->socket_fd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	return 0;
}

int CNG_Server_host(CNG_Server *server, uint16_t port) {
	struct sockaddr_in server_addr;
	server_addr.sin_family      = AF_INET;
	server_addr.sin_port        = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(
			server->socket_fd,
			(struct sockaddr *) &server_addr,
			sizeof(server_addr)
		)
	    < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	return 0;
}

void CNG_Server_close(CNG_Server *server) { close(server->socket_fd); }

void CNG_Server_receive(
	CNG_Server              *server,
	CNG_ServerMessageBuffer *buffer,
	CNG_Server_Address      *addr
) {
	addr->addr_size = sizeof(addr->addr);
	assert(buffer->size <= CNG_BUFFER_SIZE);
	buffer->size = recvfrom(
		server->socket_fd,
		buffer->buffer,
		buffer->size,
		0,
		(struct sockaddr *) &addr->addr,
		&addr->addr_size
	);
	if (buffer->size < 0) {
		perror("recvfrom (server)");
		exit(EXIT_FAILURE);
	}
}

void CNG_Server_send(
	CNG_Server              *server,
	CNG_ServerMessageBuffer *message,
	CNG_Server_Address      *addr
) {
	ssize_t n = sendto(
		server->socket_fd,
		message->buffer,
		message->size,
		0,
		(struct sockaddr *) &addr->addr,
		addr->addr_size
	);

	if (n < 0) {
		perror("sendto (server)");
		exit(EXIT_FAILURE);
	}
}

void print_addr(uint32_t num) {
	char bin_num[33];
	bin_num[32] = '\0';
	for (int i = 0; i < 32; i++) bin_num[31 - i] = (num >> i & 1) + '0';

	printf(
		"%u.%u.%u.%u",
		num >> 24,
		num << 8 >> 24,
		num << 16 >> 24,
		num << 24 >> 24
	);

	//	printf(
	//		"%u == %s == %u.%u.%u.%u",
	//		num,
	//		bin_num,
	//		num >> 24,
	//		num << 8 >> 24,
	//		num << 16 >> 24,
	//		num << 24 >> 24
	//	);
}

int CNG_Server_createConnection(
	CNG_Server_Address *addr, const char *hostname, uint16_t port
) {
	struct hostent *host_info;

	addr->addr.sin_family = AF_INET;
	addr->addr.sin_port   = htons(port);
	host_info             = gethostbyname2(hostname, AF_INET);
	if (host_info == NULL) {
		fprintf(stderr, "Unknown host %s.\n", hostname);
		exit(EXIT_FAILURE);
	}
	addr->addr.sin_addr = *(struct in_addr *) host_info->h_addr_list[0];
	addr->addr_size     = sizeof(addr->addr);
	return 0;
}
