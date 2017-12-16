#include <proc.h>
#include <vmm.h>
#include <screen.h>
#include <mfs.h>
#include <string.h>
#include <util.h>

#define PROCJMP(addr, stack)    __asm__ __volatile__ ("movq %0, %%rbp\r\n"      \
                                                      "pushq $0x23\r\n"         \
                                                      "pushq %0\r\n"            \
                                                      "pushfq\r\n"              \
                                                      "pushq $0x1b\r\n"         \
                                                      "pushq %1\r\n"            \
                                                      "iretq" :: "r"((uint64_t)stack), "r"(addr));

struct proc_table ptable;

void proc_init()
{
    ptable.procs = vmm_alloc(sizeof(struct proc) * 8092, 0);
    ptable.proc_count = 0;
    ptable.running = 0;
}

static void load_elf(struct proc* p, const mfs_fileinfo* info)
{
    char* ptr;

    char elf_header[64];
    char section_header[256];

    uint64_t entry;
    uint64_t section_offset;
    uint16_t section_size;
    uint16_t section_count;

    uint32_t type;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_filesz;
    uint64_t p_memsz;

    mfs_read(elf_header, info, 0, 64);
    entry = *(uint64_t*)(elf_header + 24);
    section_offset = *(uint64_t*)(elf_header + 32);
    section_size = *(uint16_t*)(elf_header + 54);
    section_count = *(uint16_t*)(elf_header + 56);

    for (size_t i = 0; i < section_count; ++i)
    {
        mfs_read(section_header, info, section_offset + section_size * i, section_size);
        type = *(uint32_t*)(section_header + 0);

        if (type != 0x01)
            continue;

        p_offset = *(uint64_t*)(section_header + 8);
        p_vaddr = *(uint64_t*)(section_header + 16);
        p_filesz = *(uint64_t*)(section_header + 32);
        p_memsz = *(uint64_t*)(section_header + 40);

        ptr = vmm_allocv(p_vaddr, p_memsz, VMM_USER | VMM_WRITE);
        if (p_filesz)
            mfs_read(ptr, info, p_offset, p_filesz);
        if (p_memsz > p_filesz)
            memset(ptr + p_filesz, 0, p_memsz - p_filesz);
    }

    /* Allocate a stack */
    vmm_allocv(0xa0000000, 0x4000, VMM_USER | VMM_WRITE);

    __asm__ __volatile__ ("xchg %bx, %bx");
    /* Jump into the proc. */
    /* We're still in kernel mode, next step is to ring3 */
    PROCJMP(entry, 0xa0004000);
}

void proc_create(const char* s)
{
    mfs_fileinfo info;
    struct proc* p;

    mfs_read_info(&info, s);
    kprint("Loading ");
    kprint(s);
    kputchar('\n');
    p = &ptable.procs[ptable.proc_count++];
    p->flags = 0;
    p->pml4 = vmm_kfork();
    load_elf(p, &info);
}
