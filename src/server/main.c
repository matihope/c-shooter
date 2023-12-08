#include "server/server.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

int main() {
	CNG_Server server;
	CNG_ServerInit(&server, 7878);

	CNG_ServerMessageBuffer msg_buffer;
	CNG_ClientAddress       client_addr;

	const uint32_t tick_rate            = 64;
	const float    suspend_time_seconds = 1.f / (float) tick_rate;
	const uint32_t suspend_time_microseconds
		= (uint32_t) (suspend_time_seconds * 1e6);

	uint32_t   server_ticks = 0;
	useconds_t to_sleep     = suspend_time_microseconds;

	struct timeval tv;
	struct timeval new_tv;
	gettimeofday(&tv, NULL);

	// Time point at which we should update
	float next_update = 0;

	while (1) {
		gettimeofday(&new_tv, NULL);
		new_tv.tv_sec -= tv.tv_sec;
		new_tv.tv_usec -= tv.tv_usec;
		float seconds = (float) new_tv.tv_sec + (float) new_tv.tv_usec / 1e6f;

		if (seconds >= next_update) {
			// TICK!

			server_ticks++;
			printf(
				"Ticks: %d, tick rate: %f tps\n",
				server_ticks,
				(float) server_ticks / seconds
			);
			next_update += suspend_time_seconds;
		} else {
			printf("Error\n");
		}

		to_sleep = (useconds_t) ((next_update - seconds) * 1e6);
		usleep(to_sleep);
	}

	while (1) {
		CNG_ServerReceive(&server, &msg_buffer, &client_addr);

		msg_buffer.buffer[msg_buffer.size] = '\0';
		printf("Client : %s\n", msg_buffer.buffer);

		CNG_ServerMessageBuffer answer;
		strcpy(answer.buffer, "Hello from server");
		answer.size = strlen(answer.buffer);
		CNG_ServerSend(&server, &answer, &client_addr);
		printf("Message sent!\n");

		if (strcmp(msg_buffer.buffer, "exit") == 0) break;
	}

	CNG_ServerClose(&server);
}
