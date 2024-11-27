#include "moon/character.h"
#include "moon/level.h"
#include "moon/game.h"
#include <genesis.h>

CharacterObject* GCharacters[MAX_CHARACTERS_REGISTRED];

u16 Character_Initialize(CharacterObject* character, VDPPlane layerId, const SpriteDefinition* spriteDef, const Palette* palette, const MathVector position, u16 paletteId, s16 priority) {
    u16 attr = TILE_ATTR(paletteId,priority,FALSE,FALSE);
    KLog_U1("Sprite Attr: ", attr);
    character->sprite = SPR_addSprite(spriteDef, position.x, position.y, TILE_ATTR(paletteId,priority,FALSE,FALSE));
    SPR_setDepth(character->sprite, priority);
    // SPR_setVRAMTileIndex(character->sprite, tileIndex);
    if(palette) {
        PAL_setPalette(paletteId, palette->data, DMA);
    }
    character->size.x = spriteDef->w;
    character->size.y = spriteDef->h;
    character->hFlip = false;
    character->vFlip = false;
    character->currAnimIdx = -1;
    character->layerId = layerId;
    character->bIsVisible = true;
    character->offset.x = character->offset.y = 0;
    character->lastUpdateFrameID = GGameCurrFrameId;
    Character_SetPosition(character, position);
    Character_RegisterCharacter(character);

    return spriteDef->maxNumTile;
}

void Character_RegisterCharacter(CharacterObject* character) {
    for(u16 idx=0;idx<MAX_CHARACTERS_REGISTRED;++idx) {
        if(!GCharacters[idx]) {
            KLog_U1("Character Register idx: ", idx);
            GCharacters[idx] = character;
            break;
        }
    }
}

void Character_UnregisterCharacter(CharacterObject* character) {
    if(!character)
        return;

    SPR_setVisibility(character->sprite, HIDDEN);

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
    _Character_UpdateLocalPositionAndVisibility(character); // force update
}

void Character_SetPriority(CharacterObject* character, s16 priority) {
    SPR_setDepth(character->sprite, priority);
}

void Character_SetPosition(CharacterObject* character, MathVector position) {
    if(position.x != character->pos.x || position.y != character->pos.y) {
        // KLog_S3("Character X: ", position.x, " Y: ", position.y, " Frame: ", GGameCurrFrameId);
        character->pos.x = position.x;
        character->pos.y = position.y;
        _Character_UpdateLocalPositionAndVisibility(character);
        // KLog_S2("Character Local X: ", character->localPos.x, " Y: ", character->localPos.y);
    }
    else {
        character->lastUpdateFrameID = GGameCurrFrameId;
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

void _Character_UpdateAllCharacters() {
    for(u16 idx=0;idx<MAX_CHARACTERS_REGISTRED;++idx) {
        if(!GCharacters[idx])
            break;

        CharacterObject *character = GCharacters[idx];
        if(character->lastUpdateFrameID != GGameCurrFrameId && _Level_IsOffsetChanged(character->layerId)) {
            if(idx == 0)
                KLog_U3("Update character lastUpdateFrameID: ", character->lastUpdateFrameID, ", CurrFrameID: ", GGameCurrFrameId, ", Changed Offset: ", (int)_Level_IsOffsetChanged(character->layerId));
            _Character_UpdateLocalPositionAndVisibility(character);
        }

        if(character->bIsVisible) {
            // KLog_U1("Character idx: ", idx);
            if(idx >0) 
                KLog_S2("Character Local X: ", character->localPos.x, " Y: ", character->localPos.y);
            SPR_setPosition(character->sprite, character->localPos.x, character->localPos.y);
        } 
    }
}

void _Character_UpdateLocalPositionAndVisibility(CharacterObject* character) {
    if(!character)
        return;

    MathVector position = character->pos;
    // left/up position (no offset applied)
    position.x -= character->offset.x;
    position.y -= character->offset.y;
    
    character->localPos = Level_WorldToLocal(character->layerId, position);
    const bool bIsVisible = Level_IsVisible(character->layerId, position, character->size);
    if(bIsVisible != character->bIsVisible) {
        if(bIsVisible)
            KLog("Character is Visible");
        else
            KLog("Character is Invisible");
        character->bIsVisible = bIsVisible;
        SPR_setVisibility(character->sprite, bIsVisible ? VISIBLE : HIDDEN);
    }
    character->lastUpdateFrameID = GGameCurrFrameId;
}
