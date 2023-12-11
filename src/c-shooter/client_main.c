#include "CNG/network.h"
#include "event/event.h"
#include "game/game.h"
#include "player/player.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

int main(int argc, const char *args[]) {
	Game game;
	if (!Game_init(&game, argc, args)) {
		printf("Failed to initialize!\n");
		return 1;
	}

	CNG_ServerMessageBuffer buffer;
	strcpy(buffer.buffer, "connect");
	buffer.size = strlen(buffer.buffer);

	CNG_Server_send(&game.server.server, &buffer, &game.server_addr);
	CNG_Server_receive(&game.server.server, &buffer, &game.server_addr);

	CNG_Event event;
	memcpy(&event, buffer.buffer, sizeof(event));
	printf("My id: %u\n", event.init.new_client_id);

	PlayerFeatures features;
	features.id = event.init.new_client_id;
	srand(features.id);
	features.color = (CNG_Color){
		.r = rand() % 255,
		.g = rand() % 255,
		.b = rand() % 255,
		.a = 255,
	};
	memcpy(&game.my_player, &features, sizeof(features));
	CNG_Collection_insert(&game.player_collection, &features);

	memcpy(buffer.buffer, &features, sizeof(features));
	buffer.size = 256;
	CNG_Server_send(&game.server.server, &buffer, &game.server_addr);

	memset(buffer.buffer, 0, 256);
	//	while (1) {
	//		CNG_Server_receive(&server, &buffer, &server_addr);
	//		printf("Server says: %s\n", buffer.buffer);
	//
	//		CNG_Server_send(&server, &buffer, &server_addr);
	//	}

	Game_run(&game);
	Game_destroy(&game);

	return 0;
}
