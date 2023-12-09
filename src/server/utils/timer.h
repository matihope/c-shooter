/**
 * @file timer.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#pragma once
#include <stdint.h>

void CNG_startTimerWithFrequency(
	uint32_t tick_rate, void *arg, int (*callback)(uint32_t, void *)
);
