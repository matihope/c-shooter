/**
 * @file timer.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#pragma once
#include <stdint.h>

void CNG_StartTimerWithFrequency(uint32_t tick_rate, int (*callback)(uint32_t));
