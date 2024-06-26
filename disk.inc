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

read_int:
mov [disk_packet.lba_l32], eax
mov DWORD [disk_packet.lba_h16], 0
mov [disk_packet.cnt], cl
mov [disk_packet.addr], di
mov [disk_packet.seg], ds
pusha
shr edi, 16
shl edi, 12
add [disk_packet.seg], edi
popa
lea si, [disk_packet]
mov ah, 0x42
int 0x13
ret


disk_packet:
	db 0x10
	db 0
	.cnt:
	dw 0
	.addr:
	dw 0
	.seg:
	dw 0
	.lba_l32:
	dd 0
	.lba_h16:
	dd 0