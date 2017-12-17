[BITS 64]

SECTION .text
EXTERN start

SECTION .start
GLOBAL long_mode

; RDI - Boot params
long_mode:
    cli

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

SECTION .text
GLOBAL disk_read
GLOBAL enter_kernel

enter_kernel:
    mov rax, rdi
    mov rdi, rsi
    jmp 0x08:rax

; RDI - destination
; RSI - LBA
; RDX - count
; RCX - DPTE
disk_read:
    pushfq

    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push r8
    push r9

    xor rax, rax
    mov r8, [rcx]
    add rcx, 4
    mov al, [rcx]
    mov r9, rax

    and rsi, 0x0FFFFFFF
    mov rax, rsi
    mov rcx, rdx

    mov rbx, rax

    mov rdx, r8
    add dx, 6
    shr eax, 24
    or rax, r9
    out dx, al

    mov rdx, r8
    add dx, 2
    mov al, cl
    out dx, al

    mov rdx, r8
    add dx, 3
    mov eax, ebx
    out dx, al

    mov rdx, r8
    add dx, 4
    mov eax, ebx
    shr eax, 8
    out dx, al

    mov rdx, r8
    add dx, 5
    mov eax, ebx
    shr eax, 16
    out dx, al

    mov rdx, r8
    add dx, 7
    mov al, 0x20
    out dx, al

.retry:
    in al, dx
    test al, 8
    jz .retry

    mov rax, 256
    xor bx, bx
    mov bl, cl
    mul bx
    mov rcx, rax

    mov rdx, r8
    rep insw

    pop r9
    pop r8
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq
    ret
