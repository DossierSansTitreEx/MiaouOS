#include <string.h>

char* strrchr(const char* s, int c)
{
    char cc;
    char sc;
    size_t i;

    cc = c;
    i = strlen(s) + 1;
    while (i--)
    {
        sc = s[i - 1];
        if (sc == cc)
            return (char*)s + i;
    }
    return NULL;
}
