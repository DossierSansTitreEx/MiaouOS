#include <vmm.h>
#include <screen.h>
#include <proc.h>

void kmain(boot_params* params)
{
    vmm_init(params);
    init_screen();
    proc_init();

    kputs("Hello from the kernel!");

    for (;;) {}
}
