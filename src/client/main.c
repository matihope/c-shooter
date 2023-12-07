#include "game/game.h"

#include <stdio.h>

int main(int argc, const char *args[]) {
	Game game;

	if (!GameInit(&game, argc, args)) {
		printf("Failed to initialize!\n");
		return 1;
	}

	GameRun(&game);
	GameDestroy(&game);

	return 0;
}
