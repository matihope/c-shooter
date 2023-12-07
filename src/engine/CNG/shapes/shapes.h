/**
 * @file shapes.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#pragma once
#include <CNG/math/vector.h>
#include <CNG/utils/utils.h>
#include <CNG/window/window.h>

typedef struct {
	CNG_Vector2f position;
	CNG_Vector2f size;
	CNG_Color    color;
} CNG_Rect;

void CNG_RectDraw(CNG_Rect *rect, CNG_Window *window);
