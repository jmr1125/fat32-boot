	;; eax - number
sendeoi:
	cmp eax, 0x8
	mov al,0x20
	jle .pic1
	out 0xa0,al
	.pic1:
	out 0x20,al
	ret
	;; define function %1 and call %2
	%macro def 2
	global %1
	extern %2
%1:
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
	%endmacro
	%macro def_int 2
	def int_%1,%2
	iret
	%endmacro
	%macro def_irq 2
	def irq_%1,%2
	mov eax,%1
	call sendeoi
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
	def_irq 0, irq0
	def_irq 1, irq1
	def_irq 2, irq2
	def_irq 3, irq3
	def_irq 4, irq4
	def_irq 5, irq5
	def_irq 6, irq6
	def_irq 7, irq7
	def_irq 8, irq8
	def_irq 9, irq9
	def_irq 10, irq10
	def_irq 11, irq11
	def_irq 12, irq12
	def_irq 13, irq13
	def_irq 14, irq14
	def_irq 15, irq15
