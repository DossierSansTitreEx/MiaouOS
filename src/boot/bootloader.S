[ORG 0x7C00]
[BITS 16]

DISK_BUFFER         equ 0x20000
DISK_BUFFER2        equ 0x21000
DISK_BUFFER3        equ 0x22000
KERNEL_BUFFER       equ 0x100000

jmp start

;
; DATA
;

ALIGN 16
BootInfo:
.MemorySize:
    dq 0
.memory_ptr:
    dq 0
.MbrPtr:
    dq 0
.dpte:
    times 16 db 0
.DriveNumber:
    db 0

ALIGN 0x8
DriveRequest:
.StructSize:
    db  0x10
.Zero:
    db  0x00
.SectorCount:
    dw  0x08
.Offset:
    dw  0x0000
.Segment:
    dw  0x0000
.LBA:
    dd  0x00000000
    dd  0x00000000

ALIGN 0x8
GDT32:
.Null:
    dw 0x0000
    dw 0x0000
    db 0x00
    db 0x00
    db 0x00
    db 0x00
.Code:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 0x98
    db 0xcf
    db 0x00
.Data:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 0x92
    db 0xcf
    db 0x00
.Descriptor:
    dw (.Descriptor - GDT32 - 1)
    dd GDT32
;
; CODE
;

start:
    cli

    ; Save the drive number
    mov [BootInfo.DriveNumber], dl

    ; Save the MBR entry Pointer
    mov [BootInfo.MbrPtr], si

    ; Set the stack at 0x7c00
    mov ax, 0x7c00
    mov sp, ax
    mov bp, ax

    ; Set the segments to zero
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Force segment zero by long jump
    jmp 0x00:enable_a20

enable_a20:
    ; FIXME: Use better code to enable the A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    jmp enable_unreal

enable_unreal:
    ; Load a 32 bit GDT
    lgdt [GDT32.Descriptor]

    ; Enable protected mode
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ; Failsafe for older CPUs
    jmp $+2

    ; Load protected segments
    mov ax, 0x10
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Disable protected mode
    mov eax, cr0
    and al, 0xfe
    mov cr0, eax

    ; Reset segments
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    jmp unreal_mode

; void drive_read_block(void* dst, uint32_t block)
;
; Read a block (4k) from the boot drive
; into memory.
;
; edi = dst
; esi = block
drive_read_block:
    mov al, 0x0f
    xor ax, di
    mov [DriveRequest.Offset], di
    shr edi, 4
    mov [DriveRequest.Segment], di
    mov edi, [BootInfo.MbrPtr]
    mov edi, [edi+8]
    shl esi, 3
    add esi, edi
    mov [DriveRequest.LBA], esi
    mov ah, 0x42
    mov dl, [BootInfo.DriveNumber]
    mov si, DriveRequest
    int 0x13
    ret

; Look up a file in a directory
;
; edi = directory
; esi = filename
drive_lookup_file:
    push ebp
    mov ebp, esp

    push esi
    mov esi, DISK_BUFFER
    call drive_read_block
    pop esi

    
 
    pop ebp
    ret

unreal_mode:
    ; Reset segments
    ; The segment cache registers
    ; will stay in 32 bit mode
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Read the drive header into a buffer
    mov         edi, DISK_BUFFER
    push        edi
    mov         esi, 10
    call        drive_read_block

    ; Get the root inode
    pop         edi
    mov DWORD   esi, [edi+0x18]
    jmp $
