#include <string.h>

char* strncat(char* __restrict s1, const char* __restrict s2, size_t n)
{
    size_t i;
    size_t j;

    i = 0;
    j = 0;

    while (s1[i])
        i++;

    while (s2[j] && j < n)
    {
        s1[i + j] = s2[j];
        j++;
    }
    s1[i + j] = 0;
    return s1;
}
