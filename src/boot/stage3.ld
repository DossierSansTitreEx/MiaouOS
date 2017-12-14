SECTIONS
{
	.text BLOCK(1) : ALIGN(1)
	{
		*(.start)
		*(.text)
		*(.data)
		*(.rodata)
	}
}
