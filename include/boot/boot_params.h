#ifndef INCLUDED_BOOT_PARAMS_H
#define INCLUDED_BOOT_PARAMS_H

#include <stdint.h>

struct mbr_entry_
{
    uint8_t     status;
    uint8_t     chs[3];
    uint8_t     type;
    uint8_t     chs_end[3];
    uint32_t    lba_base;
    uint32_t    lba_length;
} __attribute__ ((packed));

typedef struct mbr_entry_ mbr_entry;

struct memory_zone_
{
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t flags;
} __attribute__ ((packed));

typedef struct memory_zone_ memory_zone;

struct dpte
{
    uint16_t    port_io;
    uint16_t    port_ctl;
    uint8_t     head;
    uint8_t     padding[13];

} __attribute__ ((packed));

struct boot_params_
{
    uint64_t        memory_size;
    memory_zone*    memory_zones;
    mbr_entry*      mbr;
    struct dpte     dpte;
    uint8_t         drive;
} __attribute__ ((packed));

typedef struct boot_params_ boot_params;

#endif
