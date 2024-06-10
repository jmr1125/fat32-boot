CXXFLAGS=-std=c++20
KERNEL_OFF=0x20000
VesaInfoBlockBufferOff=0x10000
VesaModeInfoBlockBufferOff=0x10200
all: readfat res.img
readfat: read.cpp fat32.hpp
	clang++ read.cpp $(CXXFLAGS) -g -o readfat
writefat: write.cpp fat32.hpp
	clang++ write.cpp $(CXXFLAGS) -g -o writefat
boot: boot.s basic.asm writefat bootloader
	./writefat --generate-header
	cat res.img.inc
	nasm -fbin boot.s -o boot -g
	nasm -felf32 boot.s -o boot.elf -g
	x86_64-elf-ld -melf_i386 -T link.boot.ld boot.elf -o boot.o
bootloader: bootloader.asm basic32.inc disk.inc
	nasm bootloader.asm -fbin -o bootloader -g # -DDEBUG
	nasm bootloader.asm -felf32 -o bootloader.elf -g # -DDEBUG
kernel.o: kernel.asm
	nasm kernel.asm -felf32 -o kernel.o -g
res.o: kernel.o c_kernel.o stdio.o int.o ints.o ints.c.o pci.c.o vbe.c.o ps2mouse.o port.o font.o
	x86_64-elf-ld -m elf_i386 -O i386 -T link.ld -o res.o kernel.o c_kernel.o stdio.o int.o ints.o ints.c.o ps2mouse.o port.o font.o
res: res.o
	x86_64-elf-objcopy -O binary res.o res
res.img: boot writefat touch.txt bootloader res
	make touch.txt&&echo === generating ===&&./writefat
touch.txt: always
	echo ---touch.txt:
	cat touch.txt
config.inc: always
	echo ---config.inc:
	cat config.inc
config.h: always
	echo ---config.h:
	cat config.h
test-gdt:
	nasm gdt.asm -fbin -o res.img
check-syntax:
	clang++ -fsyntax-only $(CXXFLAGS) read.cpp write.cpp fat32.hpp
always:
	echo vol:DGNOS > touch.txt
	echo real:bootloader >> touch.txt
	echo file:/boload/`stat -f %z bootloader` >> touch.txt
	echo real:res >> touch.txt
	echo file:/KERNEL/`stat -f %z res` >> touch.txt
	echo "// generate by makefile" > config.h
	echo "#define kernel_off $(KERNEL_OFF)" >> config.h
	echo "#define LFB_varible_addr $(LFB_VARIBLE_ADDR)" >> config.h
	echo "#define VesaInfoBlockBuffer $(VesaInfoBlockBufferOff)" >> config.h
	echo "#define VesaModeInfoBlockBuffer $(VesaModeInfoBlockBufferOff)" >> config.h
	echo ";; generate by makefile" > config.inc
	echo "%define kernel_off $(KERNEL_OFF)" >> config.inc
	echo "%define VesaInfoBlockBuffer $(VesaInfoBlockBufferOff)" >> config.inc
	echo "%define VesaModeInfoBlockBuffer $(VesaModeInfoBlockBufferOff)" >> config.inc



c_kernel.o: kernel.cpp stdio.h port.h to_call_int.h
	clang++ -std=c++17 -g --target=x86_64 -m32 -c -o c_kernel.o kernel.cpp
stdio.o: stdio.cpp
	clang++ -std=c++17 -g --target=x86_64 -m32 -c -o stdio.o stdio.cpp
COMPILE=clang++ -std=c++17 -g --target=x86_64 -m32 -c -o 
int.o: int.cpp int.h
	$(COMPILE) int.o int.cpp
ints.o: ints.asm
	nasm -felf32 ints.asm -o ints.o -g
ints.c.o: ints.cpp
	$(COMPILE) ints.c.o ints.cpp
pci.c.o: pci.cpp
	$(COMPILE) pci.c.o pci.cpp
vbe.c.o: vbe.cpp
	$(COMPILE) vbe.c.o vbe.cpp
ps2mouse.o: ps2mouse.cpp
	$(COMPILE) ps2mouse.o ps2mouse.cpp
file2symbolc: file2symbolc.cpp
	clang++ -o file2symbolc file2symbolc.cpp
to_call_int: to_call_int.asm
	nasm -fbin -o to_call_int to_call_int.asm
to_call_int.h: to_call_int file2symbolc
	./file2symbolc to_call_int to_call_int > to_call_int.h
font.o:font.c
	$(COMPILE) font.o font.c
