#ifndef _UTIL_H
#define _UTIL_H 1

static inline void magic_break()
{
    __asm__ __volatile__ ("xchg %bx, %bx");
}

#endif
