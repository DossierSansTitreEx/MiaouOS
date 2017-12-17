[BITS 64]

SECTION .bss
EXTERN kernel_stack

SECTION .text
EXTERN proc_scheduler_main
GLOBAL int_pit
GLOBAL int_real_time_clock

int_pit:
    ; Prevent reentrant INT
    cli

    ; ACK the IRQ
    push rax
    mov al, 0x20
    out 0x20, al
    pop rax

    ; Make a continuation - push the local state
    ; on the kernel stack
    push rbp
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Load a kernel stack
    lea rsp, [REL kernel_stack]
    mov rbp, rsp

    jmp proc_scheduler_main

int_real_time_clock:
    cli
    xchg bx, bx
    iretq
