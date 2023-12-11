/**
 * @file game.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#include "game.h"

#include "../player/player.h"

#include <pthread.h>

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

	return success;
}

void Game_destroy(Game *game) { CNG_Window_destroy(&game->window); }

void Game_run(Game *game) {
	Player player = Player_init(
		(CNG_Color){ 150, 50, 50 }, (CNG_Vector2f){ .x = 0, .y = 0 }
	);

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
		pthread_mutex_unlock(&game->server.mutex);

		CNG_Window_clear(&game->window, BACKGROUND_COLOR);

		Player_draw(&player, &game->window);

		CNG_Window_update(&game->window);
	}
}
