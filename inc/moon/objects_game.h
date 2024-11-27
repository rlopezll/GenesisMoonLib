#ifndef _OBJECT_GAME_
#define _OBJECT_GAME_

#include <genesis.h>
#include "moon/math_types.h"

typedef struct {
    u8      typeID;
    u16     uniqueID;
    MathBox box;
} TMX_Object; 

typedef void (*CallbackTriggerFunc)(u8 controllerIdx, TMX_Object* objectTrigger);

void Objects_RegisterGameObjects(TMX_Object* GTMX_Objects, u16 numObjects);
void Objects_UnRegisterGameObjects();
TMX_Object* Objects_GetObject(u8 typeID, u16 uniqueID);

void Objects_RegisterTrigger(TMX_Object* objectTrigger);
void Objects_UnRegisterTriggers();
void Objects_RegisterTriggerCallback(CallbackTriggerFunc enterCallback, CallbackTriggerFunc exitCallback);
void Objects_UnRegisterTriggerCallback();

// --- Internal use
void _Objects_Initialize();
void _Objects_Update();
// --- 

#endif