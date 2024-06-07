#include "int.h"
#include "ints.h"
#include "port.h"
#include "stdio.h"
#include "vbe.h"
#include "v86.h"
#include "v86_func.h"
extern "C" {
extern void k_main(void);
}
void k_main() {
  initialize_interrupts();
  puts("interrupts done", 0x0f, (char *)0xb8000 + 320);

  for(int i=0x7c00;i<0x7c00+v86_func_context_size;i++) {
    *((char*)i)=v86_func_context[i-0x7c00];
  }
  enter_v86(0,(uint16_t)0x7c00,0,0,0<<12);
}
