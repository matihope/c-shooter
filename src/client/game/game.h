/**
 * @file game.hpp
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#pragma once

#include "CNG/window/window.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define BACKGROUND_COLOR \
	(CNG_Color) { .r = 21, .g = 21, .b = 21, .a = 255 }

typedef struct {
	CNG_Window window;
	bool       quit;
} Game;

bool GameInit(Game *game, int argc, const char *argv[]);

void GameDestroy(Game *game);

void GameRun(Game *game);
