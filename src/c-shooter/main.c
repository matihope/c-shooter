#include "CNG/network.h"
#include "game/game.h"
#include "player/player.h"

#include <stdio.h>
#include <string.h>

int main(int argc, const char *args[]) {
	CNG_Server server;
	CNG_Server_init(&server);

	CNG_Server_Address server_addr;
	CNG_Server_createConnection(&server_addr, "localhost", 7878);

	CNG_ServerMessageBuffer buffer;
	strcpy(buffer.buffer, "connect");
	buffer.size = strlen(buffer.buffer);

	CNG_Server_send(&server, &buffer, &server_addr);
	CNG_Server_receive(&server, &buffer, &server_addr);

	uint16_t my_id;
	memcpy(&my_id, buffer.buffer, sizeof(my_id));
	printf("My id: %u\n", my_id);

	PlayerFeatures features;
	features.id = my_id;
	srand(my_id);
	features.color = (CNG_Color){
		.r = rand() % 255,
		.g = rand() % 255,
		.b = rand() % 255,
		.a = 255,
	};

	memcpy(buffer.buffer, &features, sizeof(features));
	CNG_Server_send(&server, &buffer, &server_addr);

	memset(buffer.buffer, 0, 256);
	while (1) {
		CNG_Server_receive(&server, &buffer, &server_addr);
		printf("Server says: %s\n", buffer.buffer);

		CNG_Server_send(&server, &buffer, &server_addr);
	}
	Game game;

	if (!Game_init(&game, argc, args)) {
		printf("Failed to initialize!\n");
		return 1;
	}

	Game_run(&game);
	Game_destroy(&game);

	return 0;
}
