#ifndef _VMM_H
#define _VMM_H 1

#include <stddef.h>
#include <boot/boot_params.h>

#define PAGESIZE 4096

#define VMM_WRITE   0x00000002
#define VMM_USER    0x00000004

void        vmm_init(boot_params* params);
void*       vmm_alloc_over(uint64_t physical_addr, size_t size, uint32_t flags);
void*       vmm_alloc(size_t size, uint32_t flags);
void*       vmm_allocv(uint64_t vaddr, size_t size, uint32_t flags);
void        vmm_switch(uint64_t pml4);
uint64_t    vmm_kfork();

#endif
