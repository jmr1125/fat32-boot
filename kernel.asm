	bits 16
	;; eax = ip
	call .getip
	.getip:
	pop eax
	sub eax, 3
	mov [start_addr], ax
	cli
	mov ax, [start_addr]
	shr ax, 4
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov ax, [start_addr]
	mov sp, ax
	mov bp, ax
	sti

	mov bx, hello
	call puts

	
	;; https://wiki.osdev.org/A20_Line
	mov     ax,2401h                ;--- A20-Gate Activate ---
int     15h
jb      .a20_failed              ;couldn't activate the gate
cmp     ah,0
	jnz     .a20_failed              ;couldn't activate the gate



	cli
	lgdt [gdt_ptr]

	mov eax,cr0
	or eax,1
	mov cr0,eax
	jmp 08h:protect

	.a20_failed:
	mov     bx,err_coudnt_activate_a20
	call    puts
	jmp hlt

	%include "basic.asm"
	bits 32
protect:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, 0x90000

	mov BYTE [ds:0xb8000], 'P'
	mov BYTE [ds:0xb8001], 0x1b
hlt:	hlt
	jmp hlt
start_addr:	 dw 0
section .rodata
hello:		 db "hello world from kernel!", 0
err_coudnt_activate_a20: db "couldn't activate a20", 0
gdt_data:
	dd 0,0
	.code:
	dw 0xffff
	dw 0
	db 0
	db 10011010b
	db 11001111b
	db 0
	.data:
	dw 0xffff
	dw 0
	db 0
	db 10010010b
	db 11001111b
	db 0
	.end:

gdt_ptr:
	dw gdt_data.end - gdt_data -1
	dd gdt_data
