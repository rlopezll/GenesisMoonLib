#include "moon/gamestate.h"

GameStateObject* GGameStates[MAX_GAMESTATES];
GameStateObject* GCurrGameState = NULL;

void _GameState_Initialize() {
    for(u8 idx=0;idx<MAX_GAMESTATES;++idx) {
        GGameStates[idx] = NULL;
    }
}

void GameState_CreateGameState(GameStateObject *gamestate, u8 uniqueId, CallbackFunc activateFunc, CallbackFunc deactivateFunc, CallbackFunc updateFunc) {
    if(!gamestate)
        return;
    gamestate->uniqueId = uniqueId;
    gamestate->activateFuncPtr = activateFunc;
    gamestate->deactivateFuncPtr = deactivateFunc;
    gamestate->updateFuncPtr = updateFunc;
}

void GameState_Activate(GameStateObject *gamestate) {
    if(!gamestate || GCurrGameState == gamestate)
        return;

    for(u8 idx=0;idx<MAX_GAMESTATES;++idx) {
        if(!GGameStates[idx])
            break;

        if(GGameStates[idx] == gamestate) {
            if(GCurrGameState && GCurrGameState->deactivateFuncPtr)
                GCurrGameState->deactivateFuncPtr();
            GCurrGameState = GGameStates[idx];
            if(GCurrGameState && GCurrGameState->activateFuncPtr) 
                GCurrGameState->activateFuncPtr();
            break;
        }
    }
}

void GameState_ActivateById(u8 uniqueId) {
    for(u8 idx=0;idx<MAX_GAMESTATES;++idx) {
        if(!GGameStates[idx])
            break;

        if(GGameStates[idx]->uniqueId == uniqueId) {
            if(GCurrGameState == GGameStates[idx])
                break;
                
            if(GCurrGameState && GCurrGameState->deactivateFuncPtr)
                GCurrGameState->deactivateFuncPtr();
            GCurrGameState = GGameStates[idx];
            if(GCurrGameState && GCurrGameState->activateFuncPtr) 
                GCurrGameState->activateFuncPtr();
            break;
        }
    }
}