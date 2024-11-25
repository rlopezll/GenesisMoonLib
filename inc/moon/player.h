#ifndef _PLAYER_GAME_
#define _PLAYER_GAME_

#include <genesis.h>
#include "moon/math_types.h"
#include "moon/character.h"

struct ControllerObject;

typedef struct PlayerObject {
    CharacterObject character;
    MathBox         limit; // World limit
    MathBox         limitScreenScrollMap;
    MathBox         triggerBox; 
    MathBox         collisionBox; 
    u8              speed;
    u8              scrollSpeed;
    Direction       scrollDir;
} PlayerObject;

u16  Player_Initialize(PlayerObject* player, u16 joy, VDPPlane layerId, const SpriteDefinition* spriteDef, const Palette* palette, const MathVector position, u16 paletteId, u16 priority);
void Player_SetLimitWorld(PlayerObject* player, const MathBox limit);
void Player_SetLimitScreenScroll(PlayerObject* player, const MathBox limitScrollMap);
void Player_SetSpeed(PlayerObject* player, u8 speed);
void Player_SetScrollSpeed(PlayerObject* player, u8 speed);

void Player_SetTriggerBox(PlayerObject* player, const MathBox triggerBox);
void Player_SetCollisionBox(PlayerObject* player, const MathBox collisionBox);
MathBox Player_GetTriggerBoxInWorld(PlayerObject* player);


void Player_SetPosition(PlayerObject* player, const MathVector position);

// --- Internal use
void _Player_UpdateInput(PlayerObject* player, const struct ControllerObject* controller);
// --- 

#endif