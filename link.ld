ENTRY(_start)
SECTIONS {
	. = 0x20000;
	.text : {
		*(.text)
		}
	.rodata : {
		*(.rodata)
		}
	.data : {
		*(.data)
		}
	.bss : {
		*(.bss)
		}
}
