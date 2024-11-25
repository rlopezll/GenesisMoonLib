#include "moon/math_types.h"
#include <maths.h>

MathVector Math_ClampByBox(const MathBox box, MathVector pos) {
    if(pos.x < box.vmin.x)
        pos.x = box.vmin.x;
    else if(pos.x > box.vmax.x)
        pos.x = box.vmax.x;

    if(pos.y < box.vmin.y)
        pos.y = box.vmin.y;
    else if(pos.y > box.vmax.y)
        pos.y = box.vmax.y;

    return pos;
}

bool Math_IsInsideBoxToBox(const MathBox box1, const MathBox box2) {
    bool bIsInside = ((box1.vmin.x < box2.vmin.x || box1.vmin.x < box2.vmax.x) && (box1.vmax.x > box2.vmin.x || box1.vmax.x > box2.vmax.x)) || 
                     ((box2.vmin.x < box1.vmin.x || box2.vmax.x < box1.vmin.x) && (box2.vmin.x > box1.vmax.x || box2.vmax.x > box1.vmax.x));
    bIsInside = bIsInside && (((box1.vmin.y < box2.vmin.y || box1.vmin.y < box2.vmax.y) && (box1.vmax.y > box2.vmin.y || box1.vmax.y > box2.vmax.y)) || 
                              ((box2.vmin.y < box1.vmin.y || box2.vmax.y < box1.vmin.y) && (box2.vmin.y > box1.vmax.y || box2.vmax.y > box1.vmax.y)));
    return bIsInside;       
}
