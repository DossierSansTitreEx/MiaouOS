#ifndef _EFFEL_PROC_
#define _EFFEL_PROC_ 1

#include <stdint.h>
#include <stddef.h>

#define P_PRESENT       1

typedef uint64_t proc_id;

struct proc
{
    uint64_t    pml4;
    uint64_t    flags;
};

struct proc_table
{
    struct proc*        procs;
    size_t              proc_count;
    proc_id             running;
};

void    proc_init();
void    proc_create(const char* s);
void    proc_loop();

#endif
