	org 0x7c00
	%include "config.inc"
	[bits 16]
enter:
	mov eax,0x10
	mov ds,eax
	mov es,eax
	mov gs,eax
	mov fs,eax
	mov gs,eax
	mov ss,eax
	mov eax,cr0
	and eax,0xfffffffe
	mov cr0,eax
	jmp 0x0:start
	[bits 16]
start:	
	;; xchg bx,bx
	pusha			; esp=0xfff0 ebp=0xfff8
	mov ax,0
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	lidt [idt16_ptr]
	;; xchg bx,bx
	mov esi,REGS_OFF+regs.interrupt_no
	mov al,[esi]
	mov byte [int_no],al
	mov esi,REGS_OFF+regs.eax
	mov eax,[esi]
	mov esi,REGS_OFF+regs.ebx
	mov ebx,[esi]
	mov esi,REGS_OFF+regs.ecx
	mov ecx,[esi]
	mov esi,REGS_OFF+regs.edx
	mov edx,[esi]
	mov esi,REGS_OFF+regs.edi
	mov edi,[esi]
	mov esi,REGS_OFF+regs.es
	mov es,[esi]
	mov esi,REGS_OFF+regs.ds
	mov ds,[esi]
	mov esi,REGS_OFF+regs.esi
	mov esi,[esi]
	;; xchg bx,bx
	db 0xcd
int_no:	db 0
	push esi
	mov esi,REGS_OFF+regs.edi
	mov [esi],edi
	pop esi
	mov edi,REGS_OFF+regs.esi
	mov [edi],esi
	mov esi,REGS_OFF+regs.edx
	mov [esi],edx
	mov esi,REGS_OFF+regs.ecx
	mov [esi],ecx
	mov esi,REGS_OFF+regs.ebx
	mov [esi],ebx
	mov esi,REGS_OFF+regs.eax
	mov [esi],eax
	mov esi,REGS_OFF+regs.es
	mov [esi],es
	mov esi,REGS_OFF+regs.ds
	mov [esi],ds
	popa			;ffe0 fff8
hang:	nop			;fff0 fff8
	nop
	cli
	lgdt [gdt_desc]
	mov eax,cr0
	or eax,1
	mov cr0,eax
	jmp 0x8:t
	[bits 32]
t:	ret
			
	idt16_ptr:                                 ; IDT table pointer for 16bit access
		dw 0x03FF                              ; table limit (size)
		dd 0x00000000                          ; table base address
		
	struc regs
	.eax:	resd 1
	.ebx:	resd 1
	.ecx:	resd 1
	.edx:	resd 1
	.esi:	resd 1
	.edi:	resd 1
	.ebp:	resd 1
	.esp:	resd 1
	.eflag:	resd 1
	.es:	resd 1
	.ds:	resd 1
	.interrupt_no:	resd 1
	endstruc


	%include "pmode.inc"
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
