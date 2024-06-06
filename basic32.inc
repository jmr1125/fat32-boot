
	%ifdef  DEBUG
	%define trap xchg bx, bx
	%else
	%define trap
	%endif

putc:
mov ah,0x0e
int 0x10
ret
puts:
lodsb
or al,al
jz .end
mov ah,0x0e
mov bh,0
int 0x10
jmp puts
.end:
ret

;; edi = buffer
;; al = value
_puthex16:
pusha
and al,0xf
cmp al,10
jl .num
add al, 'A'-10-'0'
.num:
add al, '0'
mov [edi], al
popa
ret

_puthexb:
pusha
	call _puthex16
	dec edi
shr al, 4
call _puthex16
popa
ret

_puthexw:
pusha
	call _puthexb
	sub edi, 2
shr ax, 8
call _puthexb
popa
ret

_puthexd:
pusha
	call _puthexw
	sub edi, 4
shr eax, 16
call _puthexw
popa
ret

	section .bss
hexbuf: times 16 db ?
.end: db ?
	section .text
_clear_hexbuf:
	pusha
	mov edi, hexbuf
	mov ecx, 16
	mov al,'0'
	rep stosb
	popa
	ret

print_hexb:
	pusha
	call _clear_hexbuf
	mov edi, hexbuf+2-1
	call _puthexb
	mov esi, hexbuf
	mov [hexbuf+2], byte 0
	call puts
	popa
	ret

print_hexw:
	pusha
	call _clear_hexbuf
	mov edi, hexbuf+4-1
	call _puthexw
	mov esi, hexbuf
	mov [hexbuf+4], byte 0
	call puts
	popa
	ret

print_hexd:
	pusha
	call _clear_hexbuf
	mov edi, hexbuf+8-1
	call _puthexd
	mov esi, hexbuf
	mov [hexbuf+8], byte 0
	call puts
	popa
	ret

	

%macro printaddr 1
pusha
mov esi, %1
call puts
popa
%endmacro

%macro print 1+
section .rodata
%%string: db %1, 0
section .text
printaddr %%string
%endmacro
