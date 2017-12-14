[BITS 16]
[ORG 0x7c00]

PML4 equ 0x1000
PDP  equ 0x2000

start:
    cli
    jmp 0x0:stack_setup

ALIGN 16
IDT:
.Descriptor:
    dw 0
    dq IDT


ALIGN 16
GDT:
.Null:
    dq 0
.Code:
    dw 0x0000
    dw 0x0000
    db 0x00
    db 10011000b
    db 00100000b
    db 0x0
.Data:
    dw 0x0000
    dw 0x0000
    db 0x00
    db 10010010b
    db 00100000b
    db 0x0
.Descriptor:
    dw (.Descriptor - GDT - 1)
    dq GDT

; This structure will be passed to the kernel
ALIGN 16
BOOT_INFO:
.mbr
    dq 0
.drive
    db 0
ALIGN 16, db 0

stack_setup:
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ax, 0x7c00
    mov sp, ax
    mov bp, ax

    mov [BOOT_INFO.drive], dl
    mov [BOOT_INFO.mbr], si

    ; Enable the A20 gate to prepare for long mode
    call fast_a20

    ; Zero the page tables
    mov di, PML4
    call zero_table
    mov di, PDP
    call zero_table

    ; Set the first PML4 entry
    mov di, PML4
    mov ax, PDP
    or ax, 1
    mov [di], ax

    xor cx, cx
.loop:
    call identity_map
    inc cx
    cmp cx, 4
    jne .loop

    ; Disable all IRQs
    call disable_irq

    ; Load a dummy IDT
    lidt [IDT.Descriptor]

    ; Set PAE and PGE
    mov eax, 10100000b
    mov cr4, eax

    ; Point CR3 to enable page trie traversal
    mov eax, PML4
    mov cr3, eax

    ; Enable paging
    mov ecx, 0xc0000080
    rdmsr
    or eax, 0x00000100
    wrmsr

    ; Switch to long mode
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax

    ; Load the GDT
    lgdt [GDT.Descriptor]

    ; Jump into long mode
    jmp 0x08:long_mode

fast_a20:
    in al, 0x92
    test al, 2
    jnz .end
    or al, 2
    and al, 0xFE
    out 0x92, al
.end:
    ret

zero_table
    xor ax, ax
    mov cx, 0x800
    rep stosw
    ret

; CX - The identity index
identity_map:
    ; Store the PDP slot address in di
    mov di, cx
    shl di, 3
    add di, PDP

    ; Set the flags P, R & S
    mov si, 0x83
    mov [di], si

    ; Set the address
    mov ax, 0x4000
    mul cx
    add di, 2
    mov [di], ax
    ret

disable_irq:
    mov al, 0xff
    out 0xa1, al
    out 0x21, al
    nop
    nop
    ret

die:
    hlt
    jmp die

[BITS 64]

long_mode:
    jmp $
