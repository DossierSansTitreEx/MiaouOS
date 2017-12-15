#include <string.h>
#include <vmm.h>
#include <util.h>

#define PAGESIZE            4096
#define PAGE_MASK           0xfffffffffffff000
#define PAGE_RECURSE        0x1fe
#define PAGES_PER_TABLE     512
#define KERNEL_HEAP_BASE    0xffffffffc0000000

typedef struct
{
    uint64_t    base;
    uint64_t    length;
} vmm_mem_store;

typedef struct
{
    uint64_t        kernel_heap;
    uint64_t        mem_store_size;
    vmm_mem_store   mem_store[128];
    uint64_t*       free_pages;
    uint64_t        free_pages_count;
    uint64_t        free_pages_capacity;
} virtual_memory_manager;

static virtual_memory_manager vmm;

static uint64_t* addr5(uint16_t a, uint16_t b, uint16_t c, uint16_t d, uint16_t e)
{
    uint64_t addr;

    /* We need to sign extend over 48 bits */
    addr = ((uint64_t)a & 0x100) << 40;
    addr |= (addr << 1);
    addr |= (addr << 2);
    addr |= (addr << 4);
    addr |= (addr << 8);

    addr |= ((uint64_t)a << (12 + 9 * 3));
    addr |= ((uint64_t)b << (12 + 9 * 2));
    addr |= ((uint64_t)c << (12 + 9 * 1));
    addr |= ((uint64_t)d << (12 + 9 * 0));
    addr |= ((uint64_t)e << 3);

    return (uint64_t*)addr;
}

static uint64_t* addr4(uint16_t a, uint16_t b, uint16_t c, uint16_t d)
{
    return addr5(a, b, c, d, 0x0);
}

static void raddr4(uint16_t* dst, uint64_t addr)
{
    dst[0] = ((addr >> (12 + 9 * 3))) & 0x1ff;
    dst[1] = ((addr >> (12 + 9 * 2))) & 0x1ff;
    dst[2] = ((addr >> (12 + 9 * 1))) & 0x1ff;
    dst[3] = ((addr >> (12 + 9 * 0))) & 0x1ff;
}

static uint64_t pml4_read()
{
    uint64_t v;

    __asm__ __volatile__ ("mov %%cr3, %%rax" : "=a" (v));
    return v;
}

static uint64_t pml4_addr()
{
    return (pml4_read() & PAGE_MASK);
}

static uint64_t alloc_vaddr(size_t size)
{
    uint64_t v;

    v = vmm.kernel_heap;
    vmm.kernel_heap += size;
    return v;
}

static uint64_t mem_store_fetch()
{
    uint64_t v;

    v = vmm.mem_store[0].base;
    vmm.mem_store[0].base += PAGESIZE;
    vmm.mem_store[0].length -= PAGESIZE;
    if (vmm.mem_store[0].length == 0)
    {
        memmove(vmm.mem_store, vmm.mem_store + 1, (vmm.mem_store_size - 1) * sizeof(vmm_mem_store));
        vmm.mem_store_size--;
    }
    return v;
}

static uint64_t physical_page()
{
    return mem_store_fetch();
}

static void init_free_list()
{
    uint64_t kernel_page;
    uint64_t physical_page;

    uint64_t pml4;
    uint64_t pdp;
    uint64_t pd;
    uint64_t pt;

    uint16_t pml4_slot;
    uint16_t pdp_slot;
    uint16_t pd_slot;
    uint16_t pt_slot;

    kernel_page = alloc_vaddr(PAGESIZE);
    pml4_slot = ((kernel_page & 0xff8000000000) >> 36);
    pdp_slot = ((kernel_page & 0x7fc0000000) >> 27);
    pd_slot = ((kernel_page & 0x3fe00000) >> 18);
    pt_slot = ((kernel_page & 0x1ff000) >> 9);

    pml4 = pml4_addr();
    physical_page = mem_store_fetch();

    uint64_t tmp;

    tmp = *(uint64_t*)(pml4 + pml4_slot);
    if (tmp & 1)
        pdp = tmp & PAGE_MASK;
    else
    {
        pdp = mem_store_fetch();
        *(uint64_t*)(pml4 + pml4_slot) = (pdp | 3);
    }

    tmp = *(uint64_t*)(pdp + pdp_slot);
    if (tmp & 1)
        pd = tmp & PAGE_MASK;
    else
    {
        pd = mem_store_fetch();
        *(uint64_t*)(pdp + pdp_slot) = (pd | 3);
    }

    tmp = *(uint64_t*)(pd + pd_slot);
    if (tmp & 1)
        pt = tmp & PAGE_MASK;
    else
    {
        pt = mem_store_fetch();
        *(uint64_t*)(pd + pd_slot) = (pt | 3);
    }

    *(uint64_t*)(pt + pt_slot) = (physical_page | 3);

    vmm.free_pages = (uint64_t*)kernel_page;
    vmm.free_pages_count = 0;
    vmm.free_pages_capacity = PAGES_PER_TABLE;
}

