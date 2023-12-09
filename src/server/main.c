#include "server/game_server.h"
#include "server/server.h"
#include "utils/collection.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

CNG_GameServer game_server;

void *sending_thread(void *arg) {
	CNG_ServerMessageBuffer message_buffer;
	strcpy(message_buffer.buffer, "123321!");
	message_buffer.size = strlen(message_buffer.buffer);

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

		CNG_CollectionIterator it;
		CNG_CollectionIterator_init(&it);
		while (CNG_CollectionIterator_next(&game_server.client_collection, &it))
			CNG_Server_send(&game_server.server, &message_buffer, it.data);

		pthread_mutex_unlock(&game_server.mutex);
	}
}

int main() {
	CNG_GameServer_init(&game_server, 7878);

	CNG_GameServer_startTicking(&game_server, 1);

	pthread_t sender_tid;
	pthread_create(&sender_tid, NULL, sending_thread, NULL);

	while (1) {
		CNG_ClientAddress      *client_addr = malloc(sizeof(CNG_ClientAddress));
		CNG_ServerMessageBuffer msg_buffer;
		CNG_Server_receive(&game_server.server, &msg_buffer, client_addr);

		printf("Client connected from addr: ");
		print_addr(client_addr->addr.sin_addr.s_addr);
		printf("!\n");

		pthread_mutex_lock(&game_server.mutex);
		CNG_Collection_insert(&game_server.client_collection, client_addr);
		pthread_mutex_unlock(&game_server.mutex);

		if (strcmp(msg_buffer.buffer, "exit") == 0) break;
	}

	pthread_cancel(sender_tid);
	CNG_GameServer_destroy(&game_server);
}
