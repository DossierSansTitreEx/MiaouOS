#include <string.h>

char* strcat(char* __restrict s1, const char* __restrict s2)
{
    size_t i;
    size_t j;

    i = 0;
    while (s1[i])
        i++;
    j = 0;
    while (s2[j])
    {
        s1[i + j] = s2[j];
        j++;
    }
    s1[i + j] = 0;
    return s1;
}
