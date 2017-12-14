#include <boot/boot_params.h>

#define PAGESIZE    4096

struct file_info_
{
    uint64_t    size;
    uint64_t    data[12];
    uint64_t    idata;
};

typedef struct file_info_ file_info;

void disk_read(void* dst, uint64_t lba, uint64_t count);

static volatile char* monitor;
static uint16_t cursor_x;
static uint16_t cursor_y;
static boot_params* g_params;

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

uint64_t next_free_page()
{
    uint64_t page;
    memory_zone* zone;

    for (uint64_t i = 0; i < g_params->memory_size; ++i)
    {
        zone = g_params->memory_zones + i;
        if (zone->base < 0x100000 || zone->length == 0 || zone->type != 0x01)
            continue;
        page = zone->base;
        zone->base += PAGESIZE;
        zone->length -= PAGESIZE;
        return page;
    }
    return 0;
}

uint64_t create_page_layer(uint64_t addr, uint16_t flags)
{
    uint64_t* ptr;
    uint64_t value;

    ptr = (uint64_t*)addr;
    value = *ptr;
    if (!(value & 1))
    {
        value = next_free_page() | flags;
        *ptr = value;
    }
    value &= 0xfffffffffffff000;
    return value;
}

void print_hex(uint64_t);

void map_page(uint64_t dst, uint16_t flags)
{
    uint64_t pml4;
    uint64_t pdp;
    uint64_t pd;
    uint64_t pt;

    uint16_t pml4_slot;
    uint16_t pdp_slot;
    uint16_t pd_slot;
    uint16_t pt_slot;

    pml4_slot = ((dst & 0xff8000000000) >> 36);
    pdp_slot = ((dst & 0x7fc0000000) >> 27);
    pd_slot = ((dst & 0x3fe00000) >> 18);
    pt_slot = ((dst & 0x1ff000) >> 9);

    print_hex(pml4_slot);
    print_hex(pdp_slot);
    print_hex(pd_slot);
    print_hex(pt_slot);

    pml4 = 0x1000;
    pdp = create_page_layer(pml4 + pml4_slot, 3);
    pd = create_page_layer(pdp + pdp_slot, 3);
    pt = create_page_layer(pd + pd_slot, 3);
    create_page_layer(pt + pt_slot, flags);
}

char* virtual_alloc(uint64_t dst, uint64_t size, uint16_t flags)
{
    char* ptr;
    uint64_t end;

    ptr = (char*)dst;

    end = dst + size;
    dst &= 0xfffffffffffff000;
    size = end - dst;
    size = (size + 4095) / 4096;
    for (uint64_t i = 0; i < size; ++i)
    {
        map_page(dst + size * i * 4096, flags);
    }
    return ptr;
}

static void memcpy(char* dst, char* src, uint64_t size)
{
    for (uint64_t i = 0; i < size; ++i)
        dst[i] = src[i];
}

static void memset(char* dst, int value, uint64_t size)
{
    for (uint64_t i = 0; i < size; ++i)
        dst[i] = (char)value;
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

static void load_file(boot_params* params, file_info* info, char* dst, uint64_t offset, uint64_t size)
{
    char* tmp;
    uint64_t chunk_first = offset / 4096;
    uint64_t chunk_last = (offset + size - 1) / 4096;
    uint64_t chunk_count = chunk_last - chunk_first + 1;
    uint64_t i;
    uint64_t j;
    uint64_t base;

    uint64_t range_start;
    uint64_t range_length;

    for (i = 0; i < chunk_count; ++i)
    {
        base = chunk_first + i;

        /* We load into out temp buffer */
        if (base < 12)
            tmp = load_inode(params, info->data[base]);
        else
        {
            tmp = load_inode(params, info->idata);
            j = ((uint64_t*)tmp)[base];
            tmp = load_inode(params, j);
        }

        /* Now we have one of the chunk we need.
           We want to copy it into the destination. */
        /* First chunk */
        range_start = 0;
        range_length = 4096;
        if (i == 0)
            range_start = offset % 4096;
        if (i == (chunk_count - 1))
            range_length = ((offset + size) - 1) % 4096 + 1;
        memcpy(dst, tmp + range_start, range_length);
        dst += range_length;
    }
}

void enter_kernel(uint64_t, boot_params* params);

static void load_elf(boot_params* params, uint64_t inode)
{
    file_info info;
    char* buf;
    char* tmp;
    uint64_t entry_point;
    uint16_t entry_size;
    uint16_t entry_count;
    uint64_t entry_offset;

    buf = load_inode(params, inode);
    info.size = *(uint64_t*)(buf + 0x38);
    for (int i = 0; i < 12; ++i)
        info.data[i] = *(uint64_t*)(buf + 0x40 + 0x08 * i);
    info.idata = *(uint64_t*)(buf + 0x100);

    tmp = (char*)(0x11000);
    load_file(params, &info, tmp, 0, 64);

    entry_point = *(uint64_t*)(tmp + 24);
    entry_offset = *(uint64_t*)(tmp + 32);
    entry_size = *(uint16_t*)(tmp + 54);
    entry_count = *(uint16_t*)(tmp + 56);

    print_hex(entry_offset);
    putchar('\n');
    print_hex(entry_size);
    putchar('\n');
    print_hex(entry_count);
    putchar('\n');

    load_file(params, &info, tmp, entry_offset, entry_count * entry_size);
    for (uint32_t i = 0; i < entry_count; ++i)
    {
        char* dst;
        uint32_t type = *(uint32_t*)(tmp + (entry_size * i) + 0);
        uint32_t flags = *(uint32_t*)(tmp + (entry_size * i) + 4);
        uint64_t p_offset = *(uint64_t*)(tmp + (entry_size * i) + 8);
        uint64_t p_vaddr = *(uint64_t*)(tmp + (entry_size * i) + 16);
        uint64_t p_filesz = *(uint64_t*)(tmp + (entry_size * i) + 32);
        uint64_t p_memsz = *(uint64_t*)(tmp + (entry_size * i) + 40);

        (void)flags;

        if (type != 0x01)
            continue;

        print_hex(p_offset);
        putchar('\n');
        print_hex(p_vaddr);
        putchar('\n');

        dst = virtual_alloc(p_vaddr, p_memsz, 0x03);
        if (p_filesz)
            load_file(params, &info, dst, p_offset, p_filesz);
        if (p_memsz > p_filesz)
            memset(dst + p_filesz, 0, p_memsz - p_filesz);
    }
    enter_kernel(entry_point, params);
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

    load_elf(params, inode);
}

void start(boot_params* params)
{
    g_params = params;

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
