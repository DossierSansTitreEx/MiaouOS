#include <string.h>

int memcmp(const void* s1, const void* s2, size_t n)
{
    const unsigned char* u1;
    const unsigned char* u2;
    int diff;

    u1 = s1;
    u2 = s2;
    for (size_t i = 0; i < n; ++i)
    {
        diff = (int)(u1[i]) - (int)(u2[i]);
        if (diff)
            return diff;
    }
    return 0;
}
