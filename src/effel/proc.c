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

struct proc_table ptable;

static void procjmp(struct proc* p)
{
    __asm__ __volatile__ ("mov %0, %%cr3\r\n" :: "r" (p->pml4));
    __asm__ __volatile__ (
            "pushq   0(%%rax)\r\n"
            "pushq   8(%%rax)\r\n"
            "pushq  16(%%rax)\r\n"
            "pushq  24(%%rax)\r\n"
            "pushq  32(%%rax)\r\n"
            "movq   48(%%rax), %%rbx\r\n"
            "movq   56(%%rax), %%rcx\r\n"
            "movq   64(%%rax), %%rdx\r\n"
            "movq   72(%%rax), %%rsi\r\n"
            "movq   80(%%rax), %%rdi\r\n"
            "movq   88(%%rax), %%rbp\r\n"
            "movq   96(%%rax), %%r8\r\n"
            "movq  104(%%rax), %%r9\r\n"
            "movq  112(%%rax), %%r10\r\n"
            "movq  120(%%rax), %%r11\r\n"
            "movq  128(%%rax), %%r12\r\n"
            "movq  136(%%rax), %%r13\r\n"
            "movq  144(%%rax), %%r14\r\n"
            "movq  152(%%rax), %%r15\r\n"
            "movq   40(%%rax), %%rax\r\n"
            "iretq\r\n"
            :: "a" (p->cpu_state));
}

static uint64_t rflags()
{
    uint64_t v;

    __asm__ __volatile__ ("pushfq\r\n"
                          "popq %0\r\n" : "=r" (v));
    return v;
}

struct proc* proc_current()
{
    return ptable.procs + ptable.running;
}

void proc_init()
{
    ptable.procs = vmm_alloc(sizeof(struct proc) * 8092, 0);
    ptable.proc_count = 0;
    ptable.running = 0;

    /* Init the PIT */
    interrupt_register(0x20, &int_pit);
    irq_enable(0x00);
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
    /* Alloc the CPU state */
    struct cpu_state* state;
    state = vmm_alloc(0x1000, VMM_WRITE);
    p->cpu_state = state;
    memset(state, 0, sizeof(*state));

    /* Allocate a stack */
    vmm_allocv(0xa0000000, 0x4000, VMM_USER | VMM_WRITE);

    /* Setup the state */
    state->ss = 0x23;
    state->rsp = state->rbp = 0xa0004000;
    flags = rflags();
    flags &= 0xfffffffffffffffe;
    flags |= 0x0200;
    state->rflags = flags;
    state->cs = 0x2b;
    state->rip = entry;
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
