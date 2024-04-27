puthex:
	push ebx
	mov  di, HexBuf
	mov  cx, 8
	mov  al, '0'
	rep  stosb
	mov  BYTE [HexBuf+8], 0
	lea  di, [HexBuf+7]
	pop  ebx

.loop:
	mov eax, ebx; put bx in hex  low ---- high
	and eax, 0fh
	cmp eax, 9
	jle .num
	jg  .alpha

.num:
	add eax, '0'
	jmp .print

.alpha:
	add eax, 'A'-10

.print:
	mov  [es:bx], al
	dec  bx
	shr  ebx, 4
	or   ebx, ebx
	jnz  .loop
	mov  bx, HexBuf
	call puts
	ret
HexBuf times 8 db 0

putc:
	mov ah, 0x0e; put char in: al
	int 10h
	ret

puts:
	mov ah, 0x0e; put string in: bx

.loop:
	mov al, [ds:ebx]
	or  al, al
	jz  .done
	inc ebx
	int 10h
	jnz .loop

.done:
	ret
