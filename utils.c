#include "utils.h"

size_t max(size_t x, size_t y)
{
    return x > y ? x : y;
}

size_t min(size_t x, size_t y)
{
    return y > x ? x : y;
}

size_t strlen(const char* str)
{
    size_t len = 0;

    while (str[len])
    {
        len++;
    }

    return len;
}

char* strcpy(char* out, const char* str)
{
    return NULL;
}