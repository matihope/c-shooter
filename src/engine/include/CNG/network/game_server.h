/**
 * @file game_server.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "CNG/common/collection.h"
#include "server.h"

typedef struct {
	bool            quit;
	CNG_Server      server;
	uint32_t        current_tick;
	pthread_mutex_t mutex;
	pthread_cond_t  condition;

	pthread_t ticking_thread;
} CNG_GameServer;

int  CNG_GameServer_init(CNG_GameServer *game_server);
void CNG_GameServer_host(CNG_GameServer *game_server, uint32_t port);
void CNG_GameServer_destroy(CNG_GameServer *game_server);
void CNG_GameServer_startTicking(
	CNG_GameServer *game_server, uint32_t tick_rate
);

typedef struct {
} CNG_GameServerClient;

#endif
