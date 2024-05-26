#include "int.h"
#include "ints.h"
#include "port.h"
#include "stdio.h"
#include "vbe.h"
extern "C" {
extern void k_main(void);
}
void k_main() {
  initialize_interrupts();
  puts("interrupts done", 0x0f, (char *)0xb8000 + 320);
  
}
