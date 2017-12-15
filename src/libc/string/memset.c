#include <string.h>

void* memset(void* b, int c, size_t len)
{
    unsigned char byte;

    byte = c;
    for (size_t i = 0; i < len; ++i)
        *((unsigned char*)b + i) = byte;
    return b;
}
