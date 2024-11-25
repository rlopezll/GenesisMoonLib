#ifndef _GAME_
#define _GAME_

#include <types.h>

typedef struct GameConfigObject {
    u16 spriteManagerNumTiles; // by default 420 tiles
} GameConfigObject;

extern u8 GGameCurrFrameId;

void Game_Initialize();
void Game_InitializeEx(const GameConfigObject* gameConfig);
void Game_Update();
void Game_Finalize();

#endif