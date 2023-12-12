/**
 * @file game.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#include "game.h"

#include "../event/event.h"

#include <pthread.h>

bool player_feature_cmp(void *player_a, void *player_b) {
	PlayerFeatures *a = player_a;
	PlayerFeatures *b = player_b;
	return a->id == b->id;
}

bool Game_init(Game *game, int argc, const char *argv[]) {
	game->my_player = malloc(sizeof(PlayerFeatures));

	bool success = true;

	CNG_Window window;
	if (!CNG_Window_init(
			&window, "AwesomeShooter", SCREEN_WIDTH, SCREEN_HEIGHT
		))
		success = false;

	if (success) {
		game->window = window;
		game->quit   = false;
	}

	CNG_GameServer_init(&game->server);
	CNG_GameServer_startTicking(&game->server, 128);

	CNG_Server_createConnection(&game->server_addr, "localhost", 7878);
	CNG_Collection_init(&game->player_collection, player_feature_cmp);
	CNG_Collection_insert(&game->player_collection, &game->my_player);

	return success;
}

void Game_destroy(Game *game) {
	CNG_Window_destroy(&game->window);
	CNG_GameServer_destroy(&game->server);
	pthread_cancel(game->sending_thread);
	pthread_cancel(game->receiving_thread);
	CNG_Collection_freeElements(&game->player_collection);
	CNG_Collection_destroy(&game->player_collection);
}

void *sending_thread(void *game_void) {
	Game                   *game = game_void;
	CNG_ServerMessageBuffer msg;
	msg.size = sizeof(CNG_Event);

	while (1) {
		pthread_mutex_lock(&game->server.mutex);

		while (game->current_tick > game->server.current_tick)
			pthread_cond_wait(&game->server.condition, &game->server.mutex);

		CNG_Event event;
		event.type           = CNG_EventType_PlayerMove;
		event.move.player_id = game->my_player->id;
		event.move.new_pos.x = game->my_player->position.x;
		event.move.new_pos.y = game->my_player->position.y;
		memcpy(msg.buffer, &event, sizeof(event));

		CNG_Server_send(&game->server.server, &msg, &game->server_addr);

		pthread_mutex_unlock(&game->server.mutex);
	}
}

void *receiving_thread(void *game_void) {
	Game *game = game_void;

	CNG_ServerMessageBuffer msg;
	msg.size = sizeof(CNG_Event);

	CNG_Event event;

	while (1) {
		CNG_Server_receive(&game->server.server, &msg, &game->server_addr);
		// handle receiving
		memcpy(&event, msg.buffer, sizeof(event));

		if (event.type != CNG_EventType_PlayerMove)
			printf("Received event: %u\n", event.type);
		pthread_mutex_lock(&game->server.mutex);

		switch (event.type) {
		case CNG_EventType_InitFeatures: {
			PlayerFeatures *features = malloc(sizeof(PlayerFeatures));
			memcpy(features, &event.features.features, sizeof(PlayerFeatures));
			if (CNG_Collection_insert(&game->player_collection, features))
				printf("Inserting player with id: %u\n", features->id);
			else
				free(features);
		} break;
		case CNG_EventType_PlayerMove: {
			CNG_CollectionIterator it;
			CNG_CollectionIterator_find(
				&game->player_collection, &event.move.player_id, &it
			);
			if (it.index != -1) {
				PlayerFeatures *features = it.data;
				features->position       = event.move.new_pos;
			}
		} break;
		default:
			break;
		}
		pthread_mutex_unlock(&game->server.mutex);
	}
}

void Game_run(Game *game) {
	Player player = Player_init(
		(CNG_Color){ 150, 50, 50 }, (CNG_Vector2f){ .x = 0, .y = 0 }
	);

	Player players_repr = Player_init(
		(CNG_Color){ 150, 50, 50 }, (CNG_Vector2f){ .x = 0, .y = 0 }
	);
	;

	pthread_create(&game->sending_thread, NULL, sending_thread, game);
	pthread_create(&game->receiving_thread, NULL, receiving_thread, game);

	SDL_Event e;
	Uint32    ticks = SDL_GetTicks();
	while (!game->quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) game->quit = true;
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE) game->quit = true;
			}
		}

		float dt = (float) (SDL_GetTicks() - ticks) / 1000.f;
		ticks    = SDL_GetTicks();

		pthread_mutex_lock(&game->server.mutex);

		Player_update(&player, dt);
		game->my_player->position = player.rect.position;

		CNG_Window_clear(&game->window, BACKGROUND_COLOR);

		CNG_CollectionIterator it;
		CNG_CollectionIterator_init(&it);
		while (CNG_CollectionIterator_next(&game->player_collection, &it)) {
			PlayerFeatures *ft         = it.data;
			players_repr.rect.position = ft->position;
			players_repr.rect.color    = ft->color;
			Player_draw(&players_repr, &game->window);
		}
		pthread_mutex_unlock(&game->server.mutex);

		CNG_Window_update(&game->window);
	}
}
