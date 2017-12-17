#ifndef _EFFEL_ATA
#define _EFFEL_ATA 1

#include <stdint.h>

void ata_read(void* dst, uint64_t lba, size_t size, void* dpte);

#endif
