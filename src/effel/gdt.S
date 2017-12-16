[BITS 64]

SECTION .text
GLOBAL gdt_switch

gdt_switch:
    lgdt [rdi]

    push rsi
    lea rax, [REL .target]
    push rax
    retf

.target:
    mov fs, rdx
    mov gs, rdx
    ret
