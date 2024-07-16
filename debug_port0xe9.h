#include "port.h"
#define trap() asm volatile("xchg %bx,%bx")
void print(char *);
void print_char(char);
void print_hex(uint32_t);
void print_bin(uint32_t);
