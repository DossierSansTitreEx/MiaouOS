#include <proc.h>
#include <vmm.h>

struct proc_table ptable;

void proc_init()
{
    ptable.procs = vmm_alloc(sizeof(struct proc) * 8092);
    ptable.proc_count = 0;
    ptable.memory = vmm_alloc(sizeof(struct proc_mem) * 32768);
    ptable.memory_count = 0;
    ptable.running = 0;
}
