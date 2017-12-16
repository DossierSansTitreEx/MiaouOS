#include <vmm.h>
#include <screen.h>
#include <proc.h>
#include <mfs.h>

void kmain(boot_params* params)
{
    uint8_t device;
    uint64_t lba;

    device = params->drive;
    lba = params->mbr->lba_base;

    vmm_init(params);
    screen_init();
    proc_init();
    mfs_init(device, lba);

    kputs("Hello from the kernel!");
    proc_create("/bin/sh");

    for (;;) {}
}
