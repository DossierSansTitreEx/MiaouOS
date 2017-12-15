#include <string.h>

char* strchr(const char* s, int c)
{
    size_t i;
    char cc;
    char sc;

    i = 0;
    cc = c;

    for (;;)
    {
        sc = s[i];

        if (sc == cc)
            return (char*)s + i;
        if (sc == 0)
            return NULL;
        i++;
    }
}
