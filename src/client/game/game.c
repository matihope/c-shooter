/**
 * @file game.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#include "game.h"

#include "../player/player.h"

#include <CNG/CNG.h>

bool GameInit(Game *game, int argc, const char **argv) {
	bool success = true;

	CNG_Window window;
	if (!CNG_WindowInit(&window, "AwesomeShooter", SCREEN_WIDTH, SCREEN_HEIGHT))
		success = false;

	if (success) {
		game->window = window;
		game->quit   = false;
	}

	return success;
}

void GameDestroy(Game *game) { CNG_WindowDestroy(&game->window); }

void GameRun(Game *game) {
	Player player = PlayerInit(
		(CNG_Color){ 150, 50, 50 }, (CNG_Vector2f){ .x = 0, .y = 0 }
	);

	SDL_Event e;
	Uint32    ticks = SDL_GetTicks();
	while (game->quit == false) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) game->quit = true;
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE) game->quit = true;
			}
		}

		float dt = (float) (SDL_GetTicks() - ticks) / 1000.f;
		ticks    = SDL_GetTicks();

		PlayerUpdate(&player, dt);

		CNG_WindowClear(&game->window, BACKGROUND_COLOR);

		PlayerDraw(&player, &game->window);

		CNG_WindowUpdate(&game->window);
	}
}
