%ifidn __?OUTPUT_FORMAT?__,bin
	org     0x9000
	[map    all bootloader.map]
	%endif

%include "config.inc"
	%define endl 0x0d, 0x0a
section .text
global  start
global  main

%ifdef   DEBUG
%warning "DEBUG IS ON"
%endif

jmp start
%include "basic32.inc"
start:
	[BITS 16]; We need 16-bit intructions for Real mode
	;; enter unreal mode
	xor ax,ax
	mov ds,ax
	mov ss,ax
	push ds
	cli
	lgdt [unreal_gdtinfo]
	mov eax,cr0
	or al,1
	mov cr0,eax
	jmp 0x8:.pmode
	.pmode:
	mov bx,0x10
	mov ds,bx
	and al,0xfe
	mov cr0,eax
	jmp 0:.unreal
	.unreal:
	pop ds
	sti	
	;; we are at unreal mode
	mov [disk], dl
	mov esi, ENDL
	call puts
	mov esi, now_bootloader
	call puts
	mov ah,0x41
	mov bx,0x55aa
	mov dl,0x80
	int 0x13
	jc .not_support
	jmp .support
	.not_support:
	mov esi, lba_support
	call puts
	jmp .hang
	.support:
	mov esi, lba_support+3
	call puts
	
	print "read bootsect",endl
	mov eax, 0
	mov cl, 1
	mov edi, bootsect
	mov dl, [disk]
	call read_int

	xor eax, eax
	mov ax, [data_loc]
	push eax
	jmp .loop_reread

	.loop_find_entry:
	or BYTE [si], 0
	jz .not_found
	
	push si
	push di
	lea di,[kernel_name]
	.continue:
	cmpsb
	je .continue
	cmp di,12+kernel_name
	pop di
	pop si
	jge .found

	add si, 32
	cmp si, 512+buffer.data
	jl .loop_find_entry
	.loop_reread:
	mov cl,1
	mov edi, buffer.data
	pop eax
	push eax
	mov dl, [disk]
	call read_int
	pop eax
	add eax, 512
	push eax
	mov si, buffer.data
	jmp .loop_find_entry

	.not_found:
	mov esi, not_found
	call puts
	hlt
	.found:
	pop eax
	push si
	mov esi, not_found+4
	call puts
	pop si
	mov ax,[si+20]
	shl eax,16
	mov ax,[si+26]

	mov [kernel_pointer], ax
	print "kernel at cluster 0x"

	call print_hexw

	printaddr ENDL

	mov DWORD [kernel_off_pointer], kernel_off
	.read_cluster:
	mov ax, [kernel_pointer]
	pusha
	;; ax - cluster num
	print "data read cluster 0x"
	call print_hexw
	printaddr ENDL
	sub ax, 2
	xor bx,bx
	mov bl,[bootsect.BPB_SecPerClus]
	imul ax, bx
	add ax,[data_loc]
	print "data read sector 0x"
	call print_hexw
	printaddr ENDL
	mov cl, [bootsect.BPB_SecPerClus]
	mov edi, [kernel_off_pointer]
	mov dl, [disk]
	trap
	call read_int
	xor bx, bx
	mov bx, [bootsect.BPB_SecPerClus]
	imul bx, [bootsect.BPB_BytsPerSec]
	add [kernel_off_pointer], bx
	popa
	.read_fat:
	pusha
	;; ax*4/512
	shr ax, 7
	add ax, [fat_loc]
	print "fat read sector 0x"
	call print_hexw
	printaddr ENDL
	mov cl, 1
	mov edi, buffer.fat
	mov dl, [disk]
	call read_int
	popa
	pusha
	mov bx,ax
	;; (bx*4) mod 512
	shl bx,2
	and ebx, 0x1ff
	mov eax,[buffer.fat+ebx]
	and eax, 0x0fffffff
	cmp eax,0x0ffffff8
	jge .done
	print "fat value 0x"
	pusha
	call print_hexd
	popa
	printaddr ENDL
	
	mov [kernel_pointer],ax
	popa
	jmp .read_cluster
	.done:
	popa
	
	mov ax, 0x13
	int 0x10



	xor ax, ax
	mov ds, ax; Set DS-register to 0 - used by lgdt

	lgdt [gdt_desc]; Load the GDT descriptor

	mov eax, cr0; Copy the contents of CR0 into EAX
	or  eax, 1; Set bit 0
	mov cr0, eax; Copy the contents of EAX into CR0

	jmp 08h:clear_pipe; Jump to code segment, offset clear_pipe
	.hang:
	hlt
	jmp .hang
	

