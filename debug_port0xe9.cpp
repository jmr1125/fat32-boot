#include "debug_port0xe9.h"

void print(char *s) {
  for (; *s; ++s) {
    outb(0xe9, *s);
  }
}
void print_char(char c) { outb(0xe9, c); }
void print_hex(uint32_t n) {
  for (int i = 7; i >= 0; --i) {
    int c = (n & (0xf << (4 * i))) >> (4 * i);
    outb(0xe9, "0123456789abcdef"[c]);
  }
}
void print_bin(uint32_t n) {
  for (int i = 31; i >= 0; --i) {
    outb(0xe9, (n & (1 << i)) ? '1' : '0');
  }
}
