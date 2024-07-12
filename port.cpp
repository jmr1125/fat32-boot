#include "port.h"


byte in_byte(short port) {
  byte b;
  asm volatile("in %%dx, %%al" : "=a"(b) : "Nd"(port) : "memory");
  return b;
}
word in_word(short port) {
  word w;
  asm volatile("in %%dx, %%ax" : "=a"(w) : "Nd"(port) : "memory");
  return w;
}
dword in_dword(short port) {
  dword d;
  asm volatile("in %%dx, %%eax" : "=a"(d) : "Nd"(port) : "memory");
  return d;
}
void out_byte(short port, byte b) {
  asm volatile("out %%al, %%dx" : : "a"(b), "Nd"(port) : "memory");
}
void out_word(short port, word w) {
  asm volatile("out %%ax, %%dx" : : "a"(w), "Nd"(port) : "memory");
}
void out_dword(short port, dword d) {
  asm volatile("out %%eax, %%dx" : : "a"(d), "Nd"(port) : "memory");
}
