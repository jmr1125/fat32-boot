bootsector:
jmpcmd:
	db 0
	db 0
	db 0; off=0x0
	BS_OEMNAME: times 8 db 0       ; off=0x3
	BPB_BytsPerSec: dw 0 ; off=0xb
	BPB_SecPerClus: db 0   ; off=0xd
	BPB_RsvdSecCnt: dw 0   ; off=0xe
	BPB_NumFATs: db 0      ; off=0x10
	BPB_RootEntCnt: dw 0   ; off=0x11
	BPB_TotSec16: dw 0     ; off=0x13
	BPB_Media: db 0      ; off=0x15
	BPB_FATSz16: dw 0      ; off=0x16
	BPB_SecPerTrk: dw 0  ; off=0x18
	BPB_NumHeads: dw 0   ; off=0x1a
	BPB_HiddSec: dd 0      ; off=0x1c
	BPB_TotSec32: dd 0     ; off=0x20
	BPB_FATSz32: dd 0      ; off=0x24
	BPB_ExtFlags: dw 0     ; off=0x28
	BPB_FSVer: dw 0        ; off=0x2a
	BPB_RootClus: dd 0     ; off=0x2c
	BPB_FSInfo: dw 0       ; off=0x30
	BPB_BkBootSec: dw 0    ; off=0x32

BPB_Reserved:
	times 12 db 0; off=0x34
	BS_DrvNum: db 0     ; off=0x40
	BS_Reserved: db 0      ; off=0x41
	BS_BootSig: db 0     ; off=0x42
	BS_VolID: dd 0         ; off=0x43

BS_VolLab:
	times 11 db 0; off=0x47
	BS_FilSysType: db 'FAT32', 0, 0, 0 ; off=0x52

code:
	bits 16

main:
	mov ax, 0x07c0
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7c00
	mov BYTE [DrvNum], dl

	mov ax, [BPB_RsvdSecCnt]
	mov [fat_loc], ax
	mov bx, [fat_loc]

	mov eax, [BPB_FATSz32]
	mul DWORD [BPB_NumFATs]
	mov ebx, eax

	mov eax, [BPB_RootEntCnt]
	mov ecx, 32
	mul ecx
	add eax, [BPB_BytsPerSec]
	dec eax
	div DWORD [BPB_BytsPerSec]

	add eax, ebx
	add eax, [BPB_RsvdSecCnt]

	mov [data_loc], eax

	xor eax, eax
	mov  ax, [fat_loc]
	call LBACHS
	mov  dh, 1
	mov  di, buf_fat
	call disk_read

	xor eax, eax
	mov  ax, [data_loc]
	sub ax,8
	call LBACHS
	mov  dh, 16
	mov  di, buf_data
	call disk_read

	xor edx, edx
	mov bx, buf_data

.loop1:
	;find kernel entry
	or    WORD [bx], 0
	jz    .end1
	mov   bx, [bx]
	call  puts
	xor   si, si

.cmp:
	;compare kernel entry
	mov      al, [bx+si]
	mov      cl, [kernel+si]
	cmp      al, cl
	jne      .break
	inc      si
	cmp      si, 11
	jl       .cmp
	;;       found
	mov      ax, si
	jmp      .end1

.break:
	add bx, 32
	jmp .loop1

.end1:
	mov  bx, not_found
	call puts

hlt:
	hlt
	jmp hlt
	; Compile using NASM compiler (Again look for it using a search engine)
	; Input: ax - LBA value
	; Output: ax - Sector
	; bx - Head
	; cx - Cylinder

LBACHS:
	PUSH dx; Save the value in dx
	XOR  dx, dx; Zero dx
	MOV  bx, [BPB_SecPerTrk]; Move into place STP (LBA all ready in place)
	DIV  bx; Make the divide (ax/bx -> ax, dx)
	inc  dx; Add one to the remainder (sector value)
	push dx; Save the sector value on the stack

	XOR dx, dx; Zero dx
	MOV bx, [BPB_NumHeads]; Move NumHeads into place (NumTracks all ready in place)
	DIV bx; Make the divide (ax/bx -> ax, dx)

	MOV cx, ax; Move ax to cx (Cylinder)
	MOV bx, dx; Move dx to bx (Head)
	POP ax; Take the last value entered on the stack off.
	;   It doesn't need to go into the same register.
	;   (Sector)
	POP dx; Restore dx, just in case something important was
	;   originally in there before running this.
	RET ; Return to the main function

	disk_read:     ;ax-sector, bx-head, cx-cylinder, dh-sector count, di=buffer
	push bp
	mov  bp, sp
	sub  bp, 6
	mov  [bp], ax
	mov  [bp+2], bx
	mov  [bp+4], cx
	xor  eax, eax
	xor  ebx, ebx
	xor  ecx, ecx
	mov  ah, 2
	mov  al, dh
	xor  edx, edx
	mov  cx, [bp+4]
	shr  cx, 2
	and  cx, 0xc0
	or   cl, [bp]
	mov  ch, [bp+4]
	mov  bx, di
	stc
	mov  dl, [DrvNum]
	int  0x13
	jc   .error
	add  bp, 6
	pop  bp
	ret

.error:
	mov  bx, disk_error
	call puts
	jmp  hlt

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
	mov  [es:di], al
	dec  di
	shr  ebx, 4
	or   ebx, ebx
	jnz  .loop
	mov  bx, HexBuf
	call puts
	ret

putc:
	mov ah, 0x0e; put char in: al
	int 10h
	ret

puts:
	mov ah, 0x0e; put string in: bx

.loop:
	mov al, [ds:bx]
	or  al, al
	jz  .done
	inc bx
	int 10h
	jnz .loop

.done:
	ret

endl db 0xd, 0xa, 0

data db "data ", 0
size    db "size: ", 0
loc     db "location ", 0
disk_error db "Disk Error", 0
not_found  db "Not Found", 0
kernel  db "kernel     "
times   510-($-$$) db 0
dw      0aa55h
section .bss

HexBuf resb   8
db     ?
DrvNum db ?
fat_loc dw ?  ; ATTENTION: in SECTOR
data_loc dw ?  ; ATTENTION: in SECTOR
buf_fat resb 512
buf_data resb 512

list times 512 dd ?
list_i dd ?
