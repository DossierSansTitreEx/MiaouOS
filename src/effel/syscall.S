[BITS 64]

SECTION .bss

ALIGNB 16

sstack_start:
    resb 4096
sstack_end:

SECTION .data
EXTERN sys_table

ALIGN 8

user_rbp:
    dq 0
user_rsp:
    dq 0

SECTION .text

GLOBAL syscall_handler
GLOBAL syscall_set_handler

syscall_handler:
    mov [REL user_rbp], rbp
    mov [REL user_rsp], rsp
    lea rsp, [REL sstack_end]
    mov rbp, rsp

    ;push r11
    push rbx
    mov rbx, rax
    lea rax, [REL sys_table]
    shl rbx, 3
    add rax, rbx
    pop rbx

    push rcx
    call [rax]
    pop rcx

    pop r11
    mov rbp, [REL user_rbp]
    mov rsp, [REL user_rsp]
    or r11, 0x200
    o64 sysret

syscall_set_handler:
    push rcx

    mov rcx, 0xc0000080
    rdmsr
    or rax, 1
    wrmsr

    mov rdx, rdi
    mov rax, rdi
    shr rdx, 32
    and rax, 0xffffffff
    mov ecx, 0xc0000082
    wrmsr

    xor rax, rax
    mov rdx, 0x00180008
    mov rcx, 0xc0000081
    wrmsr

    xor rdx, rdx
    mov rax, 0x200
    mov rcx, 0xc0000084
    wrmsr

    pop rcx
    ret
