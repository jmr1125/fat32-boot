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
hlt:	hlt
	jmp hlt
%include "basic.asm"
start_addr:	 dw 0
hello:		 db "hello world!", 0
