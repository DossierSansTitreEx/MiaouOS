#ifndef TSS_H
#define TSS_H

#include <stdint.h>

struct tss
{
    uint32_t zero1;
    uint64_t rsp[3];
    uint64_t zero2;
    uint64_t ist[8];
    uint64_t zero3;
    uint16_t zero4;
    uint16_t iomb;
} __attribute__ ((packed, aligned(16)));

extern struct tss tss;

void tss_init();

#endif
