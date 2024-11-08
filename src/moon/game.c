#include "moon/game.h"
#include "moon/gamestate.h"
#include "moon/controller.h"
#include "moon/level.h"

void _inputHandler(u16 joy, u16 changed, u16 state) {
    _Controller_SetInput(joy, changed, state);
}

void Game_Initialize() {
    _GameState_Initialize();
    _Controller_InitializeControllers();
    _Character_InitializeRegisterCharacters();
    _Objects_Initialize();

    MathVector resolution;
    resolution.x = 320; resolution.y = 240;
    _Level_Initialize(resolution);

    VDP_setScreenWidth320(); 

    JOY_init();
    JOY_setEventHandler(_inputHandler);
}

void Game_Update() {
    _Controller_UpdateInput();
    
    if(GCurrGameState && GCurrGameState->updateFuncPtr) {
        GCurrGameState->updateFuncPtr();
    }

    _Level_Update();
    _Character_Update();
    _Objects_Update();
    SPR_update();

    //For versions prior to SGDK 1.60 use VDP_waitVSync instead.
    SYS_doVBlankProcess();
}

void Game_Finalize() {
    SPR_end();
}
