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

bool CNG_WindowInit(
	CNG_Window *cng_window,
	const char *title,
	int         window_width,
	int         window_height
);

void CNG_WindowDestroy(CNG_Window *window);

void CNG_WindowClear(CNG_Window *window, CNG_Color color);

void CNG_WindowUpdate(CNG_Window *window);
