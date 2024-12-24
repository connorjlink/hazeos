#ifndef HAZEOS_GRAPHICS_H
#define HAZEOS_GRAPHICS_H

#include <stddef.h>

typedef struct
{
    size_t x, y;
} Point;

typedef struct
{
    Point pos, size;
} Rect;

#define POINT(x, y) (Point){ x, y }
#define RECT(pos, size) (Rect){ pos, size };

#endif
