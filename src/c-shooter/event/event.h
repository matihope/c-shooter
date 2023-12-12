/**
 * @file event.h
 * @author Mateusz Kołpa (matihopemine@gmail.com)
 */

#ifndef SHOOTER_EVENT_H
#define SHOOTER_EVENT_H
#include "../player/player.h"
#include "CNG/common/math.h"

#include <unitypes.h>

enum CNG_EventType {
	CNG_EventType_Init,
	CNG_EventType_InitFeatures,
	CNG_EventType_PlayerMove
};

typedef struct {
	enum CNG_EventType type;  // CNG_EventType_Init
	uint16_t           new_client_id;
} CNG_Event_Init;

typedef struct {
	enum CNG_EventType type;  // CNG_EventType_PlayerMove
	uint16_t           player_id;
	CNG_Vector2f       new_pos;
	uint32_t           tick_origin;
} CNG_Event_PlayerMove;

typedef struct {
	enum CNG_EventType type;  // CNG_EventType_InitFeatures
	PlayerFeatures     features;
} CNG_Event_InitFeatures;

typedef union {
	enum CNG_EventType     type;
	CNG_Event_Init         init;
	CNG_Event_PlayerMove   move;
	CNG_Event_InitFeatures features;
} CNG_Event;


#endif
