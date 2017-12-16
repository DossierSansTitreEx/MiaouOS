#include <string.h>

size_t strlcpy(char* __restrict dst, const char* __restrict src, size_t len)
{
    size_t i;
    char c;

    for (i = 0; i < len; ++i)
    {
        c = src[i];
        dst[i] = c;
        if (c == 0)
            return strlen(src);
    }
    dst[len] = 0;
    return strlen(src);
}
