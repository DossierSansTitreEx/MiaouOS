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
    ; Clear screen
    mov ax, 0x0003
    int 0x10

    ; Hide cursor
    mov ah, 0x01
    mov ch, 0x3f
    int 0x10

    ; Try to find an active partition
    mov si, MBR.Partitions
.loop:
    call try_boot
    add si, 0x10
    cmp si, MBR.Partitions + 0x40
    jne .loop

    mov si, msg
    call print
    
.halt
    ; Halt forever
    hlt
    jmp .halt

try_boot:
    ret

print:
    pusha

    xor dx, dx
    mov bx, 0x0007
    mov cx, 1

.loop:
    mov ah, 0x02
    int 0x10
    inc dx

    mov al, [si]
    test al, al
    jz .loop_end
    mov ah, 0x09
    int 0x10
    inc si
    jmp .loop

.loop_end:
    popa
    ret

msg: db 'No Operating System found.', 0

times (0x1b4 - ($-$$)) db 0

MBR:
.ID:
    times 10 db 0
.Partitions:
    times 64 db 0
.Signature:
    dw 0xaa55
