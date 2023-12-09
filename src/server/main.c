#include "server/server.h"
#include "utils/collection.h"
#include "utils/timer.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

CNG_Collection client_collection;
CNG_Server     server;

CNG_ServerMessageBuffer message_buffer;

uint32_t        current_tick = 0;
pthread_mutex_t mutex;
pthread_cond_t  cond;

void *sending_thread(void *arg) {
	uint32_t my_tick = 0;
	while (1) {
		pthread_mutex_lock(&mutex);
		while (my_tick >= current_tick) pthread_cond_wait(&cond, &mutex);
		my_tick++;

		CNG_CollectionIterator it = CNG_CollectionIterator_init();
		while (CNG_CollectionIterator_next(&client_collection, &it))
			CNG_Server_send(&server, &message_buffer, it.data);

		pthread_mutex_unlock(&mutex);
	}
}

int server_tick_loop(uint32_t tick_number) {
	pthread_mutex_lock(&mutex);
	current_tick = tick_number;
	if (tick_number % 10 == 0) printf("Tick %u\n", tick_number);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);
	return false;
}

void *main_server_thread(void *tid) {
	printf("Started main server thread with tid: %lu...\n", *(pthread_t *) tid);

	CNG_startTimerWithFrequency(1, server_tick_loop);

	pthread_exit(NULL);
}

bool compare_clients(void *a, void *b) {
	CNG_ClientAddress *client_a = a;
	CNG_ClientAddress *client_b = b;
	if (client_a->addr_size != client_b->addr_size) return false;
	if (client_a->addr.sin_addr.s_addr != client_b->addr.sin_addr.s_addr)
		return false;
	if (client_a->addr.sin_port != client_b->addr.sin_port) return false;

	return true;
}

int main() {
	if (pthread_mutex_init(&mutex, NULL) != 0) {
		printf("\nmutex init failed\n");
		return 1;
	}

	if (pthread_cond_init(&cond, NULL) != 0) {
		printf("\ncond init failed\n");
		return 1;
	}

	strcpy(message_buffer.buffer, "123");
	message_buffer.size = strlen(message_buffer.buffer);

	CNG_Collection_create(&client_collection, compare_clients);

	CNG_Server_init(&server, 7878);

	pthread_t tick_tid;
	pthread_create(&tick_tid, NULL, main_server_thread, &tick_tid);
	pthread_t sender_tid;
	pthread_create(&sender_tid, NULL, sending_thread, NULL);

	while (1) {
		CNG_ClientAddress      *client_addr = malloc(sizeof(CNG_ClientAddress));
		CNG_ServerMessageBuffer msg_buffer;
		CNG_Server_receive(&server, &msg_buffer, client_addr);

		printf("Client connected!\n");

		pthread_mutex_lock(&mutex);
		CNG_Collection_insert(&client_collection, client_addr);
		pthread_mutex_unlock(&mutex);

		if (strcmp(msg_buffer.buffer, "exit") == 0) break;
	}

	pthread_cancel(tick_tid);
	pthread_cancel(sender_tid);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

	// Free the allocated clients
	CNG_CollectionIterator it = CNG_CollectionIterator_init();
	while (CNG_CollectionIterator_next(&client_collection, &it)) free(it.data);
	CNG_Collection_destroy(&client_collection);

	CNG_Server_close(&server);
}
