#include <mfs.h>
#include <ata.h>
#include <vmm.h>
#include <string.h>

static struct mfs mfs;

static void read_inode(char* dst, uint64_t inode)
{
    ata_read(dst, mfs.lba + 40 + inode * 8, 8, &mfs.dpte);
}

void mfs_init(const boot_params* params)
{
    mfs.device = params->drive;
    mfs.lba = params->mbr->lba_base;
    mfs.dpte = params->dpte;
}

void mfs_init_buffers()
{
    for (size_t i = 0; i < 4; ++i)
        mfs.buffers[i] = vmm_alloc(4096, 0);
    read_inode(mfs.buffers[0], 0);
    mfs.root = *(uint64_t*)(mfs.buffers[0] + 0x18);
}

static void read_info_inode(mfs_fileinfo* info, uint64_t inode)
{
    info->inode = inode;
    read_inode(mfs.buffers[0], inode);
    info->size = *(uint64_t*)(mfs.buffers[0] + 0x38);
    for (size_t i = 0; i < 12; ++i)
        info->data[i] = *(uint64_t*)(mfs.buffers[0] + 0x40 + 8 * i);
    info->idata = *(uint64_t*)(mfs.buffers[0] + 0x100);
}

static uint64_t find_child(uint64_t parent, const char* name)
{
    mfs_fileinfo info;
    uint16_t namelen;
    uint64_t inode;
    char namebuf[256];
    size_t i;

    read_info_inode(&info, parent);
    mfs_read(mfs.buffers[0], &info, 0, 4096);
    i = 0;
    for (;;)
    {
        namelen = *(uint16_t*)(mfs.buffers[0] + i);
        i += 2;
        memcpy(namebuf, mfs.buffers[0] + i, namelen);
        namebuf[namelen] = 0;
        i += namelen;
        inode = *(uint64_t*)(mfs.buffers[0] + i);
        if (strcmp(name, namebuf) == 0)
            return inode;
    }
}

void mfs_read_info(mfs_fileinfo* info, const char* path)
{
    uint64_t inode;
    char name[256];
    size_t i;

    inode = mfs.root;
    for (;;)
    {
        while (*path == '/')
            path++;
        if (*path == 0)
            break;

        for (i = 0; path[i] && path[i] != '/'; i++)
            name[i] = path[i];
        name[i] = 0;
        path += i;
        inode = find_child(inode, name);
    }
    read_info_inode(info, inode);
}

void mfs_read(char* dst, const mfs_fileinfo* info, size_t offset, size_t length)
{
    size_t chunk_first;
    size_t chunk_last;
    size_t chunk_count;
    size_t chunk;

    uint64_t inode;
    size_t len;

    chunk_first = offset / 4096;
    chunk_last = (offset + length - 1) / 4096;
    chunk_count = chunk_last - chunk_first + 1;

    size_t range_start;
    size_t range_end;

    for (size_t i = 0; i < chunk_count; ++i)
    {
        chunk = chunk_first + i;
        if (chunk < 12)
            inode = info->data[chunk];
        else
        {
            read_inode(mfs.buffers[0], info->idata);
            inode = *(uint64_t*)(mfs.buffers[0] + (chunk - 12) * 8);
        }

        if (i == 0)
            range_start = (offset % 4096);
        else
            range_start = 0;

        if (i == chunk_count - 1)
            range_end = ((offset + length - 1) % 4096) + 1;
        else
            range_end = 4096;

        len = range_end - range_start;
        if (len == 4096)
            read_inode(dst, inode);
        else
        {
            read_inode(mfs.buffers[3], inode);
            memcpy(dst, mfs.buffers[3] + range_start, len);
        }
        dst += len;
    }
}
