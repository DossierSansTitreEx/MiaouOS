#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <sys/_types/_null.h>
#include <sys/_types/_size_t.h>

__BEGIN_DECL
void*   memcpy(void* __restrict dst, const void* __restrict src, size_t n);
void*   memmove(void* dst, const void* src, size_t len);
void*   memchr(const void* s, int c, size_t num);
int     memcmp(const void* s1, const void* s2, size_t n);
void*   memset(void* b, int c, size_t len);
char*   strcat(char* __restrict s1, const char* __restrict s2);
size_t  strlen(const char* s) __attribute__((pure));
__END_DECL

#endif
