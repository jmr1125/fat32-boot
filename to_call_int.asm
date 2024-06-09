org 0x7c00
	[bits 32]
	mov [return_addr], eax
	cli
	lgdt [realgdtr]
	mov eax,cr0
	and eax,0x7ffffffe
	mov cr0,eax
	jmp 0x18:setupRmode-0x180
	[bits 16]
setupRmode:
	mov ax, 0x0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	lidt [idt_real]
	xchg bx,bx
	mov ah,0x0e
	mov al,'H'
	mov bl,0
	int 10h
	mov eax, [return_addr]
	call setup_pmode
	jmp 0x08:.1
	.1:
	jmp eax
hang:	cli
	hlt
	%include "pmode.inc"
	section .data
idt_real:
	dw 0x3ff
	dd 0
realgdt:
    dq 0                    ; 空描述符
    dq 0x00CF9A000000FFFF   ; 代码段描述符 (32 位代码段)
    dq 0x00CF92000000FFFF   ; 数据段描述符 (32 位数据段)
    dq 0x00009A000000FFFF   ; 代码段描述符 (16 位代码段)
    dq 0x000092000000FFFF   ; 数据段描述符 (16 位数据段)

realgdtr:
    dw realgdt_end - realgdt - 1    ; GDT 长度
    dd realgdt                  ; GDT 基地址
realgdt_end:
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
return_addr:	dd 0
