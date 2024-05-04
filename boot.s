%ifidn __?OUTPUT_FORMAT?__,bin
	[map all boot.map]
	org 0x7c00
	%endif
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
global main
code:
	bits 16

main:
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7c00
	mov BYTE [DrvNum], dl

	mov  ax, [fat_loc]
	mov  cl, 1
	mov  dl, [DrvNum]
	mov  bx, buf_fat
	call disk_read

	mov  ax, [data_loc]
	mov  cl, 1
	mov  dl, [DrvNum]
	mov  bx, buf_data
	call disk_read

	mov bx, buf_data

.loop1:
	;find kernel entry
	or    WORD [bx], 0
	jz    .end1
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
	mov si,bx
	jmp      .end2

.break:
	add bx, 32
	jmp .loop1

	.end1:
	;; 
	mov  bx, not_found
	call puts
	jmp hlt
	.end2:
	mov  bx, not_found+4
	call puts
	mov WORD [kernel_pointer],kernel_code
	xor ebx, ebx
	mov bx, [si+0x14]
	shl ebx, 16
	mov bx, [si+0x1a]


	.read_kernel:
	;; bx - cluster to read
	push bx

	sub bx, 2
	mov ax, bx
	mul BYTE [BPB_SecPerClus]
	add ax,[data_loc]

	mov cl, [BPB_SecPerClus]
	mov dl, [DrvNum]
	mov bx, [kernel_pointer]
	call disk_read
	mov ax, [BPB_SecPerClus]
	mul WORD [BPB_BytsPerSec]
	add [kernel_pointer], ax

	pop bx

	push bx
	shl bx, 2
	mov eax,[buf_fat+bx]
	pop bx

	and eax, 0x0fffffff
	cmp eax, 0xFFFFFF8
	jge .end3
	mov bx,ax
	
	jmp .read_kernel

	.end3:
	;; mov ax, es
	;; shl ax, 4
	;; add ax, kernel_code
	mov BYTE dl, [DrvNum]
	jmp kernel_code
hlt:
	hlt
	jmp hlt

	; Compile using NASM compiler (Again look for it using a search engine)
	; Input: ax - LBA value
	; Output: ax - Sector
	; bx - Head
	; cx - Cylinder

lba_to_chs:
	;ax  - LBA
	;    - cx [bits 0-5]: sector number
	;    - cx [bits 6-15]: cylinder
	;    - dh: head
	push ax
	push dx

	xor dx, dx; dx = 0
	div word [BPB_SecPerTrk]; ax = LBA / SectorsPerTrack
	;   dx = LBA % SectorsPerTrack

	inc dx; dx = (LBA % SectorsPerTrack + 1) = sector
	mov cx, dx; cx = sector

	xor dx, dx; dx = 0
	div word [BPB_NumHeads]; ax = (LBA / SectorsPerTrack) / Heads = cylinder
	;   dx = (LBA / SectorsPerTrack) % Heads = head
	mov dh, dl; dh = head
	mov ch, al; ch = cylinder (lower 8 bits)
	shl ah, 6
	or  cl, ah; put upper 2 bits of cylinder in CL

	pop ax
	mov dl, al; restore DL
	pop ax
	ret

	
	; Reads sectors from a disk
	; Parameters:
	; - ax: LBA address
	; - cl: number of sectors to read (up to 128)
	; - dl: drive number
	; - es:bx: memory address where to store read data
	

disk_read:

	push ax; save registers we will modify
	push bx
	push cx
	push dx
	push di

	push cx; temporarily save CL (number of sectors to read)
	call lba_to_chs; compute CHS
	pop  ax; AL = number of sectors to read

	mov ah, 02h
	mov di, 3; retry count

.retry:
	pusha ; save all registers, we don't know what bios modifies
	stc   ; set carry flag, some BIOS'es don't set it
	int   13h; carry flag cleared = success
	jnc   .done; jump if carry not set

	;  read failed
	popa
	;; call disk_reset

	dec  di
	test di, di
	jnz  .retry

.fail:
	mov bx, disk_error
	;   all attempts are exhausted
	jmp hlt

.done:
	popa

	pop di
	pop dx
	pop cx
	pop bx
	pop ax; restore registers modified
	ret

%include "basic.asm"

%include "res.img.inc"

disk_error db "Disk Error", 0
not_found  db "Not Found Bootloader", 0
kernel  db "boload     "
times   510-($-$$) db 0
dw      0aa55h
section .bss

db     ?
DrvNum db ?
buf_fat resb 512
buf_data resb 512
kernel_pointer:	resb	4
kernel_code:
