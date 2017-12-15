[BITS 64]

SECTION .bss
ALIGNB 16

stack_bottom:
    resb 4096 * 4
stack_top:


SECTION .text

EXTERN kmain
GLOBAL __start

__start:
    ; Initialize the kernel stack
    lea rax, [REL stack_top]
    mov rsp, rax
    mov rbp, rax

    jmp kmain
.die:
    hlt
    jmp .die
