/**
 * @file shapes.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#ifndef SHAPES_H
#define SHAPES_H

#include "CNG/common/math.h"
#include "base.h"
#include "window.h"

typedef struct {
	CNG_Vector2f position;
	CNG_Vector2f size;
	CNG_Color    color;
} CNG_Rect;

void CNG_Rect_draw(CNG_Rect *rect, CNG_Window *window);

#endif
