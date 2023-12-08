/**
 * @file server.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#include "server.h"

static int create_socket(uint16_t port) {
	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family      = AF_INET;
	server_addr.sin_port        = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	return sock;
}

bool CNG_ServerInit(CNG_Server *server, uint16_t port) {
	server->socket_fd = create_socket(port);
	return true;
}

void CNG_ServerClose(CNG_Server *server) { close(server->socket_fd); }

void CNG_ServerReceive(
	CNG_Server              *server,
	CNG_ServerMessageBuffer *buffer,
	CNG_ClientAddress       *new_client
) {
	new_client->addr_size = sizeof(new_client->addr);
	buffer->size          = recvfrom(
        server->socket_fd,
        buffer->buffer,
        CNG_BUFFER_SIZE,
        0,
        (struct sockaddr *) &new_client->addr,
        &new_client->addr_size
    );
	if (buffer->size < 0) {
		perror("recvfrom (server)");
		exit(EXIT_FAILURE);
	}
}

void CNG_ServerSend(
	CNG_Server              *server,
	CNG_ServerMessageBuffer *message,
	CNG_ClientAddress       *client
) {
	ssize_t n = sendto(
		server->socket_fd,
		message->buffer,
		message->size,
		0,
		(struct sockaddr *) &client->addr,
		client->addr_size
	);

	if (n < 0) {
		perror("sendto (server)");
		exit(EXIT_FAILURE);
	}
}

void print_i32(uint32_t num) {
	char bin_num[33];
	bin_num[32] = '\0';
	for (int i = 0; i < 32; i++) bin_num[i] = '0' + num & (1 << i);

	printf("Number: %s", bin_num);
}
