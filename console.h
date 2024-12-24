#ifndef HAZEOS_CONSOLE_H
#define HAZEOS_CONSOLE_H

#include <stdint.h>

#include "defs.h"

void write_console(Rect parent, Point* cursor, uint8_t color, const char* text);

#endif
