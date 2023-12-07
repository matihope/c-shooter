/**
 * @file player.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#pragma once
#include <CNG/CNG.h>

typedef struct {
	CNG_Rect rect;
} Player;

Player PlayerInit(CNG_Color color, CNG_Vector2f position);

void PlayerUpdate(Player *player, const float dt);

void PlayerDraw(Player *player, CNG_Window *window);
