#include "stdio.h"
void putchar(char c, char a, char *off) {
  *(off) = c;
  *(off + 1) = a;
}
void puts(char *str, char attr, char *off) {
  while (*str != 0) {
    putchar(*str, attr, off);
    str++;
    off += 2;
  }
}
void flip(char n) {
  // move datas
  asm("rep movsb"
      :
      : "D"(0xb8000), "S"(0xb8000 + 80 * 2 * n), "c"(28 * 80 * 2 - n * 80 * 2));
  // clean others
  asm("rep stosb" : : "D"(0xb8000 + 80 * 2 * n), "c"(n * 80 * 2), "a"(0));
}
