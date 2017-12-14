ENTRY(kmain)
SECTIONS
{
    _KERNEL_BASE_ = 0xffffffff80000000;

    .text(_KERNEL_BASE_) : ALIGN(0x1000) {
        *(.text)
    }

    .data : ALIGN(0x1000) {
        *(.data)
    }

    .rodata : ALIGN(0x1000) {
        *(.rodata)
    }

    .bss : ALIGN(0x1000) {
        *(.bss)
    }

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }

    _KERNEL_END_ = .;
}
