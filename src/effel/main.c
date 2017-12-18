#include <gdt.h>
#include <tss.h>
#include <vmm.h>
#include <screen.h>
#include <proc.h>
#include <mfs.h>
#include <syscall.h>
#include <interrupt.h>

void kmain(boot_params* params)
{
    gdt_init();
    tss_init();
    mfs_init(params);
    vmm_init(params);
    interrupt_init();
    screen_init();
    proc_init();
    mfs_init_buffers();
    syscall_init();

    kputs("Hello from the kernel!");
    for (int i = 0; i < 4; ++i)
        proc_create("/bin/sh");

    proc_schedule();

    for (;;) {}
}
