#include <stdint.h>
#include <interrupt.h>
#include <x86.h>
#include <string.h>

struct idt_entry
{
    uint16_t    offset_low;
    uint16_t    selector;
    uint8_t     zero1;
    uint8_t     type;
    uint16_t    offset_mid;
    uint32_t    offset_high;
    uint32_t    zero2;
} __attribute__ ((packed));

struct idt
{
    uint16_t            size;
    struct idt_entry*   ptr;
} __attribute__ ((packed));

__attribute__ ((aligned (16)))
static struct idt idt;
static struct idt_entry entries[256];

static void pic_init()
{
    outb(0x20, 0x11);
    io_wait();
    outb(0xa0, 0x11);
    io_wait();
    outb(0x21, 0x20);
    io_wait();
    outb(0xa1, 0x28);
    io_wait();
    outb(0x21, 0x04);
    io_wait();
    outb(0xa1, 0x02);
    io_wait();
    outb(0x21, 0x01);
    io_wait();
    outb(0xa1, 0x01);
    io_wait();
    outb(0x21, 0xff);
    io_wait();
    outb(0xa1, 0xff);
    io_wait();
}

void irq_enable(uint8_t line)
{
    uint16_t port;
    uint8_t mask;

    if (line < 8)
    {
        port = 0x21;
    }
    else
    {
        port = 0xa1;
        line -= 8;
    }
    mask = inb(port);
    mask &= ~(1 << line);
    outb(port, mask);
}

void irq_disable(uint8_t line)
{
    uint16_t port;
    uint8_t mask;

    if (line < 8)
    {
        port = 0x21;
    }
    else
    {
        port = 0xa1;
        line -= 8;
    }
    mask = inb(port);
    mask |= (1 << line);
    outb(port, mask);
}

void interrupt_register(uint8_t slot, void* isr)
{
    uint64_t isr_value;
    struct idt_entry* e;

    isr_value = (uint64_t)isr;
    e = entries + slot;
    e->offset_low = isr_value & 0xffff;
    e->selector = 0x08;
    e->zero1 = 0;
    e->type = 0x8e;
    e->offset_mid = (isr_value >> 16) & 0xffff;
    e->offset_high = (isr_value >> 32) & 0xffffffff;
}

void interrupt_init()
{
    pic_init();
    memset(entries, 0, sizeof(entries));
    idt.size = sizeof(entries) - 1;
    idt.ptr = entries;
    __asm__ __volatile__ ("lidt (%0)\r\n" :: "r" (&idt));
}