static void init_mem_store(boot_params* params)
{
    size_t store_size;
    size_t i;

    store_size = 0;
    for (i = 0; i < params->memory_size; ++i)
    {
        memory_zone* zone;

        zone = params->memory_zones + i;
        if (zone->type != 0x01 || zone->length == 0 || zone->base < 0x100000)
            continue;
        vmm.mem_store[store_size].base = zone->base;
        vmm.mem_store[store_size].length = zone->length;
        store_size++;
        zone->length = 0;
    }
    vmm.mem_store_size = store_size;
    vmm.free_pages = NULL;
    vmm.free_pages_count = 0;
    vmm.free_pages_capacity = 0;
}

static void map_page_raw(uint64_t virtual_addr, uint64_t physical_addr)
{
    uint16_t v[4];
    uint64_t* tmp;

    raddr4(v, virtual_addr);

    /* Check in PML4 */
    tmp = addr5(PAGE_RECURSE, PAGE_RECURSE, PAGE_RECURSE, PAGE_RECURSE, v[0]);
    if ((*tmp & 1) == 0)
        *tmp = (physical_page() | 3);

    /* Check in PDP */
    tmp = addr5(PAGE_RECURSE, PAGE_RECURSE, PAGE_RECURSE, v[0], v[1]);
    if ((*tmp & 1) == 0)
        *tmp = (physical_page() | 3);

    /* Check in PD */
    tmp = addr5(PAGE_RECURSE, PAGE_RECURSE, v[0], v[1], v[2]);
    if ((*tmp & 1) == 0)
        *tmp = (physical_page() | 3);

    tmp = addr5(PAGE_RECURSE, v[0], v[1], v[2], v[3]);
    *tmp = (physical_addr | 3);
}

static size_t page_count_from_size(size_t size)
{
    return (size + (PAGESIZE - 1)) / PAGESIZE;
}

static void map_pages_raw(uint64_t virtual_addr, uint64_t physical_addr, size_t size)
{
    size_t count;

    count = page_count_from_size(size);
    for (size_t i = 0; i < count; ++i)
        map_page_raw(virtual_addr + i * PAGESIZE, physical_addr + i * PAGESIZE);
}

void* vmm_alloc_over(uint64_t physical_addr, size_t size)
{
    uint64_t vaddr;
    size_t count;

    count = page_count_from_size(size);
    vaddr = alloc_vaddr(count * PAGESIZE);
    map_pages_raw(vaddr, physical_addr, size);
    return (void*)vaddr;
}

void* vmm_alloc(size_t size)
{
    uint64_t vaddr;
    size_t count;

    count = page_count_from_size(size);
    vaddr = alloc_vaddr(count * PAGESIZE);
    for (size_t i = 0; i < count; ++i)
        map_page_raw(vaddr + i * PAGESIZE, physical_page());
    return (void*)vaddr;
}

void reload_paging()
{
    __asm__ __volatile__ (
        "mov %%cr3, %%rax\r\n"
        "mov %%rax, %%cr3\r\n"
        ::: "rax"
    );
}

void init_paging()
{
    /* TODO: reclaim memory */
    uint64_t *pte;

    pte = addr4(PAGE_RECURSE, PAGE_RECURSE, PAGE_RECURSE, PAGE_RECURSE);
    *pte = 0x0;
    reload_paging();
}

void vmm_init(boot_params* params)
{
    uint64_t pml4;

    /* Setup a temporary self-paging trick */
    pml4 = pml4_addr();
    *(uint64_t*)(pml4 + 0xff0) = (pml4 | 3);

    vmm.kernel_heap = KERNEL_HEAP_BASE;
    init_mem_store(params);
    init_free_list();
    init_paging();
}
