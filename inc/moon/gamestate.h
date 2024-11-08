#ifndef _GAMESTATE_
#define _GAMESTATE_

#include <genesis.h>

#define MAX_GAMESTATES 32

typedef  void (*CallbackFunc)();

typedef struct GameStateObject {
    u8           uniqueId;
    CallbackFunc activateFuncPtr;
    CallbackFunc deactivateFuncPtr;
    CallbackFunc updateFuncPtr;
} GameStateObject;

extern GameStateObject* GGameStates[MAX_GAMESTATES];
extern GameStateObject* GCurrGameState;

// --- Internal use
void _GameState_Initialize();
// --- 

void GameState_CreateGameState(GameStateObject *gamestate, u8 uniqueId, CallbackFunc activateFunc, CallbackFunc deactivateFunc, CallbackFunc updateFunc);
void GameState_Activate(GameStateObject *gamestate);
void GameState_ActivateById(u8 uniqueId);

#endif