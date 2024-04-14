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

	mov  ax, [0x12]; BPB_RootEntCnt
	mov  cx, 32
	mul  cx
	add  ax, [BPB_BytsPerSec]
	dec  ax
	div  WORD [BPB_SecPerClus]; root_dir_sectors
	add  ax, [fat_loc]
	xchg ax, bx; now it stores at bx
	xor  ax, ax

	mov eax, [0x24]
	xor cx, cx
	mov cl, [0x10]
	mul ecx; ax = num fats * fat size
	add ax, bx
	mov [data_loc], ax

	mov  bx, helloworld
	call puts
	mov  bx, endl
	call puts
	xor  bx, bx
	mov  bl, [0xd]
	call puthex
	mov  bx, endl
	call puts

	mov eax,1
	push cx
	push bx
	push ax
	pop bx
	call puthex
	mov bx, endl
	call puts
	pop bx
	call puthex
	mov bx, endl
	call puts
	pop bx
	call puthex

hlt:
	hlt
	jmp hlt
lbachs:	push dx			;ax=lba => ax-sector bx-head cx-cylinder
	xor dx, dx
	mov bx, [BPB_SecPerTrk]
	div bx
	inc dx
	push dx

	xor dx, dx
	mov bx, [BPB_NumHeads]
	div bx

	mov cx, ax
	mov bx, dx
	pop ax
	pop dx
	ret
disk_read:			;eax = lba
	push bp
	mov bp,sp
	sub bp,17
	mov [bp+17], bx			;buffer
	mov [bp+16], cl			;len in sectors
	xor edx, edx
	mov ebx, eax
	div DWORD [BPB_SecPerTrk]
	mov [bp+12], eax		;Temp
	inc edx
	mov [bp+8], edx		;Sector
	xor edx, edx
	div DWORD [BPB_NumHeads] ;Temp%NumOfHead
	mov [bp+4], edx		;Head
	mov eax, [bp+8]		;Temp
	div DWORD [BPB_NumHeads] ;Temp/NumOfHead
	mov [bp], eax		;Cylinder
	;; [bp] cylinder
	;; [bp+4] head
	;; [bp+8] sector
	;; [bp+12] temp
	;; [bp+16] len
	;; [bp+17] buffer
	mov ah,2
	
	mov al, [bp+16]
	
	mov ecx, [bp]
	and ecx, 0xff
	mov ch,cl
	
	mov edx, [bp]
	shr edx, 2
	and edx, 0xc0
	mov cl,dl

	mov dh, [bp+4]
	
	mov dl, [DrvNum]
	
	int 0x13
	jc .error
	add sp, 17
	pop bp
	ret
	.error:
	mov bx, disk_error
		call puts
		jmp hlt
	
puthex:
	push ebx
	mov di, HexBuf
	mov cx, 8
	mov al, '0'
	rep stosb
	mov BYTE [HexBuf+8], 0
	lea di, [HexBuf+7]
	pop ebx
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
	mov [es:di], al
	dec di
	shr  ebx, 4
	or   ebx, ebx
	jnz .loop
	mov bx, HexBuf
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
endl       db 0xd, 0xa, 0
	helloworld db "Hello, World!", 0
	disk_error db "Disk Error", 0
times      510-($-$$) db 0
dw         0aa55h
	section    .bss
HexBuf: resb 8
	db ?
DrvNum     db ?
fat_loc dw ?  ; ATTENTION: in SECTOR
data_loc dw ?  ; ATTENTION: in SECTOR
buffer     resb 512
