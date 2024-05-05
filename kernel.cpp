#include "int.h"
#include "ints.h"
#include "port.h"
#include "stdio.h"
extern "C" {
extern void k_main(void);
}
void k_main() {
  initialize_interrupts();
  puts("loaded", 0x0f, (char *)0xb8000 + 320);
  int a = 0;
  a /= 0;
}
