#ifndef _MOON_COLLISION_
#define _MOON_COLLISION_

#include <genesis.h>
#include "moon/math_types.h"
#include "moon/objects_game.h"

void Collision_SetScrollMap(s8 layerId);
void Collision_RegisterCollision(TMX_Object* objectTrigger);
void Collision_UnRegisterCollisions();

// pos and box are in world coords.
bool Collision_Check(const MathVector pos, const MathBox box, const MathVector dir, MathVector *collisionPos);

// --- Internal use
void _Collision_Initialize();
void _Collision_Update();
// --- 

#endif