#include "moon/character.h"
#include "moon/level.h"
#include <genesis.h>

CharacterObject* GCharacters[MAX_CHARACTERS_REGISTRED];

void Character_Initialize(CharacterObject* character, VDPPlane layerId, const SpriteDefinition* spriteDef, const MathVector position, u16 tileIndex, u16 palette, u16 priority) {
    s16 x = fastFix32ToInt(position.x);
    s16 y = fastFix32ToInt(position.y);
    character->sprite = SPR_addSprite(spriteDef, x, y, TILE_ATTR(palette,priority,FALSE,FALSE));
    SPR_setVRAMTileIndex(character->sprite, tileIndex);
    PAL_setPalette(palette, spriteDef->palette->data, DMA);
    character->size.x = spriteDef->w;
    character->size.y = spriteDef->h;
    character->hFlip = false;
    character->vFlip = false;
    character->currAnimIdx = -1;
    character->layerId = layerId;
    character->bIsVisible = true;
    character->offset.x = character->offset.y = 0;
    Character_SetPosition(character, position);
    Character_RegisterCharacter(character);
}

void Character_RegisterCharacter(CharacterObject* character) {
    for(u16 idx=0;idx<MAX_CHARACTERS_REGISTRED;++idx) {
        if(!GCharacters[idx]) {
            GCharacters[idx] = character;
            break;
        }
    }
}

void Character_UnregisterCharacter(CharacterObject* character) {
    if(!character)
        return;

    SPR_setVisibility(character->sprite, HIDDEN);

    u16 lastValidIdx = 0;
    s16 characterIdx = -1;
    for(u16 idx=0;idx<MAX_CHARACTERS_REGISTRED;++idx) {
        if(!GCharacters[idx]) { // register character list finished!
            if(characterIdx >=0 ) { // character found in the list
                if(idx-1 > characterIdx) {
                    GCharacters[characterIdx] = GCharacters[idx-1];
                    GCharacters[idx-1] = NULL;
                }
                else { //is the last character
                    GCharacters[characterIdx] = NULL;
                }
            }
            break;
        }
        else if(GCharacters[idx] == character) {
            characterIdx = idx;
        }
    }
}

void Character_UnregisterAllCharacters() {
    for(u16 idx=0;idx<MAX_CHARACTERS_REGISTRED;++idx) {
        GCharacters[idx] = NULL;
    }
}

void Character_SetOffset(CharacterObject* character, const MathVector offset) {
    character->offset = offset;
}

void Character_SetPosition(CharacterObject* character, MathVector position) {
    character->pos.x = position.x;
    character->pos.y = position.y;
    // left/up position (no offset applied)
    position.x -= character->offset.x;
    position.y -= character->offset.y;
    character->localPos = Level_WorldToLocal(character->layerId, position);
    const bool bIsVisible = Level_IsVisible(character->layerId, position, character->size);
    if(bIsVisible != character->bIsVisible) {
        if(bIsVisible)
            KLog("Character is Invisible");
        else
            KLog("Character is Visible");
        character->bIsVisible = bIsVisible;
        SPR_setVisibility(character->sprite, bIsVisible ? VISIBLE : HIDDEN);
    }
}

bool Character_IsVisible(CharacterObject* character) {
    return character->bIsVisible;
}

void Character_SetAnim(CharacterObject* character, u8 animIdx) {
    if(character->currAnimIdx == animIdx)
        return;

    SPR_setAnim(character->sprite, animIdx);
    character->currAnimIdx = animIdx;
}

void Character_SetVFlip(CharacterObject* character, bool bFlip) {
    if(character->vFlip == bFlip) 
        return;
    character->vFlip = bFlip;
    SPR_setVFlip(character->sprite, bFlip);
}

void Character_SetHFlip(CharacterObject* character, bool bFlip) {
    if(character->hFlip == bFlip) 
        return;
    character->hFlip = bFlip;
    SPR_setHFlip(character->sprite, bFlip);
}

void _Character_InitializeRegisterCharacters() {
    Character_UnregisterAllCharacters();
}

void _Character_Update() {
    for(u16 idx=0;idx<MAX_CHARACTERS_REGISTRED;++idx) {
        if(!GCharacters[idx])
            break;

        if(GCharacters[idx]->bIsVisible) {
            KLog_U2("localPos x:", GCharacters[idx]->localPos.x, " y: ", GCharacters[idx]->localPos.y);
            SPR_setPosition(GCharacters[idx]->sprite, GCharacters[idx]->localPos.x, GCharacters[idx]->localPos.y);
        } 
    }
}