#include "int.h"
#include "ints.h"
inline void io_wait(void) { outb(0x80, 0); }
void PIC_sendEOI(uint8_t irq) {
  if (irq >= 8)
    outb(PIC2_COMMAND, PIC_EOI);

  outb(PIC1_COMMAND, PIC_EOI);
}

/*
arguments:
        offset1 - vector offset for master PIC
                vectors on the master become offset1..offset1+7
        offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2) {
  uint8_t a1, a2;

  a1 = inb(PIC1_DATA); // save masks
  a2 = inb(PIC2_DATA);

  outb(PIC1_COMMAND,
       ICW1_INIT |
           ICW1_ICW4); // starts the initialization sequence (in cascade mode)
  io_wait();
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(PIC1_DATA, offset1); // ICW2: Master PIC vector offset
  io_wait();
  outb(PIC2_DATA, offset2); // ICW2: Slave PIC vector offset
  io_wait();
  outb(
      PIC1_DATA,
      4); // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
  io_wait();
  outb(PIC2_DATA, 2); // ICW3: tell Slave PIC its cascade identity (0000 0010)
  io_wait();

  outb(PIC1_DATA,
       ICW4_8086); // ICW4: have the PICs use 8086 mode (and not 8080 mode)
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();

  outb(PIC1_DATA, a1); // restore saved masks.
  outb(PIC2_DATA, a2);
}

void pic_disable(void) {
  outb(PIC1_DATA, 0xff);
  outb(PIC2_DATA, 0xff);
}

void IRQ_set_mask(uint8_t IRQline) {
  if (IRQline == ALL) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
    return;
  }
  uint16_t port;
  uint8_t value;

  if (IRQline < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    IRQline -= 8;
  }
  value = inb(port) | (1 << IRQline);
  outb(port, value);
}

void IRQ_clear_mask(uint8_t IRQline) {
  uint16_t port;
  uint8_t value;

  if (IRQline < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    IRQline -= 8;
  }
  value = inb(port) & ~(1 << IRQline);
  outb(port, value);
}

/* Helper func */
uint16_t __pic_get_irq_reg(int ocw3) {
  /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
   * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
  outb(PIC1_COMMAND, ocw3);
  outb(PIC2_COMMAND, ocw3);
  return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void) { return __pic_get_irq_reg(PIC_READ_IRR); }

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void) { return __pic_get_irq_reg(PIC_READ_ISR); }

x86_idtr IDTR;
x86_int idt[256];
void loadIDTR() {
  IDTR.limit = 256 * (sizeof(x86_int) - 1);
  IDTR.base = idt;
  __asm__ volatile("lidt (%0)" : : "p"(&IDTR));
}

void addInt(int n, void (*handler)(), dword dpl) {
  word selector = 0x08;
  asm volatile("mov %%cs,%0" : "=a"(selector));
  word settings;
  switch (dpl) {
  case 0:
    settings = INT0;
    break;
  case 1:
  case 2:
  case 3:
    settings = INT3;
    break;
  }
  dword off = (dword)handler;
  idt[n].low_offset = (off & 0xffff);
  idt[n].selector = selector;
  idt[n].setting = settings;
  idt[n].high_offset = (off >> 16);
}
void initialize_interrupts(void) {
  asm volatile("cli");
  PIC_remap(0x20, 0x28);
  IRQ_set_mask(ALL);
  loadIDTR();
  addInt(0, int_0, 0);
  addInt(1, int_1, 0);
  addInt(2, int_2, 0);
  addInt(3, int_3, 0);
  addInt(4, int_4, 0);
  addInt(5, int_5, 0);
  addInt(6, int_6, 0);
  addInt(7, int_7, 0);
  addInt(8, int_8, 0);
  addInt(9, int_9, 0);
  addInt(10, int_10, 0);
  addInt(11, int_11, 0);
  addInt(12, int_12, 0);
  addInt(13, int_13, 0);
  addInt(14, int_14, 0);
  addInt(16, int_16, 0);
  addInt(17, int_17, 0);
  addInt(18, int_18, 0);
  addInt(19, int_19, 0);
  addInt(32, irq_0, 0);
  addInt(33, irq_1, 0);
  addInt(34, irq_2, 0);
  addInt(35, irq_3, 0);
  addInt(36, irq_4, 0);
  addInt(37, irq_5, 0);
  addInt(38, irq_6, 0);
  addInt(39, irq_7, 0);
  addInt(40, irq_8, 0);
  addInt(41, irq_9, 0);
  addInt(42, irq_10, 0);
  addInt(43, irq_11, 0);
  addInt(44, irq_12, 0);
  addInt(45, irq_13, 0);
  addInt(46, irq_14, 0);
  addInt(47, irq_15, 0);
  for (int i = 0; i < 16; ++i) {
    IRQ_clear_mask(i);
  }
  asm volatile("sti");
}
