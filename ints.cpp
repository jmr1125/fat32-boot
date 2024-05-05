#include "stdio.h"
// implementation for interupt 0 ~ 19
extern "C"{
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
void int0(){
  puts("int0: DIV BY Zero",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int1(){
  puts("int1: DEBUG",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int2(){
  puts("int2: NMI",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int3(){
  puts("int3: BREAKPOINT",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int4(){
  puts("int4: OVERFLOW",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int5(){
  puts("int5: BOUND",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int6(){
  puts("int6: INVALID OPCODE",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int7(){
  puts("int7: DEVICE NOT AVAILABLE",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int8(){
  puts("int8: DOUBLE FAULT",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int9(){
  puts("int9: COPROC SEGMENT OVERFLOW",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int10(){
  puts("int10: INVALID TSS",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int11(){
  puts("int11: SEGMENT NOT PRESENT",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int12(){
  puts("int12: STACK FAULT",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int13(){
  puts("int13: GENERAL PROTECTION FAULT",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int14(){
  puts("int14: PAGE FAULT",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int16(){
  puts("int16: FPU ERROR",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int17(){
  puts("int17: ALIGNMENT CHECK",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int18(){
  puts("int18: MACHINE CHECK",COLOR_LIGHT_RED,(char*)0xb80a0);
}
void int19(){
  puts("int19: SIMD FAULT",COLOR_LIGHT_RED,(char*)0xb80a0);
}
