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
char*   strncat(char* __restrict s1, const char* __restrict s2, size_t n);
char*   strchr(const char* s, int c);
char*   strrchr(const char* s, int c);
int     strcmp(const char* s1, const char* s2);
int     strncmp(const char* s1, const char* s2, size_t n);
size_t  strlen(const char* s) __attribute__((pure));
char*   strcpy(char* dst, const char* src);
char*   strncpy(char* dst, const char* src, size_t n);
size_t  strlcpy(char* __restrict dst, const char* __restrict src, size_t len);
__END_DECL

#endif
