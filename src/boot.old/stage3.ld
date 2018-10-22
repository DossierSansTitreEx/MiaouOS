SECTIONS
{
	.text : {
		*(.start)
		*(.text)
		*(.data)
		*(.rodata)
        *(.bss)
	}
}
