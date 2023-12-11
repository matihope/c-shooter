#include "CNG/common/collection.h"
#include "CNG/network.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

CNG_GameServer game_server;
CNG_Collection client_collection;
CNG_Collection player_pos_collection;
pthread_t      sender_tid;

typedef struct {
	uint16_t id;
	float    x, y;
} PlayerPosition;

bool player_pos_cmp(void *player_a, void *player_b) {
	PlayerPosition *a = player_a;
	PlayerPosition *b = player_b;
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

	uint32_t my_tick = 0;
	while (1) {
		pthread_mutex_lock(&game_server.mutex);
		while (my_tick >= game_server.current_tick)
			pthread_cond_wait(&game_server.condition, &game_server.mutex);
		my_tick++;

		sprintf(message_buffer.buffer, "%d", my_tick);
		message_buffer.size = strlen(message_buffer.buffer);

		if (my_tick + 1 < game_server.current_tick)
			printf(
				"Can't keep up! Skipped ticks: %d\n",
				game_server.current_tick - my_tick
			);

		CNG_CollectionIterator it;
		CNG_CollectionIterator_init(&it);
		while (CNG_CollectionIterator_next(&client_collection, &it))
			CNG_Server_send(&game_server.server, &message_buffer, it.data);

		pthread_mutex_unlock(&game_server.mutex);
	}
}

void INThandler(int sig) {
	int c;

	signal(sig, SIG_IGN);
	printf("You hit Ctrl-C - do you really want to quit? [y/n] ");
	c = getchar();
	if (c == 'y' || c == 'Y') {
		pthread_cancel(sender_tid);

		CNG_Collection_freeElements(&client_collection);
		CNG_Collection_destroy(&client_collection);
		CNG_GameServer_destroy(&game_server);

		CNG_Collection_freeElements(&player_pos_collection);
		CNG_Collection_destroy(&player_pos_collection);
		exit(0);
	} else
		signal(SIGINT, INThandler);
	getchar();  // Get new line character
}

int main() {
	CNG_Collection_init(&player_pos_collection, player_pos_cmp);
	CNG_Collection_init(&client_collection, compare_clients);

	CNG_GameServer_init(&game_server);
	CNG_GameServer_host(&game_server, 7878);
	CNG_GameServer_startTicking(&game_server, 128);

	pthread_create(&sender_tid, NULL, sending_thread, NULL);

	signal(SIGINT, INThandler);
	while (1) {
		CNG_Server_Address *client_addr = malloc(sizeof(CNG_Server_Address));
		CNG_ServerMessageBuffer msg_buffer;
		CNG_Server_receive(&game_server.server, &msg_buffer, client_addr);

		pthread_mutex_lock(&game_server.mutex);
		if (CNG_Collection_insert(&client_collection, client_addr)) {
			printf(
				"Client connected with msg \"%s\" from addr: ",
				msg_buffer.buffer
			);
			print_addr(ntohl(client_addr->addr.sin_addr.s_addr));
			printf(":%u", ntohs(client_addr->addr.sin_port));
			printf("!\n");

			uint16_t client_id = CNG_Collection_size(&client_collection);
			printf("New client\'s id: %u\n", client_id);

			CNG_ServerMessageBuffer connect_response;
			memcpy(connect_response.buffer, &client_id, sizeof(client_id));
			connect_response.size = sizeof(client_id);
			CNG_Server_send(
				&game_server.server, &connect_response, client_addr
			);

		} else {
			free(client_addr);
			//			atof();
			printf("Client says: \"%s\"\n", msg_buffer.buffer);
		}

		pthread_mutex_unlock(&game_server.mutex);

		if (strcmp(msg_buffer.buffer, "exit") == 0) break;
	}
}
