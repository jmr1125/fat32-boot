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
