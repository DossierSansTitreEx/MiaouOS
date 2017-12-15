#include <string.h>

int strncmp(const char* s1, const char* s2, size_t n)
{
    size_t i;
    int diff;
    int c1;
    int c2;

    for (i = 0; i < n; ++i)
    {
        c1 = (unsigned char)(s1[i]);
        c2 = (unsigned char)(s2[i]);
        diff = c1  - c2;
        if (diff)
            return diff;
        if (c1 == 0)
            return 0;
    }
    return 0;
}
