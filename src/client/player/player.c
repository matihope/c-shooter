/**
 * @file player.c
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#include "player.h"

Player Player_init(CNG_Color color, CNG_Vector2f position) {
	return (Player
	){ .rect = (CNG_Rect){ .color    = color,
		                   .position = position,
		                   .size     = (CNG_Vector2f){ .x = 50, .y = 50 } } };
}

void Player_update(Player *player, const float dt) {
	const Uint8 *current_key_states = SDL_GetKeyboardState(NULL);

	float d_x = (float) current_key_states[SDL_SCANCODE_D]
	          - (float) current_key_states[SDL_SCANCODE_A];
	float d_y = (float) current_key_states[SDL_SCANCODE_S]
	          - (float) current_key_states[SDL_SCANCODE_W];
	player->rect.position.x += 500 * d_x * dt;
	player->rect.position.y += 500 * d_y * dt;
}

void Player_draw(Player *player, CNG_Window *window) {
	CNG_Rect_draw(&player->rect, window);
}
