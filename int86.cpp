#include "int86.h"
#include "to_call_int.h"
void int86(int num, regs *in, regs *out) {
  // asm("xchg %bx,%bx");
  for (int i = 0; i < to_call_int_size; ++i) {
    *((char *)0x7c00 + i) = to_call_int_context[i];
  }
  // asm("xchg %bx,%bx");
  REGS->eax = in->eax;
  REGS->ebx = in->ebx;
  REGS->ecx = in->ecx;
  REGS->edx = in->edx;
  REGS->esi = in->esi;
  REGS->edi = in->edi;
  REGS->ebp = in->ebp;
  REGS->ds = in->ds;
  REGS->es = in->es;
  REGS->interrupt_no = num;
  asm("push %es");
  asm("push %ds");
  to_real();
  asm("pop %ds");
  asm("pop %es");
  if (out) {
    out->eax = REGS->eax;
    out->ebx = REGS->ebx;
    out->ecx = REGS->ecx;
    out->edx = REGS->edx;
    out->esi = REGS->esi;
    out->edi = REGS->edi;
    out->ebp = REGS->ebp;
  }
}
