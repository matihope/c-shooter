#include "CNG/network.h"
#include "event/event.h"
#include "game/game.h"
#include "player/player.h"

#include <stdio.h>
#include <string.h>

int main(int argc, const char *args[]) {
	Game game;
	if (!Game_init(&game, argc, args)) {
		printf("Failed to initialize!\n");
		return 1;
	}

	CNG_Event event;
	event.type               = CNG_EventType_Init;
	event.init.new_client_id = -1;
	CNG_ServerMessageBuffer buffer;
	buffer.size = sizeof(event);

	CNG_Server_send(&game.server.server, &buffer, &game.server_addr);
	CNG_Server_receive(&game.server.server, &buffer, &game.server_addr);

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
	memcpy(game.my_player, &features, sizeof(features));
	CNG_Collection_insert(&game.player_collection, game.my_player);

	// Notify the server about my features
	event.type = CNG_EventType_InitFeatures;
	memcpy(&event.features, &features, sizeof(features));
	memcpy(buffer.buffer, &event, sizeof(event));
	CNG_Server_send(&game.server.server, &buffer, &game.server_addr);

	Game_run(&game);
	Game_destroy(&game);

	return 0;
}
