/**
 * @file game_server.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#pragma once
#include "../utils/collection.h"
#include "server.h"

typedef struct {
	bool            quit;
	CNG_Server      server;
	uint32_t        current_tick;
	CNG_Collection  client_collection;
	pthread_mutex_t mutex;
	pthread_cond_t  condition;

	pthread_t ticking_thread;
} CNG_GameServer;

int  CNG_GameServer_init(CNG_GameServer *game_server, uint32_t port);
void CNG_GameServer_destroy(CNG_GameServer *game_server);
void CNG_GameServer_startTicking(
	CNG_GameServer *game_server, uint32_t tick_rate
);
