#ifndef HAZEOS_VGA_H
#define HAZEOS_VGA_H

#include <stddef.h>

typedef enum
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
} VGAColor;

typedef struct
{
	VGAColor fg, bg;
} VGAPalette;

typedef struct
{
    char c;
    size_t x, y;
    VGAPalette palette;
} VGACharacter;

#define PALETTE(fg, bg) (VGAPalette){ fg, bg }
#define INVERT_PALETTE(palette) (VGAPalette){ palette.bg, palette.fg }

#define CHARACTER(c, x, y, palette) (VGACharacter){ c, x, y, palette }

#endif
