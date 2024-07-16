#include "debug_port0xe9.h"
#include "port.h"
#include "ps2keyboard.h"
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
void int0() {
  trap();
  print("int0: DIV BY Zero");
}
void int1() {
  trap();
  print("int1: DEBUG");
}
void int2() {
  trap();
  print("int2: NMI");
}
void int3() {
  trap();
  print("int3: BREAKPOINT");
}
void int4() {
  trap();
  print("int4: OVERFLOW");
}
void int5() {
  trap();
  print("int5: BOUND");
}
void int6() {
  trap();
  print("int6: INVALID OPCODE");
}
void int7() {
  trap();
  print("int7: DEVICE NOT AVAILABLE");
}
void int8() {
  trap();
  print("int8: DOUBLE FAULT");
}
void int9() {
  trap();
  print("int9: COPROC SEGMENT OVERFLOW");
}
void int10() {
  trap();
  print("int10: INVALID TSS");
}
void int11() {
  trap();
  print("int11: SEGMENT NOT PRESENT");
}
void int12() {
  trap();
  print("int12: STACK FAULT");
}
void int13() {
  trap();
  print("int13: GENERAL PROTECTION FAULT");
}
void int14() {
  trap();
  print("int14: PAGE FAULT");
}
void int16() {
  trap();
  print("int16: FPU ERROR");
}
void int17() {
  trap();
  print("int17: ALIGNMENT CHECK");
}
void int18() {
  trap();
  print("int18: MACHINE CHECK");
}
void int19() {
  trap();
  print("int19: SIMD FAULT");
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
    print("\nbytes_count:");
    print_hex(bytes_count);
    print("\nc1: ");
    print_bin(c1);
    print("\nc2: ");
    print_bin(c2);
    print("\nc3: ");
    print_bin(c3);
    print("\n");
    if ((c1 & 0b00001000) == 0) {
      i = -1;
    }
    mouse_dy = c2;
    mouse_dx = -c3;
  }
}
void irq13() { print("irq13\n"); }
void irq14() { print("irq14\n"); }
void irq15() { print("irq15\n"); }
