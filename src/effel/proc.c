#include <proc.h>
#include <vmm.h>
#include <screen.h>
#include <mfs.h>
#include <string.h>
#include <util.h>
#include <tss.h>
#include <interrupt.h>
#include <x86.h>

void int_pit();
void int_real_time_clock();

#define PROCJMP(kstack)    __asm__ __volatile__ ("movq %0, %%rsp\r\n"             \
                                                 "fxrstor (%%rsp)\r\n"            \
                                                 "addq $512, %%rsp\r\n"           \
                                                 "popq %%r15\r\n"                 \
                                                 "popq %%r14\r\n"                 \
                                                 "popq %%r13\r\n"                 \
                                                 "popq %%r12\r\n"                 \
                                                 "popq %%r11\r\n"                 \
                                                 "popq %%r10\r\n"                 \
                                                 "popq %%r9\r\n"                  \
                                                 "popq %%r8\r\n"                  \
                                                 "popq %%rsi\r\n"                 \
                                                 "popq %%rdi\r\n"                 \
                                                 "popq %%rdx\r\n"                 \
                                                 "popq %%rcx\r\n"                 \
                                                 "popq %%rbx\r\n"                 \
                                                 "popq %%rax\r\n"                 \
                                                 "popq %%rbp\r\n"                 \
                                                 "iretq\r\n" :: "r"(kstack));

struct proc_table ptable;

static void procjmp(struct proc* p)
{
    void* kstack;
    void* tss_kstack;

    __asm__ __volatile__ ("mov %0, %%cr3\r\n" :: "r" (p->pml4));
    kstack = p->kstack;
    tss_kstack = (char*)kstack + 20 * 0x08;
    tss.rsp[0] = (uint64_t)tss_kstack;
    PROCJMP(kstack);
}

static uint64_t rflags()
{
    uint64_t v;

    __asm__ __volatile__ ("pushfq\r\n"
                          "popq %0\r\n" : "=r" (v));
    return v;
}

void proc_init()
{
    ptable.procs = vmm_alloc(sizeof(struct proc) * 8092, 0);
    ptable.proc_count = 0;
    ptable.running = 0;

    /* Init the PIT */
    interrupt_register(0x20, &int_pit);
    irq_enable(0x00);

#if 0
    /* Init the RTC */
    interrupt_register(0x20, &int_pit);
    interrupt_register(0x28, &int_real_time_clock);
    irq_enable(0x00);
    irq_enable(0x02);
    irq_enable(0x08);

    outb(0x70, 0x8a);
    outb(0x71, 0x20);
    outb(0x70, 0x8b);
    rtc_mask = inb(0x71);
    outb(0x70, 0x8b);
    outb(0x71, rtc_mask | 0x40);
    outb(0x70, 0x0c);
    inb(0x71);
    __asm__ __volatile__ ("sti");
    for (;;) {}
#endif
}

static void proc_push_sse(struct proc* p)
{
    char* ptr;

    ptr = p->kstack;
    ptr -= 512;
    memset(ptr, 0, 512);
    p->kstack = ptr;
}

static void proc_push64(struct proc* p, uint64_t value)
{
    uint64_t* ptr;

    ptr = (uint64_t*)p->kstack;
    ptr--;
    *ptr = value;
    p->kstack = ptr;
}

static void load_elf(struct proc* p, const mfs_fileinfo* info)
{
    char* ptr;

    char elf_header[64];
    char section_header[256];

    uint64_t entry;
    uint64_t section_offset;
    uint64_t flags;
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
    /* Alloc a kernel stack */
    p->kstack_base = vmm_alloc(0x1000, VMM_WRITE);
    p->kstack = (char*)p->kstack_base + 0x1000;

    /* Allocate a stack */
    vmm_allocv(0xa0000000, 0x4000, VMM_USER | VMM_WRITE);

    /* Setup iret */
    proc_push64(p, 0x23);
    proc_push64(p, 0xa0004000);
    flags = rflags();
    flags &= 0xfffffffffffffffe;
    flags |= 0x0200;
    proc_push64(p, flags);
    proc_push64(p, 0x2b);
    proc_push64(p, entry);

    /* Setup saved registers */
    proc_push64(p, 0xa0004000); /* rbp last */
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);
    proc_push64(p, 0x0);

    proc_push_sse(p);
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

void proc_schedule()
{
    ptable.running = 0;
    procjmp(ptable.procs);
}

void proc_scheduler_main()
{
    ptable.running++;
    if (ptable.running >= ptable.proc_count)
        ptable.running = 0;
    procjmp(ptable.procs + ptable.running);
}
