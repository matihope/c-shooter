/**
 * @file player.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#pragma once
#include <CNG/CNG.h>

typedef struct {
	CNG_Rect rect;
} Player;

Player Player_init(CNG_Color color, CNG_Vector2f position);

void Player_update(Player *player, float dt);

void Player_draw(Player *player, CNG_Window *window);
