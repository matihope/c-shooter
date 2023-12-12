#include "CNG/common/collection.h"
#include "CNG/network.h"
#include "event/event.h"
#include "player/player.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

CNG_GameServer game_server;
CNG_Collection client_collection;
CNG_Collection player_feature_collection;
pthread_t      sender_tid;

bool player_pos_cmp(void *player_a, void *player_b) {
	PlayerFeatures *a = player_a;
	PlayerFeatures *b = player_b;
	return a->id == b->id;
}

static bool compare_clients(void *a, void *b) {
	CNG_Server_Address *client_a = a;
	CNG_Server_Address *client_b = b;
	if (client_a->addr_size != client_b->addr_size) return false;
	if (client_a->addr.sin_addr.s_addr != client_b->addr.sin_addr.s_addr)
		return false;
	if (client_a->addr.sin_port != client_b->addr.sin_port) return false;

	return true;
}

void *sending_thread(void *arg) {
	CNG_ServerMessageBuffer message_buffer;
	message_buffer.size = sizeof(CNG_Event);
	CNG_Event event;

	uint32_t my_tick = 0;
	while (1) {
		pthread_mutex_lock(&game_server.mutex);
		while (my_tick >= game_server.current_tick)
			pthread_cond_wait(&game_server.condition, &game_server.mutex);
		my_tick++;

		if (my_tick + 1 < game_server.current_tick)
			printf(
				"Can't keep up! Skipped ticks: %d\n",
				game_server.current_tick - my_tick
			);


		CNG_CollectionIterator client_addr_it;
		CNG_CollectionIterator_init(&client_addr_it);
		while (CNG_CollectionIterator_next(&client_collection, &client_addr_it)
		) {
			//			printf(
			//				"Sending to client: %u\n",
			//				((CNG_Server_Address *)
			//client_addr_it.data)->addr.sin_port
			//			);
			CNG_CollectionIterator player_pos_it;
			CNG_CollectionIterator_init(&player_pos_it);
			while (CNG_CollectionIterator_next(
				&player_feature_collection, &player_pos_it
			)) {
				PlayerFeatures *ft   = player_pos_it.data;
				event.type           = CNG_EventType_PlayerMove;
				event.move.player_id = ft->id;
				event.move.new_pos   = ft->position;
				memcpy(message_buffer.buffer, &event, sizeof(event));
				CNG_Server_send(
					&game_server.server, &message_buffer, client_addr_it.data
				);
			}
		}


		pthread_mutex_unlock(&game_server.mutex);
	}
}

void INThandler(int sig) {
	printf("Shutting the server down...\n");
	signal(sig, SIG_IGN);
	pthread_cancel(sender_tid);

	CNG_Collection_freeElements(&client_collection);
	CNG_Collection_destroy(&client_collection);
	CNG_GameServer_destroy(&game_server);

	CNG_Collection_freeElements(&player_feature_collection);
	CNG_Collection_destroy(&player_feature_collection);
	exit(0);
}

int main() {
	CNG_Collection_init(&player_feature_collection, player_pos_cmp);
	CNG_Collection_init(&client_collection, compare_clients);

	CNG_GameServer_init(&game_server);
	CNG_GameServer_host(&game_server, 7878);
	CNG_GameServer_startTicking(&game_server, 128);

	pthread_create(&sender_tid, NULL, sending_thread, NULL);


	signal(SIGINT, INThandler);
	signal(SIGTERM, INThandler);
	CNG_Event               event;
	CNG_ServerMessageBuffer msg_buffer;
	msg_buffer.size = sizeof(CNG_Event);

	while (1) {
		CNG_Server_Address *client_addr = malloc(sizeof(CNG_Server_Address));
		CNG_Server_receive(&game_server.server, &msg_buffer, client_addr);
		memcpy(&event, msg_buffer.buffer, sizeof(event));

		pthread_mutex_lock(&game_server.mutex);
		if (CNG_Collection_insert(&client_collection, client_addr)) {
			// Display new client's address
			printf("Client connected from addr: ");
			print_addr(ntohl(client_addr->addr.sin_addr.s_addr));
			printf(":%u", ntohs(client_addr->addr.sin_port));
			printf("!\n");

			// Send them their new id
			CNG_Event response_event;
			response_event.type = CNG_EventType_Init;
			response_event.init.new_client_id
				= CNG_Collection_size(&client_collection);
			printf("New client\'s id: %u\n", response_event.init.new_client_id);
			memcpy(msg_buffer.buffer, &response_event, sizeof(CNG_Event));
			CNG_Server_send(&game_server.server, &msg_buffer, client_addr);

			// Send other clients' features
			response_event.type = CNG_EventType_InitFeatures;
			CNG_CollectionIterator it;
			CNG_CollectionIterator_init(&it);
			while (CNG_CollectionIterator_next(&player_feature_collection, &it)
			) {
				memcpy(
					&response_event.features, it.data, sizeof(PlayerFeatures)
				);
				memcpy(&msg_buffer.buffer, &response_event, sizeof(CNG_Event));
				CNG_Server_send(&game_server.server, &msg_buffer, client_addr);
			}
		} else {
			free(client_addr);
		}

		if (event.type != 2) printf("Event type %u \n", event.type);

		switch (event.type) {
		case CNG_EventType_Init:
			break;
		case CNG_EventType_InitFeatures: {
			// Store the features
			printf(
				"Inserting features of player: %u\n", event.features.features.id
			);
			PlayerFeatures *ft = malloc(sizeof(PlayerFeatures));
			memcpy(ft, &event.features, sizeof(PlayerFeatures));
			CNG_Collection_insert(&player_feature_collection, ft);

			// Notify all the clients about the new client's features
			CNG_CollectionIterator it;
			CNG_CollectionIterator_init(&it);
			memcpy(&msg_buffer.buffer, &event, sizeof(event));
			while (CNG_CollectionIterator_next(&client_collection, &it))
				CNG_Server_send(&game_server.server, &msg_buffer, it.data);
		} break;
		case CNG_EventType_PlayerMove: {
			CNG_CollectionIterator it;
			CNG_CollectionIterator_find(
				&player_feature_collection, &event.move.player_id, &it
			);
			if (it.index != -1) {
				PlayerFeatures *playerFeatures = it.data;
				playerFeatures->position       = event.move.new_pos;
			}
		} break;
		}

		pthread_mutex_unlock(&game_server.mutex);
	}
}
