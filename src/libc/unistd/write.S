[BITS 64]

SECTION .text
GLOBAL write

write:
    ; Code for write
    mov rax, 0x01
    syscall
    xor rax, rax
    ret
