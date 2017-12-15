#ifndef _VMM_H
#define _VMM_H 1

#include <stddef.h>
#include <boot/boot_params.h>

#define PAGESIZE 4096

void vmm_init(boot_params* params);
void* vmm_alloc_over(uint64_t physical_addr, size_t size);
void* vmm_alloc(size_t size);

#endif
