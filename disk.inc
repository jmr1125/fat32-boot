	;;  bl - sector number
	;;  bh - head
	;;  ebx>>16 - cylinder
	;;  ebx calculated by lba2chs
	;;  ch - sector to read
	;;  edi - buffer
	;;; disk read in CHS format

read:
	mov dx, 0x1f6
	mov al, bh
	and al, 0x0f
	or  al, 0b10100000
	out dx, al

	mov dx, 0x1f2
	mov al, ch
	out dx, al

	mov dx, 0x1f3
	mov al, bl
	out dx, al

	mov dx, 0x1f4
	mov eax, ebx
	shr eax, 16
	out dx, al

	mov dx, 0x1f5
	mov eax, ebx
	shr eax, 24
	out dx, al

	mov dx, 0x1f7
	mov al, 0x20
	out dx, al

	;;   wait 400ns
	call wait100ns
	call wait100ns
	call wait100ns
	call wait100ns

	;; wait for disk not busy

.wait:
	in   al, dx
	test al, 0x08
	jz   .wait

	mov eax, 512/2
	xor ebx, ebx
	mov bl, ch
	mul bx
	mov ecx, eax
	mov dx, 0x1f0
	rep insw
	ret

wait100ns:
	;it causes the extra 0x0 when reading so i delete it
	;;  push ax
	;;  push dx
	;;  mov dx, 0x3f6
	;;  in al, dx
	;;  pop dx
	;;  pop ax
	ret
	;; ebx - lba

lba2chs:
	xor  edx, edx
	mov  eax, ebx
	div  WORD [bootsect.BPB_SecPerTrk]
	;;   lba / sectors per track = ax = Temp
	;;   lba % sectors per track = dx
	inc  dx
	push dx; sector
	xor  dx, dx
	div  WORD [bootsect.BPB_NumHeads]
	;;   Temp / heads = ax = Cylinder
	;;   Temp % heads = dx = Head
	push dx; head
	push ax; cylinder
	;;   set up argument for read
	pop  ax; cylinder
	shl  eax, 16
	mov  ebx, eax
	pop  ax; head
	shl  ax, 8
	or   bx, ax
	pop  ax; sector
	or   bx, ax
	ret
