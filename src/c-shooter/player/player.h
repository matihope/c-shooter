/**
 * @file player.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <CNG/graphics.h>

typedef struct {
	uint16_t     id;
	CNG_Color    color;
	CNG_Vector2f position;
	uint32_t     tick_origin;
} PlayerFeatures;

typedef struct {
	uint16_t     id;
	CNG_Vector2f position;
} PlayerNewPosition;

typedef struct {
	CNG_Rect rect;
} Player;

Player Player_init(CNG_Color color, CNG_Vector2f position);

void Player_update(Player *player, float dt);

void Player_draw(Player *player, CNG_Window *window);

#endif  // PLAYER_H
