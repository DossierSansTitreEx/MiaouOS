#include <boot/boot_params.h>

#define PAGESIZE    4096

void disk_read(void* dst, uint64_t lba, uint64_t count);

static volatile char* monitor;
static uint16_t cursor_x;
static uint16_t cursor_y;

static void scroll()
{
    for (uint32_t i = 0; i < (80 * 24 * 2); ++i)
        monitor[i] = monitor[i + 80 * 2];
    for (uint32_t i = 0; i < 80 * 2; ++i)
        monitor[(24 * 80 * 2) + i] = 0;
    cursor_x = 0;
    cursor_y = 24;
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

static void _print_hex(uint64_t num)
{
    static const char* const table = "0123456789abcdef";

    uint8_t nibble;

    nibble = num % 16;
    num /= 16;
    if (num)
        _print_hex(num);
    putchar(table[nibble]);
}

static void print_hex(uint64_t num)
{
    print("0x");
    _print_hex(num);
}

static void dump_memory_map(boot_params* params)
{
    memory_zone* zone;

    puts("Memory Map:");
    for (uint64_t i = 0; i < params->memory_size; ++i)
    {
        zone = params->memory_zones + i;

        putchar(' ');
        print_hex(zone->base);
        print(" -> ");
        print_hex(zone->base + zone->length);
        print(" (type: ");
        print_hex(zone->type);
        puts(")");
    }
}

uint64_t next_free_page(boot_params* params)
{
    uint64_t page;
    memory_zone* zone;

    for (uint64_t i = 0; i < params->memory_size; ++i)
    {
        zone = params->memory_zones + i;
        if (zone->base < 0x100000 || zone->length == 0 || zone->type != 0x01)
            continue;
        page = zone->base;
        zone->base += PAGESIZE;
        zone->length -= PAGESIZE;
        return page;
    }
    return 0;
}

static int strequ(const char* s1, const char* s2)
{
    int i;

    i = 0;
    while (s1[i] == s2[i])
    {
        i++;
        if (s1[i] == 0)
            return 1;
    }
    return 0;
}

static char* disk_buffer(uint64_t lba, uint64_t count)
{
    char* dst;

    dst = (char*)0x10000;
    disk_read(dst, lba, count);
    return dst;
}

static char* load_chunk(boot_params* params, uint64_t chunk)
{
    uint64_t lba;

    lba = params->mbr->lba_base + 40 + chunk * 8;
    return disk_buffer(lba, 8);
}

static char* load_inode(boot_params* params, uint64_t inode)
{
    return load_chunk(params, inode);
}

static uint64_t find_child(boot_params* params, uint64_t parent, const char* child_name)
{
    char* buf;
    char name[256];
    uint16_t namelen;
    uint64_t tmp;
    int i;

    buf = load_inode(params, parent);
    tmp = *(uint64_t*)(buf + 0x40);
    buf = load_inode(params, tmp);
    for (;;)
    {
        namelen = *(uint16_t*)(buf);
        buf += 2;
        for (i = 0; i < namelen; ++i)
            name[i] = buf[i];
        name[i] = 0;
        buf += namelen;
        tmp = *(uint64_t*)(buf);
        buf += 8;
        if (strequ(name, child_name))
            return tmp;
    }
}

static void load_effel(boot_params* params)
{
    uint64_t inode;
    char* buffer;

    buffer = load_inode(params, 0x0);
    inode = *(uint64_t*)(buffer + 0x18);
    puts(buffer);
    print("Root: ");
    print_hex(inode);
    putchar('\n');

    inode = find_child(params, inode, "boot");
    inode = find_child(params, inode, "effel");

    print("Found kernel inode: ");
    print_hex(inode);
    putchar('\n');
}

void start(boot_params* params)
{
    monitor = (char*)0xb8000;
    cursor_x = 0;
    cursor_y = 0;

    puts("Entering 64-bit mode");
    print("Boot params: ");
    print_hex((uint64_t)params);
    putchar('\n');
    print("Boot drive: ");
    print_hex(params->drive);
    putchar('\n');
    print("Partition: ");
    print_hex(params->mbr->lba_base);
    print(" -> ");
    print_hex(params->mbr->lba_base + params->mbr->lba_length);
    putchar('\n');

    dump_memory_map(params);

    load_effel(params);

    for (;;) {}
}
