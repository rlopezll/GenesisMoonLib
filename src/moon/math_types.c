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
