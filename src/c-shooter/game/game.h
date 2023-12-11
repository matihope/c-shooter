/**
 * @file game.hpp
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#ifndef GAME_H
#define GAME_H

#include "../player/player.h"
#include "CNG/graphics.h"
#include "CNG/network.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define BACKGROUND_COLOR \
	(CNG_Color) { .r = 21, .g = 21, .b = 21, .a = 255 }

typedef struct {
	CNG_Window         window;
	bool               quit;
	uint32_t           current_tick;
	CNG_GameServer     server;
	CNG_Server_Address server_addr;
	pthread_t          sending_thread;
	pthread_t          receiving_thread;

	PlayerFeatures my_player;
	CNG_Collection player_collection;  // of PlayerFeatures
} Game;

bool Game_init(Game *game, int argc, const char *argv[]);

void Game_destroy(Game *game);

void Game_run(Game *game);

#endif
