	global enter_v86
	global exit_v86
	section .text
	%include "config.inc"
enter_v86:
	push ebp
	mov ebp, esp
	push es
	push ds
	push fs
	push gs
	pusha
	pushf
	push cs
	push .retfrom_v86
	mov [saved_ss], ss
	mov [saved_esp], esp

	xor eax, eax
	push eax
	push eax
	push eax
	push eax
	push dword [ebp + 16]
	push dword [ebp + 20]
	or dword [ebp + 24], (1<<17)|(1<<1)
	push dword [ebp + 24]
	push dword [ebp + 8]
	push dword [ebp + 12]

	mov ebx, eax
	mov ecx, eax
	mov edx, eax
	mov esi, eax
	mov edi, eax
	mov ebp, eax

	lidt [realm_idt]
	lgdt [realm_gdt]
	iret

	.retfrom_v86:
	popa
	pop gs
	pop fs
	pop ds
	pop es

	leave
	ret

exit_v86:
	mov ss, [saved_ss]
	mov esp, [saved_esp]
	iret




	section .bss
	saved_ss resd 1
	saved_esp resd 1
aaa:	
