#pragma once
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef unsigned long long int qword;
typedef byte uint8_t;
typedef word uint16_t;
typedef dword uint32_t;
typedef qword uint64_t;

inline byte in_byte(short port){
  byte b;
  asm volatile("in %%dx, %%eax" : "=a" (port) : "d" (b));
  return b;
}
inline word in_word(short port){
  word w;
  asm volatile("in %%dx, %%ax" : "=a" (port) : "d" (w));
  return w;
}
inline dword in_dword(short port){
  dword d;
  asm volatile("in %%dx, %%eax" : "=a" (port) : "d" (d));
  return d;
}
inline void out_byte(short port, byte b){
  asm volatile("out %%al, %%dx" : : "a" (b), "d" (port));
}
inline void out_word(short port, word w){
  asm volatile("out %%ax, %%dx" : : "a" (w), "d" (port));
}
inline void out_dword(short port, dword d){
  asm volatile("out %%eax, %%dx" : : "a" (d), "d" (port));
}

#define outb(port, data)		(out_byte(port, data))
#define outw(port, data)		(out_word(port, data))
#define outl(port, data)		(out_dword(port, data))
#define inb(port)			(in_byte(port))

