/**
 * @file game_server.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#include "game_server.h"

#include "../utils/timer.h"

#include <pthread.h>

static bool compare_clients(void *a, void *b) {
	CNG_ClientAddress *client_a = a;
	CNG_ClientAddress *client_b = b;
	if (client_a->addr_size != client_b->addr_size) return false;
	if (client_a->addr.sin_addr.s_addr != client_b->addr.sin_addr.s_addr)
		return false;
	if (client_a->addr.sin_port != client_b->addr.sin_port) return false;

	return true;
}

int CNG_GameServer_init(CNG_GameServer *game_server, uint32_t port) {
	game_server->quit = false;
	if (pthread_mutex_init(&game_server->mutex, NULL) != 0) {
		printf("\nmutex init failed\n");
		return -1;
	}

	if (pthread_cond_init(&game_server->condition, NULL) != 0) {
		printf("\ncond init failed\n");
		return -1;
	}

	CNG_Collection_create(&game_server->client_collection, compare_clients);

	if (CNG_Server_init(&game_server->server, port) != 0) return -1;

	return 0;
}

void CNG_GameServer_destroy(CNG_GameServer *game_server) {
	pthread_cancel(game_server->ticking_thread);

	pthread_mutex_destroy(&game_server->mutex);
	pthread_cond_destroy(&game_server->condition);

	// Free the allocated clients
	CNG_CollectionIterator it;
	CNG_CollectionIterator_init(&it);
	while (CNG_CollectionIterator_next(&game_server->client_collection, &it))
		free(it.data);
	CNG_Collection_destroy(&game_server->client_collection);

	CNG_Server_close(&game_server->server);
}

static int server_tick_loop(uint32_t tick_number, void *game_server_void) {
	CNG_GameServer *game_server = game_server_void;

	pthread_mutex_lock(&game_server->mutex);

	game_server->current_tick = tick_number;
	if (game_server->current_tick % 10 == 0)
		printf("Tick %u\n", game_server->current_tick);
	pthread_cond_broadcast(&game_server->condition);

	bool quit = game_server->quit;
	pthread_mutex_unlock(&game_server->mutex);

	return quit;
}

static void *server_tick_thread(void *arg) {
	void          **args        = arg;
	CNG_GameServer *game_server = args[0];
	uint32_t        tick_rate   = args[1];
	free(arg);

	printf("Starting server tick thread...\n");
	CNG_startTimerWithFrequency(tick_rate, game_server, server_tick_loop);

	pthread_exit(NULL);
}

void CNG_GameServer_startTicking(
	CNG_GameServer *game_server, uint32_t tick_rate
) {
	// This is probably not the best way to pass values around, but it is a bit
	// interesting, at least to me :D
	// We are passing tick_rate by value to the thread by storing it in the
	// memory next to a pointer to CNG_GameServer.
	// This is very fragile... but fun ;D
	void **args = malloc(sizeof(void *) + sizeof(tick_rate));
	args[0]     = game_server;
	args[1]     = tick_rate;

	pthread_create(
		&game_server->ticking_thread, NULL, server_tick_thread, args
	);
}
