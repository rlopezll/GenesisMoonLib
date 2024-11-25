#ifndef _CHARACTER_
#define _CHARACTER_

#include <genesis.h>
#include "moon/math_types.h"

struct Sprite;
struct SpriteDefinition;
struct ControllerObject;

#define MAX_CHARACTERS_REGISTRED 128

typedef struct CharacterObject {
    struct Sprite* sprite;
    VDPPlane       layerId;
    MathVector     pos;
    MathVector     offset; // position offset in sprite
    MathVector     localPos; // left/up position (no offset applied)
    MathVector     size;
    s8             currAnimIdx;
    u8             lastUpdateFrameID;
    bool           hFlip;
    bool           vFlip;
    bool           bIsVisible;
} CharacterObject;

extern CharacterObject* GCharacters[MAX_CHARACTERS_REGISTRED];

u16  Character_Initialize(CharacterObject* character, VDPPlane layerId, const SpriteDefinition* spriteDef, const Palette* palette, const MathVector position, u16 paletteId, u16 priority);
void Character_RegisterCharacter(CharacterObject* character);
void Character_UnregisterCharacter(CharacterObject* character);
void Character_UnregisterAllCharacters();

void Character_SetOffset(CharacterObject* character, const MathVector offset);
void Character_SetPosition(CharacterObject* character, MathVector position);

void Character_SetAnim(CharacterObject* character, u8 animIdx);
void Character_SetVFlip(CharacterObject* character, bool bFlip);
void Character_SetHFlip(CharacterObject* character, bool bFlip);

bool Character_IsVisible(CharacterObject* character);

// --- Internal use
void _Character_InitializeRegisterCharacters();
void _Character_UpdateAllCharacters(); // Update All Characters
void _Character_UpdateLocalPositionAndVisibility(CharacterObject* character);
// --- 

#endif