#include <string.h>

char* strncpy(char* dst, const char* src, size_t n)
{
    size_t i;
    char c;

    for (i = 0; i < n; ++i)
    {
        c = src[i];
        dst[i] = c;
        if (c == 0)
            break;
        i++;
    }
    for (i = 0; i < n; ++i)
        dst[i] = 0;
    return dst;
}
