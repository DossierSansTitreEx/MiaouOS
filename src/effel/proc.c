#include <proc.h>
#include <vmm.h>

struct proc_table ptable;

void proc_init()
{
    ptable.procs = vmm_alloc(sizeof(struct proc) * 8000);
    ptable.proc_count = 0;
    ptable.running = 0;
}
