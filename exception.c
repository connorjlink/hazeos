#include "exception.h"
#include "console.h"
#include "defs.h"

void exception_handler(uint64_t code)
{
    write_console(_console_rect, &_console_cursor, _active_color, "Error!");
}
