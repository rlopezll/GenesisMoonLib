#include "moon/controller.h"
#include "moon/player.h"
#include <joy.h>

ControllerObject GControllers[MAX_CONTROLLERS];

void _Controller_InitializeControllers() {
    for(u8 idx=0;idx<MAX_CONTROLLERS;++idx) {
        GControllers[idx].controllerId = idx;
        GControllers[idx].player = NULL;
    }
}

void _Controller_SetPlayer(struct PlayerObject *player, u16 joy) {
    if(joy < MAX_CONTROLLERS) {
        GControllers[joy].player = player;
    }
}

void _Controller_SetInput(u16 joy, u16 changed, u16 state) {
    if(joy < MAX_CONTROLLERS) {
        if(!GControllers[joy].player) 
            return;

        ControllerObject *controller = &GControllers[joy];
        bool bIsMoving = false;
        if(state &  BUTTON_DOWN){
            controller->axis.y = 1;
            bIsMoving = true;
        }
        else if(state &  BUTTON_UP){            
            controller->axis.y = -1;
            bIsMoving = true;
        } else {
            controller->axis.y = 0;
        }

        if(state &  BUTTON_LEFT){
            controller->axis.x = -1;
            bIsMoving = true;
        }
        else if(state &  BUTTON_RIGHT){
            controller->axis.x = 1;
            bIsMoving = true;
        } else {
            controller->axis.x = 0;
        }

        if(!bIsMoving) {
            controller->axis.x = controller->axis.y = 0;
        }
    }
}

void _Controller_UpdateInput() {
    for(u8 idx=0;idx<MAX_CONTROLLERS;++idx) {
        if(GControllers[idx].player) {
            _Player_UpdateInput(GControllers[idx].player, &GControllers[idx]);
        }
    }
}
