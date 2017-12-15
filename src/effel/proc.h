#ifndef _EFFEL_PROC_
#define _EFFEL_PROC_ 1

#include <stdint.h>
#include <stddef.h>

#define P_PRESENT       1
#define PROC_MEM_END    0xffff

typedef uint64_t proc_id;
typedef uint32_t proc_mem_id;

struct proc_mem
{
    uint64_t        vaddr;
    uint64_t        paddr;
    size_t          size;
    uint32_t        flags;
    proc_mem_id     next;
};

struct proc
{
    uint32_t        flags;
    proc_mem_id     memory;
};

struct proc_table
{
    struct proc*        procs;
    size_t              proc_count;
    struct proc_mem*    memory;
    size_t              memory_count;
    proc_id             running;
};

void    proc_init();
void    proc_create(const char* s);
void    proc_loop();

#endif
