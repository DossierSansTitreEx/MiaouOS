#include <tss.h>
#include <string.h>

__attribute__ ((aligned (16)))
static char tss_default_stack[4096];

struct tss tss;

void tss_init()
{
    memset(&tss, 0, sizeof(tss));
    tss.rsp[0] = ((uint64_t)tss_default_stack) + 4096;
    tss.iomb = sizeof(tss);
    __asm__ __volatile__ ("ltr %%ax\r\n" :: "a" (0x30));
}
