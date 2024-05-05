#include "port.h"
struct x86_int {
  word low_offset;
  word selector;
  word setting;
  word high_offset;
} __attribute__((packed));

struct x86_idtr {
  word limit;
  x86_int *base;
} __attribute__((packed));
static_assert(sizeof(x86_int *) == sizeof(uint32_t));

#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI 0x20 /* End-of-interrupt command code */

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define ICW1_ICW4 0x01      /* Indicates that ICW4 will be present */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08     /* Level triggered (edge) mode */
#define ICW1_INIT 0x10      /* Initialization - required! */

#define ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10       /* Special fully nested (not) */

#define PIC_READ_IRR 0x0a /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR 0x0b /* OCW3 irq service next CMD read */

inline void io_wait(void);
void PIC_sendEOI(uint8_t irq);
void PIC_remap(int offset1, int offset2);
void pic_disable(void);
void IRQ_set_mask(uint8_t IRQline);
void IRQ_clear_mask(uint8_t IRQline);
uint16_t __pic_get_irq_reg(int ocw3);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);

void loadIDTR();
void addInt(int n, void (*handler)(), dword dpl);
#define INT0 0x8E00
#define INT3 0xEE00

#define ALL 0xFF
#define TIMER 0
#define KEYBOARD 1
#define CASCADE 2
#define COM2_4 3
#define COM1_3 4
#define LPT 5
#define FLOPPY 6
#define FREE7 7
#define CLOCK 8
#define FREE9 9
#define FREE10 10
#define FREE11 11
#define PS2MOUSE 12
#define COPROC 13
#define IDE_1 14
#define IDE_2 15

void initialize_interrupts(void);
