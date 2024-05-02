org     0x7c00
[map    all bootloader.map]
section .text
global  start
global  main

begin:
	bits 16
	call get_addr

get_addr:
	pop eax
	sub eax, 3
	mov [entry_point], ax
	mov bx, [entry_point]
	;;  copy self to 0x0000:0x7c00
	mov ax, 0
	mov es, ax
	mov ds, ax
	mov si, bx
	mov di, 0x7c00
	mov ecx, end - begin
	rep movsb
	jmp 0x0:0x7c00+start-begin; jmp to start of 0x7c00

start:
	[BITS 16]; We need 16-bit intructions for Real mode

	mov [drive], dl

	cli ; Disable interrupts, we want to be alone

	xor ax, ax
	mov ds, ax; Set DS-register to 0 - used by lgdt

	lgdt [gdt_desc]; Load the GDT descriptor

	mov eax, cr0; Copy the contents of CR0 into EAX
	or  eax, 1; Set bit 0
	mov cr0, eax; Copy the contents of EAX into CR0

	jmp 08h:clear_pipe; Jump to code segment, offset clear_pipe

	[BITS 32]; We now need 32-bit instructions

clear_pipe:
	mov ax, 10h; Save data segment identifyer
	mov ds, ax; Move a valid data segment into the data segment register
	mov ss, ax; Move a valid data segment into the stack segment register
	mov es, ax; Move a valid data segment into the extra segment register
	mov fs, ax; Move a valid data segment into the fs register
	mov gs, ax; Move a valid data segment into the gs register
	mov esp, 090000h; Move the stack pointer to 090000h

	;;  clear the screen
	mov edi, 0xb8000
	mov ecx, 80*25*2
	mov al, 0
	rep stosb
	jmp main
%include "basic32.inc"
%include "disk.inc"
%include "res.img.inc"
%include "config.inc"

main:
	mov  esi, protected_mode_string
	mov  ecx, 24
	mov  ebx, 0
	mov  ah, 0x0f
	call puts
	mov  eax, 1
	call flip

	print 24, 1, 0x0f, "drive:"
	scr_move 24,8
	mov   edx, [drive]
	and   edx, 0xff
	call  puthex
	mov   eax, 1
	call  flip

	mov  ebx, 0x00000001
	mov  ch, 1
	mov  edi, bootsect
	call read

	xor   edx, edx
	print 24, 1, 0x0f, "fat location:"
	scr_move 24,14
	mov   dx, [fat_loc]
	call  puthex
	mov   eax, 1
	call  flip
	print 24, 1, 0x0f, "data location:"
	scr_move 24,15
	mov   dx, [data_loc]
	call  puthex
	mov   eax, 1
	call  flip


	%macro read_fat 1
	xor  ebx, ebx
	mov  bx, [fat_loc]
	add bx, %1
	xchg bx,bx
	call lba2chs
	mov  ch, 1
	mov  edi, buffer.fat; disk.inc:wait100ns()
	call read
	%endmacro

	%macro read_data 1
	xor  ebx, ebx
	mov  bx, [data_loc]
	add bx, %1
	xchg bx,bx
	call lba2chs
	mov  ch, 1
	mov  edi, buffer.data; disk.inc:wait100ns()
	call read
	%endmacro

	mov  ax, 0
	;; clear buffer
	mov  ecx, 512
	mov  al, 0
	mov  edi, buffer.data
	rep  stosb
	mov  edi, buffer.fat
	rep  stosb
	.find_kernel_reread:
	read_data ax
	inc ax
	mov si,0
	.next_entry:
	or  BYTE [buffer.data+si], 0
	jz  .break
	;; strcmp
	push si
	push di
	mov di, kernel_name
	lea si, [buffer.data+si]
	rep cmpsb
	mov al, [di]
	pop di
	pop si
	cmp al, 0xff
	push ebx
	push si
	je  .break_found
	add si, 32
	cmp si, 512
	je  .find_kernel_reread
	jmp .next_entry
	.break:
	print 24, 1, 0x0f, "kernel not found"
	mov   eax, 1
	call  flip
	jmp hang
	.break_found:
	print 24, 1, 0x0f, "kernel found"
	mov   eax, 1
	call  flip
	print 24, 1, 0x0f, "sector: data+"
	scr_move 24,15
	mov   edx, ebx
	call  puthex
	mov   eax, 1
	call  flip
	print 24, 1, 0x0f, "offset: 0x"
	scr_move 24,12
	xor   edx, edx
	mov   dx, si
	call  puthex
	mov   eax, 1
	call  flip
hang:
	mov dx,0xe9
	mov al,'h'
	out dx,al
	mov al,'a'
	out dx,al
	mov al,'l'
	out dx,al
	mov al,'t'
	out dx,al
.hang:	jmp hang.hang; Loop, self-jump
section .data

drive:
	db 0
	entry_point: dw 0

protected_mode_string:
	db '[PM] Protected Mode now'

.end:
	db 0

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

end:
kernel_name:	db 'KERNEL     ',0xff
	section .bss

hexbuf:
	resb 8
	db   ?

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

buffer:
.fat:
	times 512 db ?

.data:
	times 512 db ?
kernel_cluster: dd ?
