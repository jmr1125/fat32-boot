#include "config.h"
#include "font.h"
#include "int.h"
#include "int86.h"
#include "port.h"
#include "ps2mouse.h"
#include "stdio.h"
#include "vbe.h"
#include "vesa.h"
extern "C" {
extern void k_main(void);
}
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
  int x = 100, y = 100;
  while (true) {
    draw('x', y, x, lfb, pitch, bpp, 1);
    y += mouse_dy;
    x += mouse_dx;
    mouse_dx = 0;
    mouse_dy = 0;
  }
}
