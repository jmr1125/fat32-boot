#include "config.h"
#include "debug_port0xe9.h"
#include "font.h"
#include "int.h"
#include "int86.h"
#include "port.h"
#include "ps2keyboard.h"
#include "ps2mouse.h"
#include "stdio.h"
#include "stdlib.h"
#include "vbe.h"
#include "vesa.h"
extern "C" {
extern void k_main(void);
}
char buf[40];
void k_main() {
  regs in;
  in.eax = 0x0;
  in.ebx = 0x0;
  in.ecx = 0x0;
  in.edx = 0x0;
  in.esi = 0x0;
  in.edi = 0x0;
  in.es = 0;
  in.ds = 0;
  in.eax = 0x4f01;
  in.ecx = 0x4118;
  // in.es = ((long long)VESA_OFF & 0x10000) >> 4;
  in.es = 0x1000;
  // in.edi = VESA_OFF % 0xffff;
  in.edi = 0x30; // 0x10030
  regs out;
  int86(0x10, &in, &out);
  in.eax = 0x4f02;
  in.ebx = 0x4118;
  in.es = 0;
  in.edi = 0;
  int86(0x10, &in, &out);
  const auto vbeModeInfo = (vbe_mode_info_structure *)(VESA_OFF);
  const auto pitch = vbeModeInfo->pitch;
  const auto width = vbeModeInfo->width;
  const auto height = vbeModeInfo->height;
  const auto bpp = vbeModeInfo->bpp;
  char *const lfb = (char *)vbeModeInfo->framebuffer;
  initialize_interrupts();
  mouse_install();
  initialize_scancode();
  outb(0x64, 0xad);
  while (inb(0x64) & 1) {
    char c = inb(0x60);
    char s[] = {'(', c, ')', '\n'};
    print("unused char: ");
    print(s);
  }
  outb(0x64, 0xae);
  unsigned char c = 0;
  outb(0x60, 0xf0);
  while (c != 0xfa) {
    while (!getkey(&c))
      ;
    print("waitack: ");
    print_hex(c);
    print("\n");
  }
  outb(0x60, 0x2); // why scan code 1,2,3 all act like code 1
  while (c != 0xfa) {
    while (!getkey(&c))
      ;
    print("waitack: ");
    print_hex(c);
    print("\n");
  }
  int x = 100, y = 100;
  int x1 = x, y1 = y;
  while (true) {
    if (getkey(&c)) {
      // c &= 0b01111111;
      draws("key", 40, 0, lfb, pitch, bpp);
      to_hex(c, buf);
      draws(buf, 40, 64, lfb, pitch, bpp);
      draws("key", 60, 0, lfb, pitch, bpp);
      to_hex(scancode1[c].c, buf);
      draws(buf, 60, 64, lfb, pitch, bpp);
      draws("key", 80, 0, lfb, pitch, bpp);
      draws(get_key_name(scancode1[c].k), 80, 64, lfb, pitch, bpp);
      draws("key", 100, 0, lfb, pitch, bpp);
      draws((scancode1[c].k ? "pressed" : "released"), 100, 64, lfb, pitch,
            bpp);
    }
    y += mouse_dy;
    x += mouse_dx;
    x = max(0, min(x, height - 8 - 1));
    y = max(0, min(y, width - 16 - 1));
    mouse_dx = 0;
    mouse_dy = 0;
    draws("x:\0", 0, 0, lfb, pitch, bpp);
    to_dec(x, buf);
    draws(buf, 0, 32, lfb, pitch, bpp);
    draws("y:\0", 20, 0, lfb, pitch, bpp);
    to_dec(y, buf);
    draws(buf, 20, 32, lfb, pitch, bpp);
    if (!(x1 == x && y1 == y)) {
      draw(' ', x1, y1, lfb, pitch, bpp);
      draw('X', x, y, lfb, pitch, bpp);
      x1 = x;
      y1 = y;
    }
  }
}
