/**
 * @file game_server.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#pragma once
#include "../utils/collection.h"
#include "server.h"

typedef struct {
	CNG_Server      server;
	uint32_t        current_tick;
	CNG_Collection  client_collection;
	pthread_mutex_t mutex;
	pthread_cond_t  condition;
} CNG_GameServer;

bool CNG_GameServer_init();
