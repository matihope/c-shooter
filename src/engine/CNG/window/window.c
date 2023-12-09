/**
 * @file window.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#include "window.h"

bool CNG_Window_init(
	CNG_Window *cng_window,
	const char *title,
	int         window_width,
	int         window_height
) {
	SDL_Window   *window          = NULL;
	SDL_Renderer *window_renderer = NULL;

	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	} else {
		// Create window
		window = SDL_CreateWindow(
			title,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			window_width,
			window_height,
			SDL_WINDOW_SHOWN
		);
		if (window == NULL) {
			printf(
				"Window could not be created! SDL_Error: %s\n", SDL_GetError()
			);
			success = false;
		} else {
			window_renderer = SDL_CreateRenderer(
				window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
			);
		}
	}

	if (success) {
		cng_window->window          = window;
		cng_window->window_renderer = window_renderer;
	}

	return success;
}

void CNG_Window_destroy(CNG_Window *window) {
	SDL_DestroyWindow(window->window);
	window->window = NULL;

	SDL_DestroyRenderer(window->window_renderer);
	window->window_renderer = NULL;

	SDL_Quit();
}

void CNG_Window_clear(CNG_Window *window, CNG_Color color) {
	SDL_SetRenderDrawColor(
		window->window_renderer, color.r, color.g, color.b, color.a
	);
	SDL_RenderClear(window->window_renderer);
}

void CNG_Window_update(CNG_Window *window) {
	SDL_RenderPresent(window->window_renderer);
}
