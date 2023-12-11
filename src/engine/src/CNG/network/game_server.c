/**
 * @file game_server.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#include "CNG/network/game_server.h"

#include "CNG/common/timer.h"

#include <pthread.h>

int CNG_GameServer_init(CNG_GameServer *game_server) {
	game_server->quit = false;
	if (pthread_mutex_init(&game_server->mutex, NULL) != 0) {
		printf("\nmutex init failed\n");
		return -1;
	}

	if (pthread_cond_init(&game_server->condition, NULL) != 0) {
		printf("\ncond init failed\n");
		return -1;
	}


	if (CNG_Server_init(&game_server->server) != 0) return -1;

	return 0;
}

void CNG_GameServer_host(CNG_GameServer *game_server, uint32_t port) {
	CNG_Server_host(&game_server->server, port);
}

void CNG_GameServer_destroy(CNG_GameServer *game_server) {
	pthread_cancel(game_server->ticking_thread);

	pthread_mutex_destroy(&game_server->mutex);
	pthread_cond_destroy(&game_server->condition);

	CNG_Server_close(&game_server->server);
}

static int server_tick_loop(uint32_t tick_number, void *game_server_void) {
	CNG_GameServer *game_server = game_server_void;

	pthread_mutex_lock(&game_server->mutex);
	game_server->current_tick = tick_number;
	pthread_cond_broadcast(&game_server->condition);
	bool quit = game_server->quit;
	pthread_mutex_unlock(&game_server->mutex);

	return quit;
}

static void *server_tick_thread(void *arg) {
	void          **args        = arg;
	CNG_GameServer *game_server = args[0];
	uint32_t        tick_rate;
	memcpy(&tick_rate, args[1], sizeof(tick_rate));
	free(args[1]);
	free(arg);

	printf("Starting server tick thread...\n");
	CNG_startTimerWithFrequency(tick_rate, game_server, server_tick_loop);

	pthread_exit(NULL);
}

void CNG_GameServer_startTicking(
	CNG_GameServer *game_server, uint32_t tick_rate
) {
	void **args = malloc(sizeof(void *) + sizeof(tick_rate));
	args[0]     = game_server;
	args[1]     = malloc(sizeof(tick_rate));
	memcpy(args[1], &tick_rate, sizeof(tick_rate));

	pthread_create(
		&game_server->ticking_thread, NULL, server_tick_thread, args
	);
}
