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
        GameCollisionObject *currCollision = &GCollisions.ObjectsCollision[idx];
        bool bIsInside = false;
        if(currCollision->object) {
            bIsInside = ((currCollision->object->box.vmin.x < levelBox.vmin.x || currCollision->object->box.vmin.x < levelBox.vmax.x) && (currCollision->object->box.vmax.x > levelBox.vmin.x || currCollision->object->box.vmax.x > levelBox.vmax.x)) || 
                                ((levelBox.vmin.x < currCollision->object->box.vmin.x || levelBox.vmax.x < currCollision->object->box.vmin.x) && (levelBox.vmin.x > currCollision->object->box.vmax.x || levelBox.vmax.x > currCollision->object->box.vmax.x));
            bIsInside = bIsInside && (((currCollision->object->box.vmin.y < levelBox.vmin.y || currCollision->object->box.vmin.y < levelBox.vmax.y) && (currCollision->object->box.vmax.y > levelBox.vmin.y || currCollision->object->box.vmax.y > levelBox.vmax.y)) || 
                                    ((levelBox.vmin.y < currCollision->object->box.vmin.y || levelBox.vmax.y < currCollision->object->box.vmin.y) && (levelBox.vmin.y > currCollision->object->box.vmax.y || levelBox.vmax.y > currCollision->object->box.vmax.y)));
        }
        currCollision->bActive = bIsInside;
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
        GameCollisionObject *currCollision = &GCollisions.ObjectsCollision[idx];
        if(currCollision->bActive && currCollision->object) {
            const bool bIsInsideX = ((currCollision->object->box.vmin.x < box.vmin.x || currCollision->object->box.vmin.x < box.vmax.x) && (currCollision->object->box.vmax.x > box.vmin.x || currCollision->object->box.vmax.x > box.vmax.x)) || 
                             ((box.vmin.x < currCollision->object->box.vmin.x || box.vmax.x < currCollision->object->box.vmin.x) && (box.vmin.x > currCollision->object->box.vmax.x || box.vmax.x > currCollision->object->box.vmax.x));
            const bool bIsInsideY = (((currCollision->object->box.vmin.y < box.vmin.y || currCollision->object->box.vmin.y < box.vmax.y) && (currCollision->object->box.vmax.y > box.vmin.y || currCollision->object->box.vmax.y > box.vmax.y)) || 
                                      ((box.vmin.y < currCollision->object->box.vmin.y || box.vmax.y < currCollision->object->box.vmin.y) && (box.vmin.y > currCollision->object->box.vmax.y || box.vmax.y > currCollision->object->box.vmax.y)));
            if(bIsInsideX && bIsInsideY) {
                // KLog_U1("Is Inside idx: ", idx);
                // KLog_S2("Collision X: ", pos.x, " Y: ", pos.y);
                // KLog_S4("Collision Player Box MinX: ", box.vmin.x, " MinY: ", box.vmin.y, " MaxX: ", box.vmax.x, " MaxY: ", box.vmax.y);
                // KLog_S4("Collision Object Box X: ", currCollision->object->box.vmin.x, " Y: ", currCollision->object->box.vmin.y, " W: ", currCollision->object->box.vmax.x, " H: ", currCollision->object->box.vmax.y);
                if(bIsInsideX && dir.x > 0 && collisionPos) {
                    // KLog("Collision 1");
                    collisionPos->x = pos.x - (box.vmax.x - currCollision->object->box.vmin.x);
                } else if(bIsInsideX && dir.x < 0 && collisionPos) {
                    // KLog("Collision 2");
                    collisionPos->x = pos.x + (currCollision->object->box.vmax.x - box.vmin.x);
                }
                if(bIsInsideY && dir.y > 0 && collisionPos) {
                    // KLog("Collision 3");
                    collisionPos->y = pos.y - (box.vmax.y - currCollision->object->box.vmin.y);
                } else if(bIsInsideY && dir.y < 0 && collisionPos) {
                    // KLog("Collision 4");
                    collisionPos->y = pos.y + (currCollision->object->box.vmax.y - box.vmin.y);
                }
                // KLog_S2("New Position X: ", collisionPos->x, " Y: ", collisionPos->y);
                return true;
            }
        }
    }
    collisionPos->x = pos.x;
    collisionPos->y = pos.y;
    return false;
}
