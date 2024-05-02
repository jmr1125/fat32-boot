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
bootloader: bootloader.asm basic32.inc disk.inc makefile
	nasm bootloader.asm -fbin -o bootloader -g
res.img: boot writefat touch.txt bootloader
	make touch.txt&&./writefat
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
	echo file:/KERNEL/0 >> touch.txt
	echo "// generate by makefile" > config.h
	echo "#define kernel_off $(KERNEL_OFF)" >> config.h
	echo ";; generate by makefile" > config.inc
	echo "%define kernel_off $(KERNEL_OFF)" >> config.inc
