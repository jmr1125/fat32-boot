	%include "config.inc"
	bits 16
	sti
	org 0x7c00
	mov eax,eax
	mov ah,0x0e
	mov bh,0x00
	mov al,'a'
	int 0xc
