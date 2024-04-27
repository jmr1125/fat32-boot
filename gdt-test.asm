	; boot.asm - 引导加载程序示例，用于进入保护模式并加载内核

	;  引导加载程序起始位置
	;; org 0x7c00

	; 引导加载程序入口点

boot:
	;   设置段寄存器
	xor ax, ax; 清零段寄存器
	mov ds, ax; DS = 0
	mov es, ax; ES = 0

	;    切换到保护模式
	cli  ; 禁止中断
	lgdt [gdt_descriptor]; 加载全局描述符表地址
	mov  eax, cr0; 读取控制寄存器CR0
	or   eax, 0x1; 设置PE位（启用保护模式）
	mov  cr0, eax; 写回控制寄存器CR0
	jmp  CODE_SEG:start_protected_mode; 跳转到保护模式代码段

	; 全局描述符表（GDT）定义

gdt_descriptor:
	dw gdt_end - gdt - 1; GDT界限
	dd gdt; GDT基址

gdt:
	;  空描述符
	dd 0x0; 基地址
	dd 0x0; 限长

gdt_code:
	;  代码段描述符（段基址0x0，限长0xFFFFF，可执行，非一致代码段，DPL=0，粒度为4KB）
	dw 0xFFFF; 限长
	dw 0x0; 基地址
	db 0x0; 基地址
	db 10011010b; 代码段描述符
	db 11001111b; 限长/粒度字段

gdt_data:
	;  数据段描述符（段基址0x0，限长0xFFFFF，可读写，向上扩展，DPL=0，粒度为4KB）
	dw 0xFFFF; 限长
	dw 0x0; 基地址
	db 0x0; 基地址
	db 10010010b; 数据段描述符
	db 11001111b; 限长/粒度字段

gdt_end:

	; 保护模式代码段起始位置

start_protected_mode:
	mov ax, DATA_SEG; 设置数据段选择子
	mov ds, ax
	mov ss, ax
	mov esp, stack_top; 设置栈顶指针

	;    加载内核并跳转
	mov  ebx, KERNEL_START; 内核加载地址
	call ebx; 跳转到内核入口

	; 无限循环
	cli

endloop:
	hlt
	jmp endloop

	; 数据段选择子定义
	DATA_SEG equ gdt_data - gdt
	CODE_SEG equ gdt_code - gdt

	; 栈顶指针定义
	stack_top dw 0x0000
	KERNEL_START equ 0x1000  ; 内核加载地址

	times 510 - ($ - $$) db 0; 填充到512字节
	dw    0xAA55; 引导扇区标识

	; 内核代码可以跟在这里
