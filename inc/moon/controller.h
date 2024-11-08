#ifndef _CONTROLLER_
#define _CONTROLLER_

#include "moon/math_types.h"

struct PlayerObject;

#define MAX_CONTROLLERS 2

typedef struct ControllerObject {
    struct PlayerObject* player;
    u8                   controllerId; // 0..1
    MathVector           axis;        
} ControllerObject;

extern ControllerObject GControllers[MAX_CONTROLLERS];

// --- Internal use
void _Controller_InitializeControllers();
void _Controller_SetPlayer(struct PlayerObject *player, u16 joy);
void _Controller_SetInput(u16 joy, u16 changed, u16 state);
void _Controller_UpdateInput();
// ---

#endif