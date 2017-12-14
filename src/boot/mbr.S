[BITS 16]
[ORG 0x0600]

start:
    ; The ORG is not correct - we are actually at 0x7c00
    ; We need to relocate before we can do anything with absolute addressing
    cli

    ; Set the stack at 0x600
    mov ax, 0x600
    mov sp, ax
    mov bp, ax

    ; Set the segments to zero
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Relocate
    mov cx, 0x100
    mov si, 0x7c00
    mov di, 0x600
    rep movsw

    ; Jump to the correct address
    jmp 0:relocated

relocated:
    jmp $

times (0x1b4 - ($-$$)) db 0

MBR:
.ID:
    times 10 db 0
.Partitions:
    times 64 db 0
.Signature:
    dw 0xaa55
