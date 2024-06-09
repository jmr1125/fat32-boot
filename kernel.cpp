#include "config.h"
#include "port.h"
#include "stdio.h"
#include "vbe.h"
#include "vesa.h"
extern "C" {
extern void k_main(void);
}
VbeInfoBlock *const vbeInfoBlock = (VbeInfoBlock *)VesaInfoBlockBuffer;
vbe_mode_info_structure *const vbeModeInfo =
    (vbe_mode_info_structure *)VesaModeInfoBlockBuffer;
void k_main() {
  const auto pitch = vbeModeInfo->pitch;
  const auto width = vbeModeInfo->width;
  const auto height = vbeModeInfo->height;
  const auto bpp = vbeModeInfo->bpp;
  char *const lfb = (char *)vbeModeInfo->framebuffer;
  for (int r = 0; r < 256; ++r) {
    for (int x = 0; x < 256; ++x) {
      for (int y = 0; y < 256; ++y) {
        lfb[x * pitch + y * bpp / 8 + 0] = r;
        lfb[x * pitch + y * bpp / 8 + 1] = x;
        lfb[x * pitch + y * bpp / 8 + 2] = y;
      }
    }
  }
}
