CXXFLAGS=-std=c++20
all: readfat writefat
readfat: read.cpp fat32.hpp makefile
	clang++ read.cpp $(CXXFLAGS) -g -o readfat
writefat: write.cpp fat32.hpp boot makefile
	clang++ write.cpp $(CXXFLAGS) -g -o writefat
boot: boot.s
	nasm -fbin boot.s -o boot
check-syntax:
	clang++ -fsyntax-only $(CXXFLAGS) read.cpp write.cpp fat32.hpp
