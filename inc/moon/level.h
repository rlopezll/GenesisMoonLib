#ifndef _LEVEL_
#define _LEVEL_

#include "moon/math_types.h"
#include <genesis.h>

#define MAX_LEVEL_LAYERS 3 // VDPPlane -> BG_A = 0, BG_B = 1, WINDOW = 2

typedef enum EAutoScrollOp {
    MULT_SCROLL,
    DIV_SCROLL,
    NONE_SCROLL
} EAutoScrollOp;

typedef struct LevelLayer {
    MathVector  offset;
    MathBox     limit;
    Map*        map;
    bool        bOffsetChanged;
} LevelLayer;

typedef struct LevelObject {
    LevelLayer layers[MAX_LEVEL_LAYERS];
    MathVector resolution;

    VDPPlane      autoScrollLayer;
    EAutoScrollOp autoScrollOp;
    u8            autoScrollValue;
} LevelObject;

extern LevelObject GLevel; // Global

// ---- Internal use
void _Level_Initialize(const MathVector resolution);
void _Level_Update();
void _Level_PostUpdate();
void _Level_SetOffsetMap(VDPPlane layerId, const MathVector offset);
MathVector _Level_GetOffsetMap(VDPPlane layerId);
bool _Level_IsOffsetChanged(VDPPlane layerId);
// ----

int  Level_LoadMap(VDPPlane layerId, const MapDefinition* mapDef, const TileSet* tileSet, const Palette* palette, u16 paletteIdx, u16 baseTileIdx, u16 priority);
void Level_SetLimitMap(VDPPlane layerId, const MathBox limit); // vmax.x = Width map - 320 vmax.y = Height map - 240
bool Level_CanGoTo(VDPPlane layerId, Direction direction);

MathVector  Level_WorldToLocal(VDPPlane layerId, const MathVector world);
MathVector  Level_LocalToWorld(VDPPlane layerId, const MathVector local);
bool        Level_IsVisible(VDPPlane layerId, const MathVector world, const MathVector size);

void        Level_SetAutoScroll(VDPPlane layerId, u8 autoScrollValue, EAutoScrollOp op);

#endif