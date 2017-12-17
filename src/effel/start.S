[BITS 64]

SECTION .bss
ALIGNB 16
    resb 4096 * 4
kernel_stack:

SECTION .text

EXTERN kmain
GLOBAL __start

__start:
    ; Initialize the kernel stack
    lea rax, [REL kernel_stack]
    mov rsp, rax
    mov rbp, rax

    jmp kmain
.die:
    hlt
    jmp .die
