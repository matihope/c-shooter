#include "game/game.h"

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void init_sockaddr(
	struct sockaddr_in *name, const char *hostname, uint16_t port
) {
	struct hostent *hostinfo;

	name->sin_family = AF_INET;
	name->sin_port   = htons(port);
	hostinfo         = gethostbyname(hostname);
	if (hostinfo == NULL) {
		fprintf(stderr, "Unknown host %s.\n", hostname);
		exit(EXIT_FAILURE);
	}
	name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}

int main(int argc, const char *args[]) {
	int                sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in server_addr;
	init_sockaddr(&server_addr, "localhost", 7878);

	const char *client_hello = "Hello from client!";
	sendto(
		sock_fd,
		client_hello,
		strlen(client_hello),
		MSG_CONFIRM,
		(const struct sockaddr *) &server_addr,
		sizeof(server_addr)
	);

	socklen_t len;
	char      buffer[1024];
	int       n = recvfrom(
        sock_fd,
        buffer,
        sizeof(buffer) / sizeof(buffer[0]),
        MSG_WAITALL,
        (struct sockaddr *) &server_addr,
        &len
    );

	buffer[n] = '\0';
	printf("Server says: %s\n", buffer);

	// return 0;
	Game game;

	if (!GameInit(&game, argc, args)) {
		printf("Failed to initialize!\n");
		return 1;
	}

	GameRun(&game);
	GameDestroy(&game);

	return 0;
}
