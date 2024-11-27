#include "moon/objects_game.h"
#include "moon/controller.h"
#include "moon/player.h"


// Game objects global vars
TMX_Object* GAllObjectsGame = 0;
u16 GNumObjectsGame = 0;

// Triggers global vars
#define MAX_OBJECTS_TRIGGER 255

typedef struct {
    TMX_Object* object;
    bool        playerInside[MAX_CONTROLLERS];    
} GameTriggerObject;

GameTriggerObject GObjectsTrigger[MAX_OBJECTS_TRIGGER];
u16 GNumObjectsTrigger = 0;
CallbackTriggerFunc GCallbackEnterTrigger = NULL;
CallbackTriggerFunc GCallbackExitTrigger = NULL;

void Objects_RegisterGameObjects(TMX_Object* GTMX_Objects, u16 numObjects) {
    GAllObjectsGame = GTMX_Objects;
    GNumObjectsGame = numObjects;
}

void Objects_UnRegisterGameObjects() {
    GAllObjectsGame = NULL;
    GNumObjectsGame = 0;
}

TMX_Object* Objects_GetObject(u8 typeID, u16 uniqueID) {
    for(u16 idx=0;idx<GNumObjectsGame;++idx) {
        if(GAllObjectsGame[idx].typeID == typeID && GAllObjectsGame[idx].uniqueID == uniqueID)
            return &GAllObjectsGame[idx];
    }
    return NULL;
}

void Objects_RegisterTrigger(TMX_Object* objectTrigger) {
    if(GNumObjectsTrigger >= MAX_OBJECTS_TRIGGER)
        return;

    GObjectsTrigger[GNumObjectsTrigger].object = objectTrigger;
    for(u8 idx=0;idx<MAX_CONTROLLERS;++idx) {
        GObjectsTrigger[GNumObjectsTrigger].playerInside[idx] = false;
    }
    ++GNumObjectsTrigger;
}

void Objects_UnRegisterTriggers() {
    for(u16 idx=0;idx<MAX_OBJECTS_TRIGGER;++idx) {
        GObjectsTrigger[idx].object = NULL;
        for(u8 idx2=0;idx2<MAX_CONTROLLERS;++idx2) {
            GObjectsTrigger[idx].playerInside[idx2] = false;
        }
    }
    GNumObjectsTrigger = 0;
}

void Objects_RegisterTriggerCallback(CallbackTriggerFunc enterCallback, CallbackTriggerFunc exitCallback) {
    GCallbackEnterTrigger = enterCallback;
    GCallbackExitTrigger = exitCallback;
}

void Objects_UnRegisterTriggerCallback() {
    GCallbackEnterTrigger = NULL;
    GCallbackExitTrigger = NULL;
}

void _Objects_Initialize() {
    Objects_UnRegisterTriggers();
    Objects_UnRegisterTriggerCallback();
}

void _Objects_Update() {

    // Triggers
    if(GNumObjectsTrigger != 0 && (GCallbackEnterTrigger || GCallbackExitTrigger))
    {
        for(u8 idx=0;idx<MAX_CONTROLLERS;++idx) {
        if(GControllers[idx].player) {
            PlayerObject* player = GControllers[idx].player;
            const MathBox playerTriggerBox = Player_GetTriggerBoxInWorld(player);
            for(u16 idx2=0;idx2<GNumObjectsTrigger;++idx2) {
                if(GObjectsTrigger[idx2].object) {
                    const MathBox triggerBox = GObjectsTrigger[idx2].object->box;
                    bool bIsInside = ((triggerBox.vmin.x < playerTriggerBox.vmin.x || triggerBox.vmin.x < playerTriggerBox.vmax.x) && (triggerBox.vmax.x > playerTriggerBox.vmin.x || triggerBox.vmax.x > playerTriggerBox.vmax.x)) || 
                       ((playerTriggerBox.vmin.x < triggerBox.vmin.x || playerTriggerBox.vmax.x < triggerBox.vmin.x) && (playerTriggerBox.vmin.x > triggerBox.vmax.x || playerTriggerBox.vmax.x > triggerBox.vmax.x));
                    bIsInside = bIsInside && (((triggerBox.vmin.y < playerTriggerBox.vmin.y || triggerBox.vmin.y < playerTriggerBox.vmax.y) && (triggerBox.vmax.y > playerTriggerBox.vmin.y || triggerBox.vmax.y > playerTriggerBox.vmax.y)) || 
                       ((playerTriggerBox.vmin.y < triggerBox.vmin.y || playerTriggerBox.vmax.y < triggerBox.vmin.y) && (playerTriggerBox.vmin.y > triggerBox.vmax.y || playerTriggerBox.vmax.y > triggerBox.vmax.y)));
                    
                    const bool playerWasInside = GObjectsTrigger[idx2].playerInside[idx];
                    if(bIsInside && !playerWasInside && GCallbackEnterTrigger) {
                        GObjectsTrigger[idx2].playerInside[idx] = true;
                        GCallbackEnterTrigger(idx, GObjectsTrigger[idx2].object);
                    }
                    else if(!bIsInside && playerWasInside && GCallbackExitTrigger) {
                        GObjectsTrigger[idx2].playerInside[idx] = false;
                        GCallbackExitTrigger(idx, GObjectsTrigger[idx2].object);
                    }
                }
            }
        }
        }
    }
}
