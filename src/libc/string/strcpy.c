#include <string.h>

char* strcpy(char* dst, const char* src)
{
    size_t i;
    char c;

    i = 0;
    for (;;)
    {
        c = src[i];
        dst[i] = c;
        if (c == 0)
            break;
        i++;
    }
    return dst;
}
