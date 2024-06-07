ENTRY(_start)
SECTIONS {
	. = 0x0;
	.lower_space : {
		*(.lower_space)
	}
	. = 0x10000;
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
