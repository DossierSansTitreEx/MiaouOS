#include <string.h>

void* memchr(const void* s, int c, size_t num)
{
    unsigned char* cptr;
    unsigned char cc;

    cptr = (unsigned char*)s;
    cc = c;

    for (size_t i = 0; i < num; ++i)
    {
        if (cptr[i] == cc)
            return cptr + i;
    }
    return NULL;
}
