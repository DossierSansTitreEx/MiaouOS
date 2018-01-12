[BITS 64]

SECTION .text
EXTERN proc_scheduler_main
EXTERN proc_current

GLOBAL int_pit
GLOBAL int_real_time_clock

int_pit:
    ; Prevent reentrant INT
    cli

    ; Save RAX, we will need it later
    push rax

    ; ACK the IRQ
    mov al, 0x20
    out 0x20, al

    ; Fetch the current proc
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    call proc_current

    ; Get the CPU state ptr
    mov rax, [rax]

    ; Save the CPU state
    ; iret state
    ; SS
    mov rcx, [rsp + 8 * 13]
    mov [rax + 8 * 0], rcx

    ; RSP
    mov rcx, [rsp + 8 * 12]
    mov [rax + 8 * 1], rcx

    ; RFLAGS
    mov rcx, [rsp + 8 * 11]
    mov [rax + 8 * 2], rcx

    ; CS
    mov rcx, [rsp + 8 * 10]
    mov [rax + 8 * 3], rcx

    ; RIP
    mov rcx, [rsp + 8 * 9]
    mov [rax + 8 * 4], rcx

    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx

    ; generic state
    mov [rax + 8 *  6], rbx
    mov [rax + 8 *  7], rcx
    mov [rax + 8 *  8], rdx
    mov [rax + 8 *  9], rsi
    mov [rax + 8 * 10], rdi
    mov [rax + 8 * 11], rbp
    mov [rax + 8 * 12], r8
    mov [rax + 8 * 13], r9
    mov [rax + 8 * 14], r10
    mov [rax + 8 * 15], r11
    mov [rax + 8 * 16], r12
    mov [rax + 8 * 17], r13
    mov [rax + 8 * 18], r14
    mov [rax + 8 * 19], r15

    ; save RAX itself
    pop rbx
    mov [rax + 8 * 5], rbx

    ; Clean up the stack
    add rsp, 8 * 5

    jmp proc_scheduler_main

int_real_time_clock:
    cli
    iretq