%include "disk.inc"
	[BITS 32]; We now need 32-bit instructions

clear_pipe:
	mov ax, 10h; Save data segment identifyer
	mov ds, ax; Move a valid data segment into the data segment register
	mov ss, ax; Move a valid data segment into the stack segment register
	mov es, ax; Move a valid data segment into the extra segment register
	mov fs, ax; Move a valid data segment into the fs register
	mov gs, ax; Move a valid data segment into the gs register
	mov esp, 090000h; Move the stack pointer to 090000h
	mov ebp, 090000h; Move the base pointer to 090000h

	;;  clear the screen
	mov edi, 0xb8000
	mov ecx, 80*25*2
	mov al, 0
	rep stosb
	jmp main
	cli
	hlt
%include "res.img.inc"

main:
	trap
	mov BYTE [0xa0000],0x0f
	jmp kernel_off
hang:
	mov dx, 0xe9
	mov al, 'h'
	out dx, al
	mov al, 'a'
	out dx, al
	mov al, 'l'
	out dx, al
	mov al, 't'
	out dx, al

.hang:
	jmp hang.hang; Loop, self-jump
section .data

	section .rodata


.end:
	db 0
unreal_gdtinfo:
   dw unreal_gdt_end - unreal_gdt - 1   ;last byte in table
   dd unreal_gdt                 ;start of table

unreal_gdt:        dd 0,0        ; entry 0 is always unused
codedesc:   db 0xff, 0xff, 0, 0, 0, 10011010b, 00000000b, 0
flatdesc:   db 0xff, 0xff, 0, 0, 0, 10010010b, 11001111b, 0
unreal_gdt_end:
gdt:
	; Address for the GDT

	gdt_null:               ; Null Segment
	dd 0
	dd 0

	gdt_code:               ; Code segment, read/execute, nonconforming
	dw 0FFFFh
	dw 0
	db 0
	db 10011010b
	db 11001111b
	db 0

	gdt_data:               ; Data segment, read/write, expand down
	dw 0FFFFh
	dw 0
	db 0
	db 10010010b
	db 11001111b
	db 0

	gdt_end:                ; Used to calculate the size of the GDT

	gdt_desc:                       ; The GDT descriptor
	dw gdt_end - gdt - 1; Limit (size)
	dd gdt; Address of the GDT

ENDL:	db endl,0
now_bootloader: db 'Now in bootloader',endl,0
lba_support: db 'NO LBA support',endl,0
not_found:	db 'NOT found',endl,0
	kernel_name: db 'KERNEL      ',0xff

	times 100 db 0x88
	align 512 ,db 0x89

	section .bss


bootsect:
	.jmpcmd:db ?, ?, ?; off=0x0
	.BS_OEMNAME: times 8 db ?       ; off=0x3
	.BPB_BytsPerSec: dw ? ; off=0xb
	.BPB_SecPerClus: db ?   ; off=0xd
	.BPB_RsvdSecCnt: dw ?   ; off=0xe
	.BPB_NumFATs: db ?      ; off=0x10
	.BPB_RootEntCnt: dw ?   ; off=0x11
	.BPB_TotSec16: dw ?     ; off=0x13
	.BPB_Media: db ?      ; off=0x15
	.BPB_FATSz16: dw ?      ; off=0x16
	.BPB_SecPerTrk: dw ?  ; off=0x18
	.BPB_NumHeads: dw ?   ; off=0x1a
	.BPB_HiddSec: dd ?      ; off=0x1c
	.BPB_TotSec32: dd ?     ; off=0x20
	.BPB_FATSz32: dd ?      ; off=0x24
	.BPB_ExtFlags: dw ?     ; off=0x28
	.BPB_FSVer: dw ?        ; off=0x2a
	.BPB_RootClus: dd ?     ; off=0x2c
	.BPB_FSInfo: dw ?       ; off=0x30
	.BPB_BkBootSec: dw ?    ; off=0x32

.BPB_Reserved:
	times 12 db ?; off=0x34
	.BS_DrvNum: db ?     ; off=0x40
	.BS_Reserved: db ?      ; off=0x41
	.BS_BootSig: db ?     ; off=0x42
	.BS_VolID: dd ?         ; off=0x43

.BS_VolLab:
	times 11 db ?; off=0x47
	.BS_FilSysType: times 8 db ? ; off=0x52
	.BS_BootCode32: times 420 db ?
	.BS_BootSign: times 2 db ?

buffer:
.fat:
	times 512 db ?

.data:
	times 512 db ?

disk:	db ?
kernel_pointer:	dw ?
kernel_off_pointer:	dd ?
tmp_buffer:	times 512 db ?
