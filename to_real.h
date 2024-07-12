
extern "C" {
extern void to_real();
}

struct regs{
  int eax,ebx,ecx,edx,esi,edi,ebp,esp,eflags,es,ds;
  int interrupt_no;
}__attribute((packed));
#include "config.h"
#define REGS ((regs*)REGS_OFF)
