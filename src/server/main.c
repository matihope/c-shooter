#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main() {
	int                sock;
	struct sockaddr_in server_addr;

	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		switch (errno) {
		case EPROTONOSUPPORT:
			fprintf(
				stderr,
				"The protocol or style is not supported by the "
				"namespace specified."
			);
			break;
		case EMFILE:
			fprintf(
				stderr,
				"The process already has too many file descriptors open."
			);
			break;
		case ENFILE:
			fprintf(
				stderr, "The system already has too many file descriptors open."
			);
			break;
		case EACCES:
			fprintf(
				stderr,
				"The process does not have the privilege to create a socket of "
				"the specified style or protocol."
			);
			break;
		case ENOBUFS:
			fprintf(stderr, "The system ran out of internal buffer space.");
			break;
		}
		perror("socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family      = AF_INET;
	server_addr.sin_port        = htons(7878);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	socklen_t len;
	int       n;

	struct sockaddr_in client_addr;
	len = sizeof(client_addr);
	char buffer[1024];

	n = recvfrom(
		sock,
		buffer,
		sizeof(buffer) / sizeof(buffer[0]),
		0,
		(struct sockaddr *) &client_addr,
		&len
	);

	buffer[n] = '\0';
	printf("Client : %s\n", buffer);

	const char *hello = "Hello from server!";
	sendto(
		sock,
		hello,
		strlen(hello),
		MSG_CONFIRM,
		(const struct sockaddr *) &client_addr,
		len
	);
	printf("Hello message sent...\n");

	close(sock);
}
