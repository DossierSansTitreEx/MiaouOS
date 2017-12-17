#ifndef X86_H
#define X86_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t v)
{
    __asm__ __volatile__ ("outb %0, %1" :: "a"(v), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t v)
{
    __asm__ __volatile__ ("outw %0, %1" :: "a"(v), "d"(port));
}

static inline void outl(uint16_t port, uint32_t v)
{
    __asm__ __volatile__ ("outl %0, %1" :: "a"(v), "d"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t v;

    __asm__ __volatile__ ("inb %1, %0" : "=a"(v) : "Nd"(port));
    return v;
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t v;

    __asm__ __volatile__ ("inw %1, %0" : "=a"(v) : "d"(port));
    return v;
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t v;

    __asm__ __volatile__ ("inl %1, %0" : "=a"(v) : "d"(port));
    return v;
}

static inline void io_wait()
{
    outb(0x80, 0x00);
}

#endif
