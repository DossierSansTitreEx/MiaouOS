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
disk_read:
    pushfq

    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    
    and rsi, 0x0FFFFFFF
    mov rax, rsi
    mov rcx, rdx

    mov rbx, rax         ; Save LBA in RBX

    mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11100000b     ; Set bit 6 in al for LBA mode
    out dx, al

    mov edx, 0x01F2      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al

    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al

    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al


    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al

    mov edx, 0x1F7       ; Command port
    mov al, 0x20         ; Read with retry.
    out dx, al

.retry:
    in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .retry            ; until the sector buffer is ready.

    mov rax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; read CL sectors
    mul bx
    mov rcx, rax         ; RCX is counter for INSW
    mov rdx, 0x1F0       ; Data port, in and out
    rep insw             ; in to [RDI]

    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq
    ret
