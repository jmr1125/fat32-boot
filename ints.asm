	%macro def_int 2
	global int_%1
	extern %2
int_%1:
	pusha
	push ds
	push es
	push fs
	push gs
	mov eax,0x10
	mov ds,eax
	mov es,eax
	cld
	call %2
	pop gs
	pop fs
	pop es
	pop ds
	popa
	iret
	%endmacro

	def_int 0, int0
	def_int 1, int1
	def_int 2, int2
	def_int 3, int3
	def_int 4, int4
	def_int 5, int5
	def_int 6, int6
	def_int 7, int7
	def_int 8, int8
	def_int 9, int9
	def_int 10, int10
	def_int 11, int11
	def_int 12, int12
	def_int 13, int13
	def_int 14, int14
	def_int 16, int16
	def_int 17, int17
	def_int 18, int18
	def_int 19, int19
