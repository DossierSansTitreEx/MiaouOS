#ifndef _SYS_CPUINFO_H_
#define _SYS_CPUINFO_H_

#include <sys/cdefs.h>
#include <sys/cpuid.h>

#define CPUINFO_SSE     (1 << 0)
#define CPUINFO_SSE2    (1 << 1)
#define CPUINFO_SSE3    (1 << 2)
#define CPUINFO_SSSE3   (1 << 3)
#define CPUINFO_SSE4_1  (1 << 4)
#define CPUINFO_SSE4_2  (1 << 5)
#define CPUINFO_SSE4A   (1 << 6)
#define CPUINFO_SSE5    (1 << 7)
#define CPUINFO_AVX     (1 << 8)
#define CPUINFO_AVX2    (1 << 9)
#define CPUINFO_AVX_512 (1 << 10)

typedef struct
{
    uint32_t    support;
    char        vendor[13];
}
cpuinfo_t;

__BEGIN_DECL
void    cpuinfo(cpuinfo_t* info);
__END_DECL

#endif
