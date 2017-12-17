[BITS 64]

SECTION .text
GLOBAL ata_read

; RDI - destination
; RSI - LBA
; RDX - count
; RCX - DPTE
ata_read:
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
