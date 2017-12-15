#include <vmm.h>
#include <screen.h>

void kmain(boot_params* params)
{
    vmm_init(params);
    init_screen();

    kputs("Hello from the kernel!");

    for (;;) {}
}
