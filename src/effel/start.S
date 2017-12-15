[BITS 64]

SECTION .text

EXTERN kmain
GLOBAL __start

__start:
    jmp kmain
.die:
    hlt
    jmp .die
