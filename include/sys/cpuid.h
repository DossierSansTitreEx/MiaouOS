#ifndef _SYS_CPUID_H_
#define _SYS_CPUID_H_

#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECL
inline void cpuid4(uint32_t* dst, uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    __asm__ __volatile__ ("cpuid\r\n"
            : "=a"(dst[0]), "=b"(dst[1]), "=c"(dst[2]), "=d"(dst[3])
            : "a"(a), "b"(b), "c"(c), "d"(d));
}

inline void cpuid(uint32_t* dst, uint32_t code)
{
    __asm__ __volatile__ ("cpuid\r\n"
            : "=a"(dst[0]), "=b"(dst[1]), "=c"(dst[2]), "=d"(dst[3])
            : "a"(code));
}

__END_DECL

#endif
