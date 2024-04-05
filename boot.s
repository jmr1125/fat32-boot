times 90 db 0
	bits 16
	jmp main
main:	mov ax,0
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7c00
	mov ah,0xe
	mov al,'h'
	int 10h
a:	hlt
	jmp a
	times 510-($-$$) db 0
	dw 0aa55h
