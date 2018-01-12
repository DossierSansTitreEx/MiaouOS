#ifndef _EFFEL_PROC_
#define _EFFEL_PROC_ 1

#include <stdint.h>
#include <stddef.h>

#define P_PRESENT       1

typedef uint64_t proc_id;

struct cpu_state
{
    /* iret-style state */
    uint64_t    ss;
    uint64_t    rsp;
    uint64_t    rflags;
    uint64_t    cs;
    uint64_t    rip;

    /* regulat state */
    uint64_t    rax;
    uint64_t    rbx;
    uint64_t    rcx;
    uint64_t    rdx;
    uint64_t    rsi;
    uint64_t    rdi;
    uint64_t    rbp;
    uint64_t    r8;
    uint64_t    r9;
    uint64_t    r10;
    uint64_t    r11;
    uint64_t    r12;
    uint64_t    r13;
    uint64_t    r14;
    uint64_t    r15;
} __attribute__ ((packed));

struct proc
{
    struct cpu_state*   cpu_state;
    uint64_t            pml4;
    uint64_t            flags;
};

struct proc_table
{
    struct proc*        procs;
    proc_id             running;
    size_t              proc_count;
} __attribute__ ((packed));

struct proc*    proc_current();
void            proc_init();
void            proc_create(const char* s);
void            proc_schedule();

#endif
