#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <sys/_types/_null.h>
#include <sys/_types/_size_t.h>

__BEGIN_DECL
size_t  strlen(const char* s) __attribute__((pure));
void*   memset(void* b, int c, size_t len);
void*   memcpy(void* __restrict dst, const void* __restrict src, size_t n);
void*   memmove(void* dst, const void* src, size_t len);
__END_DECL

#endif
