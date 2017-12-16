#include <gdt.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <util.h>

struct gdt_entry
{
    unsigned int limit_low:16;
    unsigned int base_low:24;
    unsigned int accessed:1;
    unsigned int read_write:1;
    unsigned int conforming_expand_down:1;
    unsigned int code:1;
    unsigned int always_1:1;
    unsigned int dpl:2;
    unsigned int present:1;
    unsigned int limit_high:4;
    unsigned int zero:1;
    unsigned int long_mode:1;
    unsigned int big:1;
    unsigned int gran:1;
    unsigned int base_high:8;
} __attribute__ ((packed));

struct gdt
{
    uint16_t length;
    uint64_t ptr;
} __attribute__ ((packed));

__attribute__ ((aligned (16)))
static struct gdt gdt;
static struct gdt_entry entries[5];

static void gdt_init_seg(size_t seg, int code, int ring)
{
    struct gdt_entry* e;

    e = entries + (seg >> 3);
    e->limit_low = 0;
    e->base_low = 0;
    e->accessed = 0;

    if (code)
    {
        e->read_write = 0;
        e->code = 1;
    }
    else
    {
        e->read_write = 1;
        e->code = 0;
    }
    e->conforming_expand_down = 0;
    e->always_1 = 1;
    e->dpl = ring & 0x03;
    e->present = 1;
    e->limit_high = 0;
    e->zero = 0;
    e->long_mode = 1;
    e->big = 0;
    e->gran = 0;
    e->base_high = 0;
}

void gdt_switch(void* addr, uint64_t code_seg, uint64_t data_seg);

void gdt_init()
{
    memset(entries, 0, sizeof(entries));
    gdt_init_seg(0x08, 1, 0);
    gdt_init_seg(0x10, 0, 0);
    gdt_init_seg(0x18, 1, 3);
    gdt_init_seg(0x20, 0, 3);

    gdt.ptr = (uint64_t)entries;
    gdt.length = (sizeof(entries) - 1);

    gdt_switch(&gdt, 0x08, 0x10);
}
