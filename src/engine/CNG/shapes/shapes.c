/**
 * @file shapes.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */
#include "shapes.h"

void CNG_Rect_draw(CNG_Rect *rect, CNG_Window *window) {
	SDL_SetRenderDrawColor(
		window->window_renderer,
		rect->color.r,
		rect->color.g,
		rect->color.b,
		rect->color.a
	);
	SDL_Rect sdl_rect;
	sdl_rect.x = (int) rect->position.x;
	sdl_rect.y = (int) rect->position.y;
	sdl_rect.w = (int) rect->size.x;
	sdl_rect.h = (int) rect->size.y;

	SDL_RenderFillRect(window->window_renderer, &sdl_rect);
}
