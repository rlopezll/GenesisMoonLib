#include "moon/level.h"

LevelObject GLevel;

void _Level_Initialize(const MathVector resolution) {
    for(u8 idx=0;idx<MAX_LEVEL_LAYERS;++idx) {
        GLevel.layers[idx].map = NULL;
        GLevel.layers[idx].bOffsetChanged = false;
    }
    GLevel.resolution = resolution;
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    GLevel.autoScrollOp = NONE_SCROLL;
}

void _Level_Update() {
    if(GLevel.autoScrollOp == DIV_SCROLL) {
        u8 otherLayer = GLevel.autoScrollLayer == 0 ? 1 : 0;
        GLevel.layers[GLevel.autoScrollLayer].offset.x = GLevel.layers[otherLayer].offset.x / GLevel.autoScrollValue;
    }
    else if(GLevel.autoScrollOp == MULT_SCROLL) {
        u8 otherLayer = GLevel.autoScrollLayer == 0 ? 1 : 0;
        GLevel.layers[GLevel.autoScrollLayer].offset.x = GLevel.layers[otherLayer].offset.x * GLevel.autoScrollValue;
    }

    for(u8 idx=0;idx<MAX_LEVEL_LAYERS;++idx) {
        if(GLevel.layers[idx].map) {
            // KLog_U2("Offset X: ", offset_x, " Y: ", offset_y);
            MAP_scrollTo(GLevel.layers[idx].map, GLevel.layers[idx].offset.x, GLevel.layers[idx].offset.y);
        }
    }
}

void _Level_PostUpdate() {
    for(u8 idx=0;idx<MAX_LEVEL_LAYERS;++idx) {
        GLevel.layers[idx].bOffsetChanged = false;
    }
}

void _Level_SetOffsetMap(VDPPlane layerId, const MathVector offset) {
    if(layerId >= MAX_LEVEL_LAYERS)
        return;
    
    const MathVector newOffset = Math_ClampByBox(GLevel.layers[layerId].limit, offset);
    if(GLevel.layers[layerId].offset.x != newOffset.x || GLevel.layers[layerId].offset.y != newOffset.y) {
        GLevel.layers[layerId].bOffsetChanged = true;
        GLevel.layers[layerId].offset = newOffset;
    }
}

MathVector _Level_GetOffsetMap(VDPPlane layerId) {
    if(layerId >= MAX_LEVEL_LAYERS)
    {
        MathVector scroll;
        scroll.x = scroll.y = 0;
        return scroll;
    }

    return GLevel.layers[layerId].offset;
}

bool _Level_IsOffsetChanged(VDPPlane layerId) {
    if(layerId >= MAX_LEVEL_LAYERS)
        return false;
    
    return GLevel.layers[layerId].bOffsetChanged;
}

int Level_LoadMap(VDPPlane layerId, const MapDefinition* mapDef, const TileSet* tileSet, const Palette* palette, u16 paletteIdx, u16 baseTileIdx, u16 priority) {
    if(layerId >= MAX_LEVEL_LAYERS)
        return baseTileIdx;

    if(tileSet) {
        VDP_loadTileSet(tileSet, baseTileIdx, DMA);
    }
    if(palette) {
        PAL_setPalette(paletteIdx, palette->data, DMA);
    }
    GLevel.layers[layerId].map = MAP_create(mapDef, layerId, TILE_ATTR_FULL(paletteIdx, priority, FALSE, FALSE, baseTileIdx));
    GLevel.layers[layerId].limit.vmin.x = GLevel.layers[layerId].limit.vmin.y = GLevel.layers[layerId].limit.vmax.x = GLevel.layers[layerId].limit.vmax.y = 0;
    GLevel.layers[layerId].offset.x = GLevel.layers[layerId].offset.y = 0;
    return tileSet->numTile;
}

void Level_SetLimitMap(VDPPlane layerId, const MathBox limit) {
    if(layerId >= MAX_LEVEL_LAYERS)
        return;

    GLevel.layers[layerId].limit.vmin.x = limit.vmin.x;
    GLevel.layers[layerId].limit.vmin.y = limit.vmin.y;
    GLevel.layers[layerId].limit.vmax.x = limit.vmax.x;
    GLevel.layers[layerId].limit.vmax.y = limit.vmax.y;
    GLevel.layers[layerId].offset       = Math_ClampByBox(GLevel.layers[layerId].limit, GLevel.layers[layerId].offset);
}

bool Level_CanGoTo(VDPPlane layerId, Direction direction) {
    if(direction == LEFT && GLevel.layers[layerId].limit.vmin.x == GLevel.layers[layerId].offset.x)
        return false;
    else if(direction == RIGHT && GLevel.layers[layerId].limit.vmax.x == GLevel.layers[layerId].offset.x)
        return false;   
    else if(direction == UP && GLevel.layers[layerId].limit.vmin.y == GLevel.layers[layerId].offset.y)
        return false;
    else if(direction == DOWN && GLevel.layers[layerId].limit.vmax.y == GLevel.layers[layerId].offset.y)
        return false;
    return true;
}

MathVector Level_WorldToLocal(VDPPlane layerId, const MathVector world) {
    MathVector local;
    local.x = world.x;
    local.y = world.y;
    if(layerId < MAX_LEVEL_LAYERS) {
        const LevelLayer* currLayer = &GLevel.layers[layerId];
        local.x = local.x - currLayer->offset.x;
        local.y = local.y - currLayer->offset.y;
    }
    return local;
}        

MathVector Level_LocalToWorld(VDPPlane layerId, const MathVector local) {
    MathVector world;
     if(layerId < MAX_LEVEL_LAYERS) {
        const LevelLayer* currLayer = &GLevel.layers[layerId];
        world.x = local.x + currLayer->offset.x;
        world.y = local.y + currLayer->offset.y;
    } else {
        world.x = local.x;
        world.y = local.y;
    }
    return world;
}


bool Level_IsVisible(VDPPlane layerId, const MathVector world, const MathVector size) {
    MathVector local = Level_WorldToLocal(layerId, world);
    return local.x + size.x > 0 && local.y + size.y > 0 && local.x < GLevel.resolution.x && local.y < GLevel.resolution.y;
}

void Level_SetAutoScroll(VDPPlane layerId, u8 autoScrollValue, EAutoScrollOp op) {
    GLevel.autoScrollLayer = layerId;
    GLevel.autoScrollOp = op;
    GLevel.autoScrollValue = autoScrollValue;
}
