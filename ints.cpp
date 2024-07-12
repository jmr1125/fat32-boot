#include "port.h"
#include "stdio.h"
// implementation for interupt 0 ~ 19
extern "C" {
extern void int0();
extern void int1();
extern void int2();
extern void int3();
extern void int4();
extern void int5();
extern void int6();
extern void int7();
extern void int8();
extern void int9();
extern void int10();
extern void int11();
extern void int12();
extern void int13();
extern void int14();
extern void int16();
extern void int17();
extern void int18();
extern void int19();
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
}
void int0() { puts("int0: DIV BY Zero", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int1() { puts("int1: DEBUG", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int2() { puts("int2: NMI", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int3() { puts("int3: BREAKPOINT", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int4() { puts("int4: OVERFLOW", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int5() { puts("int5: BOUND", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int6() { puts("int6: INVALID OPCODE", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int7() {
  puts("int7: DEVICE NOT AVAILABLE", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int8() { puts("int8: DOUBLE FAULT", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int9() {
  puts("int9: COPROC SEGMENT OVERFLOW", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int10() { puts("int10: INVALID TSS", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int11() {
  puts("int11: SEGMENT NOT PRESENT", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int12() { puts("int12: STACK FAULT", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int13() {
  puts("int13: GENERAL PROTECTION FAULT", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int14() { puts("int14: PAGE FAULT", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int16() { puts("int16: FPU ERROR", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int17() {
  puts("int17: ALIGNMENT CHECK", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int18() { puts("int18: MACHINE CHECK", COLOR_LIGHT_RED, (char *)0xb80a0); }
void int19() { puts("int19: SIMD FAULT", COLOR_LIGHT_RED, (char *)0xb80a0); }
void print(char *s) {
  for (; *s; ++s) {
    outb(0xe9, *s);
  }
}
void irq0() { // print("irq0\n");
}
void irq1() { print("irq1\n"); }
void irq2() { print("irq2\n"); }
void irq3() { print("irq3\n"); }
void irq4() { print("irq4\n"); }
void irq5() { print("irq5\n"); }
void irq6() { print("irq6\n"); }
void irq7() { print("irq7\n"); }
void irq8() { print("irq8\n"); }
void irq9() { print("irq9\n"); }
void irq10() { print("irq10\n"); }
void irq11() { print("irq11\n"); }
#include "ps2mouse.h"
void irq12() {
  static int bytes[128];
  static int bytes_count = 0, i = 0;
  print("irq12\n");
  char c = inb(0x60);
  bytes[bytes_count++] = c;
  bytes_count %= 128;
  ++i;
  if (i == 3) {
    i = 0;
    char c3, c2, c1;
    if (bytes_count - 1 < 0) {
      c3 = bytes[bytes_count - 1 + 128];
    } else {
      c3 = bytes[bytes_count - 1];
    }
    if (bytes_count - 2 < 0) {
      c2 = bytes[bytes_count - 2 + 128];
    } else {
      c2 = bytes[bytes_count - 2];
    }
    if (bytes_count - 3 < 0) {
      c1 = bytes[bytes_count - 3 + 128];
    } else {
      c1 = bytes[bytes_count - 3];
    }
    print("c1: ");
    for (int i = 7; i >= 0; --i) {
      if (c1 & (1 << i)) {
        print("1");
      } else {
        print("0");
      }
    }
    print("\nc2: ");
    for (int i = 7; i >= 0; --i) {
      if (c2 & (1 << i)) {
        print("1");
      } else {
        print("0");
      }
    }
    print("\nc3: ");
    for (int i = 7; i >= 0; --i) {
      if (c3 & (1 << i)) {
        print("1");
      } else {
        print("0");
      }
    }
    print("\n");
    if ((c1 & 0b00001000) == 0) {
      i = -1;
    }
    mouse_dx = c2;
    mouse_dy = -c3;
  }
}
void irq13() { print("irq13\n"); }
void irq14() { print("irq14\n"); }
void irq15() { print("irq15\n"); }
