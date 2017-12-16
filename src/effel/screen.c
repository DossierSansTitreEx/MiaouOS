#include <stdint.h>
#include <string.h>
#include <screen.h>
#include <vmm.h>

static char* monitor;
static int32_t cursor_x;
static int32_t cursor_y;

void kscroll()
{
    memmove(monitor, monitor + 80 * 2, 80 * 24 * 2);
    memset(monitor + 80 * 24 * 2, 0, 80 * 2);
    cursor_x = 0;
    cursor_y--;
    if (cursor_y < 0)
        cursor_y = 0;
}

void kputchar(char c)
{
    uint32_t index;

    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;
        return;
    }

    if (cursor_y >= 25)
        kscroll();

    index = cursor_y * 80 + cursor_x;
    monitor[index * 2 + 0] = c;
    monitor[index * 2 + 1] = 0x07;

    cursor_x++;
    if (cursor_x >= 80)
    {
        cursor_x = 0;
        cursor_y++;
    }
}

void kprint(const char* s)
{
    for (uint32_t i = 0; s[i]; ++i)
        kputchar(s[i]);
}

void kputs(const char* s)
{
    kprint(s);
    kputchar('\n');
}

static void _kprint_hex(uint64_t num)
{
    static const char* const table = "0123456789abcdef";

    uint8_t nibble;

    nibble = num % 16;
    num /= 16;
    if (num)
        _kprint_hex(num);
    kputchar(table[nibble]);
}

void kprint_hex(uint64_t num)
{
    kprint("0x");
    _kprint_hex(num);
}

void screen_init()
{
    monitor = vmm_alloc_over(0xb8000, 80 * 25 * 2);
    cursor_x = 0;
    cursor_y = 0;
    for (int i = 0; i < 25; ++i)
        kscroll();
}
