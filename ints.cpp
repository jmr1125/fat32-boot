#include "stdio.h"
#include "port.h"
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
}
void int0() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int00"[i]);
  }
  puts("int0: DIV BY Zero", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int1() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int01"[i]);
  }
  puts("int1: DEBUG", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int2() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int02"[i]);
  }
  puts("int2: NMI", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int3() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int03"[i]);
  }
  puts("int3: BREAKPOINT", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int4() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int04"[i]);
  }
  puts("int4: OVERFLOW", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int5() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int05"[i]);
  }
  puts("int5: BOUND", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int6() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int06"[i]);
  }
  puts("int6: INVALID OPCODE", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int7() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int07"[i]);
  }
  puts("int7: DEVICE NOT AVAILABLE", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int8() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int08"[i]);
  }
  puts("int8: DOUBLE FAULT", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int9() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int09"[i]);
  }
  puts("int9: COPROC SEGMENT OVERFLOW", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int10() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int10"[i]);
  }
  puts("int10: INVALID TSS", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int11() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int11"[i]);
  }
  puts("int11: SEGMENT NOT PRESENT", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int12() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int12"[i]);
  }
  puts("int12: STACK FAULT", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int13() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int13"[i]);
  }
  puts("int13: GENERAL PROTECTION FAULT", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int14() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int14"[i]);
  }
  puts("int14: PAGE FAULT", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int16() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int16"[i]);
  }
  puts("int16: FPU ERROR", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int17() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int17"[i]);
  }
  puts("int17: ALIGNMENT CHECK", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int18() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int18"[i]);
  }
  puts("int18: MACHINE CHECK", COLOR_LIGHT_RED, (char *)0xb80a0);
}
void int19() {
  for (int i = 0; i < 5; ++i) {
    outb(0xeb, "int19"[i]);
  }
  puts("int19: SIMD FAULT", COLOR_LIGHT_RED, (char *)0xb80a0);
}
