/**
 * @file game.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#include "game.h"

#include "../event/event.h"
#include "../player/player.h"

#include <pthread.h>

bool player_feature_cmp(void *player_a, void *player_b) {
	PlayerFeatures *a = player_a;
	PlayerFeatures *b = player_b;
	return a->id == b->id;
}

bool Game_init(Game *game, int argc, const char *argv[]) {
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

	CNG_Server_init(&game->server.server);
	CNG_Server_createConnection(&game->server_addr, "localhost", 7878);
	CNG_Collection_init(&game->player_collection, player_feature_cmp);

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
	while (1) {
		pthread_mutex_lock(&game->server.mutex);

		while (game->current_tick > game->server.current_tick)
			pthread_cond_wait(&game->server.condition, &game->server.mutex);

		CNG_Event event;
		event.type           = CNG_EventType_PlayerMove;
		event.move.player_id = game->my_player.id;
		event.move.new_pos.x = game->my_player.position.x;
		event.move.new_pos.y = game->my_player.position.y;
		memcpy(msg.buffer, &event, sizeof(event));

		CNG_Server_send(&game->server.server, &msg, &game->server_addr);

		pthread_mutex_unlock(&game->server.mutex);
	}
}

void *receiving_thread(void *game_void) {
	Game                   *game = game_void;
	CNG_ServerMessageBuffer msg;
	while (1) {
		CNG_Server_receive(&game->server.server, &msg, &game->server_addr);
		// handle receiving
	}
}

void Game_run(Game *game) {
	Player player = Player_init(
		(CNG_Color){ 150, 50, 50 }, (CNG_Vector2f){ .x = 0, .y = 0 }
	);

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
		game->my_player.position = player.rect.position;
		pthread_mutex_unlock(&game->server.mutex);

		CNG_Window_clear(&game->window, BACKGROUND_COLOR);

		Player_draw(&player, &game->window);

		CNG_Window_update(&game->window);
	}
}
