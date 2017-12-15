#include <string.h>

__attribute__((pure))
size_t strlen(const char* s)
{
    size_t len;

    len = 0;
    while (s[len])
        len++;
    return len;
}
