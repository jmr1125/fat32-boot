CXXFLAGS=-std=c++20
all: readfat res.img
readfat: read.cpp fat32.hpp makefile
	clang++ read.cpp $(CXXFLAGS) -g -o readfat
writefat: write.cpp fat32.hpp makefile
	clang++ write.cpp $(CXXFLAGS) -g -o writefat
boot: boot.s
	nasm -fbin boot.s -o boot
res.img: boot writefat touch.txt
	./writefat
touch.txt: makefile
	echo vol:DGNOS > touch.txt
	echo real:kernel >> touch.txt
	echo file:/kernel/`stat -f %z kernel` >> touch.txt
	echo ---touch.txt:
	cat touch.txt
check-syntax:
	clang++ -fsyntax-only $(CXXFLAGS) read.cpp write.cpp fat32.hpp
