#include "moon/collision.h"
#include "moon/controller.h"
#include "moon/level.h"
#include "moon/player.h"

#define MAX_OBJECTS_COLLISION  255

typedef struct {
    TMX_Object* object;
    bool        bActive;
} GameCollisionObject;

typedef struct {
    GameCollisionObject ObjectsCollision[MAX_OBJECTS_COLLISION];
    u16 numObjectsCollision;
    s8 layerId;
    MathVector lastLevelOffset;
} CollisionsConfigObject;

CollisionsConfigObject GCollisions;

void _Collision_Initialize() {
    GCollisions.numObjectsCollision = 0;
    GCollisions.layerId = -1;
}

void _Collision_Update() {
    if(GCollisions.layerId < 0)
        return;

    const MathVector levelOffset = _Level_GetOffsetMap(GCollisions.layerId);
    if(levelOffset.x != GCollisions.lastLevelOffset.x || levelOffset.y != GCollisions.lastLevelOffset.y)
        return;
    GCollisions.lastLevelOffset = levelOffset;
    MathBox levelBox;
    levelBox.vmin.x = levelOffset.x;
    levelBox.vmin.y = levelOffset.y;
    levelBox.vmax.x = levelOffset.x + GLevel.resolution.x;
    levelBox.vmax.y = levelOffset.y + GLevel.resolution.y;

    for(u16 idx=0;idx<GCollisions.numObjectsCollision;++idx) {
        GameCollisionObject *object = &GCollisions.ObjectsCollision[idx];
        bool bIsInside = false;
        if(object->object) {
            bIsInside = ((object->object->box.x < levelBox.vmin.x || object->object->box.x < levelBox.vmax.x) && (object->object->box.w > levelBox.vmin.x || object->object->box.w > levelBox.vmax.x)) || 
                                ((levelBox.vmin.x < object->object->box.x || levelBox.vmax.x < object->object->box.x) && (levelBox.vmin.x > object->object->box.w || levelBox.vmax.x > object->object->box.w));
            bIsInside = bIsInside && (((object->object->box.y < levelBox.vmin.y || object->object->box.y < levelBox.vmax.y) && (object->object->box.h > levelBox.vmin.y || object->object->box.h > levelBox.vmax.y)) || 
                                    ((levelBox.vmin.y < object->object->box.y || levelBox.vmax.y < object->object->box.y) && (levelBox.vmin.y > object->object->box.h || levelBox.vmax.y > object->object->box.h)));
        }
        object->bActive = bIsInside;
    }
}

void Collision_SetScrollMap(s8 layerId) {
    GCollisions.layerId = layerId;
    for(u16 idx=0;idx<GCollisions.numObjectsCollision;++idx) {
        GCollisions.ObjectsCollision[idx].bActive = layerId >= 0 ? false : true;
    }
}

void Collision_RegisterCollision(TMX_Object* objectTrigger) {
    if(GCollisions.numObjectsCollision >= MAX_OBJECTS_COLLISION)
        return;

    GCollisions.ObjectsCollision[GCollisions.numObjectsCollision].object = objectTrigger;
    if(GCollisions.layerId >= 0) {
        GCollisions.ObjectsCollision[GCollisions.numObjectsCollision].bActive = false;
    }
    else {
        GCollisions.ObjectsCollision[GCollisions.numObjectsCollision].bActive = true;
    }
    ++GCollisions.numObjectsCollision;
}

void Collision_UnRegisterCollisions() {
    for(u16 idx=0;idx<MAX_OBJECTS_COLLISION;++idx) {
        GCollisions.ObjectsCollision[idx].object = NULL;
    }
    GCollisions.numObjectsCollision = 0;
}

bool Collision_Check(const MathVector pos, const MathBox box, const MathVector dir, MathVector *collisionPos) {
    for(u16 idx=0;idx<GCollisions.numObjectsCollision;++idx) {
        GameCollisionObject *object = &GCollisions.ObjectsCollision[idx];
        if(object->bActive && object->object) {
            bool bIsInside = ((object->object->box.x < box.vmin.x || object->object->box.x < box.vmax.x) && (object->object->box.w > box.vmin.x || object->object->box.w > box.vmax.x)) || 
                             ((box.vmin.x < object->object->box.x || box.vmax.x < object->object->box.x) && (box.vmin.x > object->object->box.w || box.vmax.x > object->object->box.w));
            bIsInside = bIsInside && (((object->object->box.y < box.vmin.y || object->object->box.y < box.vmax.y) && (object->object->box.h > box.vmin.y || object->object->box.h > box.vmax.y)) || 
                                      ((box.vmin.y < object->object->box.y || box.vmax.y < object->object->box.y) && (box.vmin.y > object->object->box.h || box.vmax.y > object->object->box.h)));
            if(bIsInside) {
                if(dir.x > 0 && collisionPos) {
                    collisionPos->x = object->object->box.x - box.vmax.x - pos.x;
                } else if(dir.x < 0 && collisionPos) {
                    collisionPos->x = object->object->box.x + object->object->box.w;
                }
                if(dir.y > 0 && collisionPos) {
                    collisionPos->y = object->object->box.y - box.vmax.y - pos.y;
                } else if(dir.y < 0 && collisionPos) {
                    collisionPos->y = object->object->box.y + object->object->box.h;
                }
                return true;
            }
        }
    }
    collisionPos->x = pos.x;
    collisionPos->y = pos.y;
    return false;
}
