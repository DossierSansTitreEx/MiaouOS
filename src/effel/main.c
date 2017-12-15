#include <stdint.h>
#include <string.h>

static char* monitor;
static int32_t cursor_x;
static int32_t cursor_y;

static void scroll()
{
    memmove(monitor, monitor + 80 * 2, 80 * 24 * 2);
    memset(monitor + 80 * 24 * 2, 0, 80 * 2);
    cursor_x = 0;
    cursor_y--;
    if (cursor_y < 0)
        cursor_y = 0;
}

static void putchar(char c)
{
    uint32_t index;

    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;
        return;
    }

    if (cursor_y >= 25)
        scroll();

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

static void print(const char* s)
{
    for (uint32_t i = 0; s[i]; ++i)
        putchar(s[i]);
}

static void puts(const char* s)
{
    print(s);
    putchar('\n');
}


void kmain()
{
    monitor = (char*)0xb8000;
    cursor_x = 0;
    cursor_y = 0;

    for (int i = 0; i < 25; ++i)
        scroll();
    puts("Hello from the kernel!");
    for (;;) {}
}
