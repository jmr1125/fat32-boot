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
.loop:
call read_int_1sector
add edi, 512
inc eax
dec cl
jnz .loop
ret

read_int_1sector:
pusha
mov [disk_packet.lba_l32], eax
mov DWORD [disk_packet.lba_h16], 0
mov WORD [disk_packet.cnt], 1
mov WORD [disk_packet.addr], tmp_buffer
mov WORD [disk_packet.seg], 0
lea si, [disk_packet]
mov ah, 0x42
int 0x13
mov esi, tmp_buffer
mov ecx, 512
.loop:
mov al, [esi]
mov [edi], al
inc esi
inc edi
dec ecx
jnz .loop
popa
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