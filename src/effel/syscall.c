#include <syscall.h>
#include <stdint.h>
#include <stddef.h>
#include <screen.h>

void* sys_table[512];

void syscall_handler();
void syscall_set_handler(void* handler);

void sys_write(int fd, const void* mem, size_t size)
{
    (void)fd;
    kprint_raw(mem, size);
}

void syscall_init()
{
    sys_table[0x01] = sys_write;
    syscall_set_handler(&syscall_handler);
}
