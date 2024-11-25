#include "moon/game.h"
#include "moon/gamestate.h"
#include "moon/controller.h"
#include "moon/collision.h"
#include "moon/level.h"

u8 GGameCurrFrameId = 0;

void _inputHandler(u16 joy, u16 changed, u16 state) {
    _Controller_SetInput(joy, changed, state);
}

void Game_Initialize() {
    GameConfigObject gameConfig;
    gameConfig.spriteManagerNumTiles = 420;
    Game_InitializeEx(&gameConfig);
}

void Game_InitializeEx(const GameConfigObject* gameConfig)
{
    _GameState_Initialize();
    _Controller_InitializeControllers();
    _Character_InitializeRegisterCharacters();
    _Objects_Initialize();
    _Collision_Initialize();

    MathVector resolution;
    resolution.x = 320; resolution.y = 240;
    _Level_Initialize(resolution);

    VDP_setScreenWidth320(); 

    SPR_initEx(gameConfig->spriteManagerNumTiles);

    JOY_init();
    JOY_setEventHandler(_inputHandler);
}

void Game_Update() {
    _Controller_UpdateInput();
    
    if(GCurrGameState && GCurrGameState->updateFuncPtr) {
        GCurrGameState->updateFuncPtr();
    }

    _Character_UpdateAllCharacters();
    _Objects_Update();
    _Collision_Update();
    _Level_Update();
    SPR_update();

    _Level_PostUpdate();
    //For versions prior to SGDK 1.60 use VDP_waitVSync instead.
    SYS_doVBlankProcess();
    ++GGameCurrFrameId;
}

void Game_Finalize() {
    SPR_end();
}
