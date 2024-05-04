	;; eax - lba
	;; cl -count
	;; di - buffer

read:
and eax, 0x0fffffff
	mov ebx, eax

	mov edx, 0x1f6
	shr eax, 24
	or  al, 11100000b
	out dx, al

	mov edx, 0x1f2
	mov al, cl
	out dx, al

	mov edx, 0x1f3
	mov eax, ebx
	out dx, al

	mov edx, 0x1f4
	mov eax, ebx
	shr eax, 8
	out dx, al

	mov edx, 0x1f5
	mov eax, ebx
	shr eax, 16
	out dx, al

	mov edx, 0x1f7
	mov al, 0x20
	out dx, al

.wait:
	in   al, dx
	test al, 8
	jz   .wait

	mov  ebx, 256
	imul ebx, ecx
	mov  ecx, ebx
	mov  edx, 0x1f0
	rep  insw
	ret
