[BITS 64]

SECTION .text
EXTERN start

SECTION .start
GLOBAL long_mode

long_mode:
    mov ax, 0x10
    mov fs, ax
    mov gs, ax

    ; Reset the stack
    mov rax, 0x7c00
    mov rsp, rax
    mov rbp, rax

    ; Enable SSE
    mov rax, cr0
    and ax, 0xfffb
    or ax, 0x0002
    mov cr0, rax
    mov rax, cr4
    or ax, 0x0600
    mov cr4, rax

    jmp start
