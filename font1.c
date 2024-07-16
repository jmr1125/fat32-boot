#include "debug_port0xe9.h"
#include "font.h"
bool font_get(int c, int x, int y) { return font[c][x] & (1 << (7 - y)); }
void draw(int c, int X, int Y, char *lfb, int pitch, int bpp) {
  for (int x = 0; x < 16; ++x) {
    for (int y = 0; y < 8; ++y) {
      int base = (X + x) * pitch + (Y + y) * bpp / 8;
      *(lfb + base + 0) = font_get(c, x, y) ? 0xFF : 0;
      *(lfb + base + 1) = font_get(c, x, y) ? 0xFF : 0;
      *(lfb + base + 2) = font_get(c, x, y) ? 0xFF : 0;
    }
  }
}

void draws(const char *s, int X, int Y, char *lfb, int pitch, int bpp) {
  for (; *s; ++s) {
    /* print("draw at "); */
    /* print_hex(X); */
    /* print("  ;  "); */
    /* print_hex(Y); */
    /* print("  ("); */
    /* char a[2]; */
    /* a[0] = *s; */
    /* a[1] = 0; */
    /* print(a); */
    /* print(")\n"); */
    draw(*s, X, Y, lfb, pitch, bpp);
    Y += 8;
  }
}
