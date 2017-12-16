#ifndef _EFFEL_MFS_H
#define _EFFEL_MFS_H 1

#include <stddef.h>
#include <stdint.h>

struct mfs
{
    uint8_t     device;
    uint64_t    lba;
    uint64_t    root;
    char*       buffers[4];
};

struct mfs_fileinfo_
{
    uint64_t    inode;
    uint64_t    size;
    uint64_t    data[12];
    uint64_t    idata;
    uint64_t    idata2;
};

typedef struct mfs_fileinfo_ mfs_fileinfo;

void    mfs_init(uint8_t device, uint64_t lba);
void    mfs_read_info(mfs_fileinfo* info, const char* path);
void    mfs_read(char* dst, const mfs_fileinfo* info, size_t offset, size_t length);

#endif
