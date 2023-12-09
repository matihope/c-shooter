/**
 * @file timer.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#include "timer.h"

#include <sys/time.h>
#include <unistd.h>

void CNG_startTimerWithFrequency(
	uint32_t tick_rate, int (*callback)(uint32_t)
) {
	const double suspend_time_seconds = 1.0 / (double) tick_rate;

	uint32_t   server_ticks = 0;
	useconds_t to_sleep;

	struct timeval tv;
	struct timeval new_tv;
	gettimeofday(&tv, NULL);

	// Time point at which we should update
	double next_update = suspend_time_seconds;

	while (1) {
		gettimeofday(&new_tv, NULL);
		new_tv.tv_sec -= tv.tv_sec;
		new_tv.tv_usec -= tv.tv_usec;
		double seconds = (float) new_tv.tv_sec + (float) new_tv.tv_usec / 1e6f;

		if (callback(server_ticks++)) break;

		to_sleep = (useconds_t) ((next_update - seconds) * 1e6);
		usleep(to_sleep);
		next_update += suspend_time_seconds;

		//		printf(
		//			"Ticks: %d, tick rate: %f tps, seconds: %fs\n",
		//			server_ticks,
		//			(float) server_ticks / seconds,
		//			seconds
		//		);
	}
}
