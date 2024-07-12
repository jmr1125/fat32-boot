
	%ifdef  DEBUG
	%define trap xchg bx, bx
	%else
	%define trap
	%endif


	;; cx - x
	;; bx - y
	;; ebx - offset

calculate_offset:
	shl  ebx,16
	shr  ebx,16
	shl  ecx,16
	shr  ecx,16
	shl  esi,16
	shr  esi,16
	imul ecx, ecx, 80*2
	shl  ebx, 1
	add  ebx, ecx
	add  ebx, 0xb8000
	ret
	;; ebx - offset
	;; al - char
	;; ah - attr

_putc:
	mov [ebx], al
	mov [1+ebx], ah
	ret
	;; esi - string
	;; ebx - offset
	;; ah - attr

_puts:
	mov al, [esi]
	or  al, al
	jz  .done
	mov [ebx], al
	mov [ebx+1], ah
	add bx, 2
	inc esi
	jmp _puts

.done:
	ret
	;; edx - integer
	;; bx - offset

_puthex:
	pusha
	mov ecx, 8
	mov al, '0'
	mov edi, hexbuf
	rep stosb
	mov BYTE[hexbuf+8], 0
	popa
	pusha
	mov edi, 7

.loop:
	mov al, dl
	and al, 0xf
	cmp al, 10
	jl  .digit
	jg  .alpha

.digit:
	add al, '0'
	jmp .ok

.alpha:
	add al, 'A'-10

.ok:
	mov [hexbuf+edi], al
	shr edx, 4
	dec edi
	cmp edi, 0
	jge .loop
	popa
	ret

putc:
	call calculate_offset
	call _putc
	ret

puts:
	call calculate_offset
	call _puts
	ret

puthex:
	call calculate_offset
	pusha
	call _puthex
	popa
	mov  esi, hexbuf
	call _puts
	ret
	;; eax - lines

flip:
	mov edi, 0xb8000
	mov ecx, 80*2
	mul ecx
	lea esi, [eax+edi]
	mov ecx, 80*2*25
	rep movsb
	mov ecx, esi
	sub ecx, edi
	mov al,0
	rep stosb
	ret

	%macro scr_move 2
	mov    ecx, %1
	mov    ebx, %2
	%endmacro

	%macro   print 4
	section  .data
	%%string db %4, 0
	section  .text
	scr_move %1, %2
	mov      ah, %3
	mov      esi, %%string
	call     puts
	%endmacro