CXXFLAGS=-std=c++20
KERNEL_OFF=0x10000
all: readfat res.img
readfat: read.cpp fat32.hpp makefile
	clang++ read.cpp $(CXXFLAGS) -g -o readfat
writefat: write.cpp fat32.hpp makefile
	clang++ write.cpp $(CXXFLAGS) -g -o writefat
boot: boot.s basic.asm writefat bootloader
	./writefat --generate-header
	cat res.img.inc
	nasm -fbin boot.s -o boot -g
#	nasm -felf32 boot.s -o boot.elf -g
bootloader: bootloader.asm basic32.inc disk.inc makefile
	nasm bootloader.asm -fbin -o bootloader -g # -DDEBUG
#	nasm bootloader.asm -felf32 -o bootloader.elf -g -DDEBUG
c_kernel.o: kernel.cpp makefile
	clang++ --target=x86_64 -m32 -c -o c_kernel.o kernel.cpp -g
kernel.o: kernel.asm makefile
	nasm kernel.asm -felf32 -o kernel.o -g
res.o: kernel.o c_kernel.o makefile
	x86_64-elf-ld -m elf_i386 -O i386 -T link.ld kernel.o c_kernel.o -o res.o
res: res.o makefile
	x86_64-elf-objcopy -O binary res.o res
res.img: boot writefat touch.txt bootloader res makefile
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
	echo ";; generate by makefile" > config.inc
	echo "%define kernel_off $(KERNEL_OFF)" >> config.inc
