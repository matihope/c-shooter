/**
 * @file window.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#include "CNG/utils/utils.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

#pragma once

typedef struct {
	SDL_Window   *window;
	SDL_Renderer *window_renderer;
} CNG_Window;

bool CNG_Window_init(
	CNG_Window *cng_window,
	const char *title,
	int         window_width,
	int         window_height
);

void CNG_Window_destroy(CNG_Window *window);

void CNG_Window_clear(CNG_Window *window, CNG_Color color);

void CNG_Window_update(CNG_Window *window);
