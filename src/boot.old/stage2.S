[BITS 16]
[ORG 0x7c00]

PML4        equ 0x1000
PDP         equ 0x2000
MEMORY_INFO equ 0x3000

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
.memory_size:
    dq 0
.memory_ptr:
    dq MEMORY_INFO
.mbr:
    dq 0
.dpte:
    times 16 db 0
.drive:
    db 0

ALIGN 16
DRIVE_INFO:
    dw 0x1e
    times 0x1c db 0

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

print_fatal:
    call print
    jmp die

die:
    hlt
    jmp die

err_no_edd:
    mov si, .msg
    call print_fatal
.msg:
    db 'EDD call failed', 0

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

    ; Get the drive info
    mov dl, [BOOT_INFO.drive]
    mov si, DRIVE_INFO
    mov ah, 0x48
    int 0x13
    jc err_no_edd
    mov ax, [DRIVE_INFO + 0x1a]
    mov si, ax
    mov ax, [DRIVE_INFO + 0x1a + 2]
    mov ds, ax
    mov di, BOOT_INFO.dpte
    mov cx, 8
    rep movsw

    xor ax, ax
    mov ds, ax

    ; Load the memory map
    call detect_memory

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

    mov edi, BOOT_INFO

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

detect_memory:
    xor ax, ax
    push ax

    mov esi, 0x00000003
    mov di, MEMORY_INFO
    xor ebx, ebx
    mov edx, 0x534D4150
    mov eax, 0xe820
    mov ecx, 24

.loop:
    ; Get a memory entry
    int 0x15

    ; Check if this was the last one
    jc .end
    test ebx, ebx
    jz .end

    ; Check if the entry is an acpi-extended one
    cmp cl, 24
    je .acpi
    mov [di + 20], esi

.acpi:
    add di, 24
    pop ax
    inc ax
    push ax
    mov eax, 0xe820
    mov ecx, 24
    jmp .loop

.end:
    pop ax
    mov [BOOT_INFO.memory_size], ax
    ret

long_mode:
