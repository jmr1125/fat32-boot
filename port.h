#pragma once
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef unsigned long long int qword;
typedef byte uint8_t;
typedef word uint16_t;
typedef dword uint32_t;
typedef qword uint64_t;
typedef unsigned short USHORT;

byte in_byte(short port) ;
word in_word(short port);
dword in_dword(short port);
void out_byte(short port, byte b);
void out_word(short port, word w);
void out_dword(short port, dword d) ;
#define outb(port, data) (out_byte(port, data))
#define outw(port, data) (out_word(port, data))
#define outl(port, data) (out_dword(port, data))
#define inb(port) (in_byte(port))
#define inl(port) (inb(port))
#define WRITE_PORT_USHORT(port, data) (out_word(port, data))
#define READ_PORT_USHORT(port) (in_word(port))
#define inportb(port) (inb(port))
#define inportw(port) (inw(port))
#define inportl(port) (inl(port))
#define outportb(port, data) (outb(port, data))
