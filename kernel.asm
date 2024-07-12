	bits 32
	global _start
	extern k_main
	section .text
_start:
	mov esp,0x10000
	mov ebp,0x10000
	mov ecx, 25*80*2
	mov edi,0xb8000
	mov al,0x00
	rep stosb
	call k_main
hlt:
	.hlt:hlt
	jmp hlt.hlt
	global to_real
to_real:
	cli
	lgdt [gdt16_ptr]
	jmp 0x8:0x7c00
	ret
	global gdt16_base
	global gdt16_ptr
	global idt16_ptr
	section .data

	gdt16_base:                                ; GDT descriptor table
		.null:                                 ; 0x00 - null segment descriptor
			dd 0x00000000                      ; must be left zero'd
			dd 0x00000000                      ; must be left zero'd
	
		.code16:                               ; 0x03 - 16bit code segment descriptor 0x000FFFFF
			dw 0xFFFF                          ; limit  0:15
			dw 0x0000                          ; base   0:15
			db 0x00                            ; base  16:23
			db 0x9A                            ; present, iopl/0, code, execute/read
			db 0x0F                            ; 1Byte granularity, 16bit selector; limit 16:19
			db 0x00                            ; base  24:31
			
		.data16:                               ; 0x04 - 16bit data segment descriptor 0x000FFFFF
			dw 0xFFFF                          ; limit  0:15
			dw 0x0000                          ; base   0:15
			db 0x00                            ; base  16:23
			db 0x92                            ; present, iopl/0, data, read/write
			db 0x0F                            ; 1Byte granularity, 16bit selector; limit 16:19
			db 0x00                            ; base  24:31
			
	gdt16_ptr:                                 ; GDT table pointer for 16bit access
		dw gdt16_ptr - gdt16_base - 1          ; table limit (size)
		dd gdt16_base                          ; table base address
		
	int32_end:                                 ; end marker (so we can copy the code)
	
	
