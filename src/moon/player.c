#include "moon/player.h"
#include "moon/character.h"
#include "moon/controller.h"
#include "moon/collision.h"
#include "moon/game.h"
#include "moon/level.h"

#define IDLE 0
#define WALK 1

// World Limit
#define LIMIT_PLAYER_MIN_X 0
#define LIMIT_PLAYER_MAX_X 289
#define LIMIT_PLAYER_MIN_Y 0
#define LIMIT_PLAYER_MAX_Y 175

// Screen Limit to move Scroll
#define LIMIT_SCREEN_SCROLL_LEFT 35
#define LIMIT_SCREEN_SCROLL_RIGHT 250
#define LIMIT_SCREEN_SCROLL_UP 20
#define LIMIT_SCREEN_SCROLL_DOWN 160

u16 Player_Initialize(PlayerObject* player, u16 joy, VDPPlane layerId, const SpriteDefinition* spriteDef, const Palette* palette, const MathVector position, u16 paletteId, u16 priority) {
    if(!player)
        return 0;

    player->speed = 1;
    player->scrollSpeed = 1;
    player->scrollDir = NONE;
    player->limit.vmin.x = LIMIT_PLAYER_MIN_X;
    player->limit.vmin.y = LIMIT_PLAYER_MIN_Y;
    player->limit.vmax.x = LIMIT_PLAYER_MAX_X;
    player->limit.vmax.y = LIMIT_PLAYER_MAX_Y;

    player->limitScreenScrollMap.vmin.x = 0;
    player->limitScreenScrollMap.vmin.y = 0;
    player->limitScreenScrollMap.vmax.x = 0;
    player->limitScreenScrollMap.vmax.y = 0;

    player->triggerBox.vmin.x = 0;
    player->triggerBox.vmin.y = 0;
    player->triggerBox.vmax.x = spriteDef->w;
    player->triggerBox.vmax.y = spriteDef->h;

    player->collisionBox.vmin.x = 0;
    player->collisionBox.vmin.y = 0;
    player->collisionBox.vmax.x = spriteDef->w;
    player->collisionBox.vmax.y = spriteDef->h;

    _Controller_SetPlayer(player, joy);
    const u16 numTiles = Character_Initialize(&player->character, layerId, spriteDef, palette, position, paletteId, priority);
    return numTiles;
}

void Player_SetLimitWorld(PlayerObject* player, const MathBox limit) {
    if(!player)
        return;

    player->limit.vmin.x = limit.vmin.x;
    player->limit.vmin.y = limit.vmin.y;
    player->limit.vmax.x = limit.vmax.x;
    player->limit.vmax.y = limit.vmax.y;
}

void Player_SetLimitScreenScroll(PlayerObject* player, const MathBox limitScrollMap) {
    if(!player)
        return;

    player->limitScreenScrollMap = limitScrollMap;
}

void Player_SetSpeed(PlayerObject* player, u8 speed) {
    if(!player)
        return;

    player->speed = speed;
}

void Player_SetScrollSpeed(PlayerObject* player, u8 speed) {
    if(!player)
        return;

    player->scrollSpeed = speed;
}

void Player_SetTriggerBox(PlayerObject* player, const MathBox triggerBox) {
    if(!player)
        return;

    player->triggerBox = triggerBox;
}

void Player_SetCollisionBox(PlayerObject* player, const MathBox collisionBox) {
    if(!player)
        return;

    player->collisionBox = collisionBox;
}

MathBox Player_GetTriggerBoxInWorld(PlayerObject* player) {
    MathBox triggerBox;
    if(player) {
        triggerBox.vmin.x = player->character.pos.x - player->character.offset.x + player->triggerBox.vmin.x;
        triggerBox.vmin.y = player->character.pos.y - player->character.offset.y + player->triggerBox.vmin.y;
        triggerBox.vmax.x = player->character.pos.x - player->character.offset.x + player->triggerBox.vmax.x;
        triggerBox.vmax.y = player->character.pos.y - player->character.offset.y + player->triggerBox.vmax.y;
    }   
    return triggerBox;
}


void Player_SetPosition(PlayerObject* player, const MathVector position) {
    if(!player)
        return;

    Character_SetPosition(&player->character, position);
}

void _Player_UpdateInput(PlayerObject* player, const struct ControllerObject* controller) {
    const bool bIsMoving = controller->axis.x != 0 || controller->axis.y != 0;

    if(bIsMoving) {
        Character_SetAnim(&player->character, WALK);
        if(controller->axis.x > 0) {
            Character_SetHFlip(&player->character, false);
        }
        else if(controller->axis.x < 0) {
            Character_SetHFlip(&player->character, true);
        }

        MathVector newPos = player->character.pos;
        MathVector newScrollOffset;
        newScrollOffset.x = newScrollOffset.y = 0;
        // KLog_S2("Axis X: ", controller->axis.x, " Y: ", controller->axis.y);
        if(controller->axis.x != 0) {
            const s16 speed = (player->scrollDir == RIGHT || player->scrollDir == LEFT) ? player->scrollSpeed : player->speed;
            const s16 inc_x = controller->axis.x * speed;
            const s16 new_pos_x = player->character.pos.x + inc_x;
            if(new_pos_x > player->limit.vmin.x && new_pos_x < player->limit.vmax.x) {
                MathVector world;
                world.x = new_pos_x;
                world.y = player->character.pos.y;
                MathVector local = Level_WorldToLocal(player->character.layerId, world);
                if( local.x > player->limitScreenScrollMap.vmax.x && player->limitScreenScrollMap.vmin.x != player->limitScreenScrollMap.vmax.x && Level_CanGoTo(player->character.layerId, RIGHT)) {
                    newScrollOffset.x = controller->axis.x * player->scrollSpeed;
                    MathVector newPos2 = Level_LocalToWorld(player->character.layerId, player->limitScreenScrollMap.vmax);
                    newPos.x = newPos2.x;
                    player->scrollDir = RIGHT;
                }
                else if(local.x < player->limitScreenScrollMap.vmin.x && player->limitScreenScrollMap.vmin.x != player->limitScreenScrollMap.vmax.x && Level_CanGoTo(player->character.layerId, LEFT)) {
                    newScrollOffset.x = controller->axis.x * player->scrollSpeed;
                    MathVector newPos2 = Level_LocalToWorld(player->character.layerId, player->limitScreenScrollMap.vmin);
                    newPos.x = newPos2.x;
                    player->scrollDir = LEFT;
                } 
                else { // Move Player
                    newPos.x = new_pos_x;
                    player->scrollDir = player->scrollDir == RIGHT || player->scrollDir == LEFT ? NONE : player->scrollDir;
                }
            }
        }
        if(controller->axis.y != 0) {
            const s16 speed = (player->scrollDir == UP || player->scrollDir == DOWN) ? player->scrollSpeed : player->speed;
            const s16 inc_y = controller->axis.y * speed;
            const s16 new_pos_y = player->character.pos.y + inc_y;
            if(new_pos_y > player->limit.vmin.y && new_pos_y < player->limit.vmax.y) { 
                MathVector world;
                world.x = player->character.pos.x;
                world.y = new_pos_y;
                MathVector local = Level_WorldToLocal(player->character.layerId, world);
                if( local.y > player->limitScreenScrollMap.vmax.y && player->limitScreenScrollMap.vmin.y != player->limitScreenScrollMap.vmax.y && Level_CanGoTo(player->character.layerId, DOWN)) {
                    newScrollOffset.y = controller->axis.y * player->scrollSpeed;
                    MathVector newPos2 = Level_LocalToWorld(player->character.layerId, player->limitScreenScrollMap.vmax);
                    newPos.y = newPos2.y;
                    player->scrollDir = DOWN;
                }
                else if(local.y < player->limitScreenScrollMap.vmin.y && player->limitScreenScrollMap.vmin.y != player->limitScreenScrollMap.vmax.y && Level_CanGoTo(player->character.layerId, UP)) {
                    newScrollOffset.y = controller->axis.y * player->scrollSpeed;
                    MathVector newPos2 = Level_LocalToWorld(player->character.layerId, player->limitScreenScrollMap.vmin);
                    newPos.y = newPos2.y;
                    player->scrollDir = UP;
                } 
                else { // Move Player
                    newPos.y = new_pos_y;
                    player->scrollDir = player->scrollDir == DOWN || player->scrollDir == UP ? NONE : player->scrollDir;
                }
            }
        }
        // Set position player character
        MathBox collisionBox;
        collisionBox.vmin.x = player->character.pos.x - player->character.offset.x + player->collisionBox.vmin.x;
        collisionBox.vmin.y = player->character.pos.y - player->character.offset.y + player->collisionBox.vmin.y;
        collisionBox.vmax.x = player->character.pos.x - player->character.offset.x + player->collisionBox.vmax.x;
        collisionBox.vmax.y = player->character.pos.y - player->character.offset.y + player->collisionBox.vmax.y;
        MathVector dir;
        dir.x = newPos.x - player->character.pos.x;
        dir.y = newPos.y - player->character.pos.y;
        Collision_Check(newPos, collisionBox, dir, &newPos);
        Character_SetPosition(&player->character, newPos);

        // Scroll map
        if(newScrollOffset.x != 0 || newScrollOffset.y != 0) {
            MathVector scrollPos = _Level_GetOffsetMap(player->character.layerId);
            scrollPos.x += newScrollOffset.x;
            scrollPos.y += newScrollOffset.y;
            _Level_SetOffsetMap(player->character.layerId, scrollPos);
        }
    } else {
        Character_SetAnim(&player->character, IDLE);
    }
}

