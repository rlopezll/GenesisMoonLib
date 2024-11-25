#ifndef _MATH_TYPES_
#define _MATH_TYPES_

#include <types.h>

typedef struct MathVector
{
    s16 x;
    s16 y;
} MathVector;

typedef struct MathBox
{
    MathVector vmin;
    MathVector vmax;
} MathBox;

typedef enum {
    NONE,
    LEFT,
    RIGHT,
    UP,
    DOWN
} Direction;

MathVector Math_ClampByBox(const MathBox box, MathVector pos);
bool       Math_IsInsideBoxToBox(const MathBox box1, const MathBox box2);

#endif