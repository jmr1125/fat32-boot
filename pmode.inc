
setup_pmode:	
	cli  ; Disable interrupts, we want to be alone

	xor ax, ax
	mov ds, ax; Set DS-register to 0 - used by lgdt

	lgdt [gdt_desc]; Load the GDT descriptor

	mov eax, cr0; Copy the contents of CR0 into EAX
	or  eax, 1; Set bit 0
	mov cr0, eax; Copy the contents of EAX into CR0
	ret