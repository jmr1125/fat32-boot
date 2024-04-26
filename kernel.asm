[map all kernel.map]

begin:
	bits 16
	call get_addr

get_addr:
	pop eax
	sub eax, 3
	sub eax, 0x7c00
	mov [entry_point], ax
	mov bx, [entry_point]
	;;  copy self to 0x0000:0x7c00
	mov ax, 0
	mov es, ax
	mov ds, ax
	mov si, begin
	add si, bx
	mov di, 0x7c00
	mov ecx, end - begin
	rep movsb
	jmp 0x0:0x7c00+start-begin
entry_point: dw 0

start:
	[BITS 16]; We need 16-bit intructions for Real mode
	[ORG  0x7C00]; The BIOS loads the boot sector into memory location 0x7C00

	cli ; Disable interrupts, we want to be alone

	xor ax, ax
	mov ds, ax; Set DS-register to 0 - used by lgdt

	lgdt [gdt_desc]; Load the GDT descriptor

	mov eax, cr0; Copy the contents of CR0 into EAX
	or  eax, 1; Set bit 0
	mov cr0, eax; Copy the contents of EAX into CR0

	jmp 08h:clear_pipe; Jump to code segment, offset clear_pipe

	[BITS 32]; We now need 32-bit instructions

clear_pipe:
	mov ax, 10h; Save data segment identifyer
	mov ds, ax; Move a valid data segment into the data segment register
	mov ss, ax; Move a valid data segment into the stack segment register
	mov es, ax; Move a valid data segment into the extra segment register
	mov fs, ax; Move a valid data segment into the fs register
	mov gs, ax; Move a valid data segment into the gs register
	mov esp, 090000h; Move the stack pointer to 090000h

	;; clear the screen
	mov edi, 0xb8000
	mov ecx, 80*25*2
	mov al,0
	rep stosb

	mov esi, protected_mode_string
	mov edi, 0xb8000
	mov ecx, protected_mode_string.end - protected_mode_string
	rep movsb

hang:
	jmp hang; Loop, self-jump

protected_mode_string:	
	db '[',0x1b,'P',0x1b,'M',0x1b,']',0x1b,' ',0x1b,'P',0x1b,'r',0x1b,'o',0x1b,'t',0x1b,'e',0x1b,'c',0x1b,'t',0x1b,'e',0x1b,'d',0x1b,' ',0x1b,'M',0x1b,'o',0x1b,'d',0x1b,'e',0x1b,' ',0x1b,'n',0x1b,'o',0x1b,'w',0x1b
	.end: db 0

gdt:
	; Address for the GDT

	gdt_null:               ; Null Segment
	dd 0
	dd 0

	gdt_code:               ; Code segment, read/execute, nonconforming
	dw 0FFFFh
	dw 0
	db 0
	db 10011010b
	db 11001111b
	db 0

	gdt_data:               ; Data segment, read/write, expand down
	dw 0FFFFh
	dw 0
	db 0
	db 10010010b
	db 11001111b
	db 0

	gdt_end:                ; Used to calculate the size of the GDT

	gdt_desc:                       ; The GDT descriptor
	dw gdt_end - gdt - 1; Limit (size)
	dd gdt; Address of the GDT

	;; times 510-($-start) db 0; Fill up the file with zeros

	;; dw 0AA55h; Boot sector identifyer

end:
