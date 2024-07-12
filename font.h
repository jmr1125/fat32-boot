extern unsigned char font[256][16];
inline bool font_get(int c, int x, int y) {
  return font[c][x] & (1 << (7 - y));
}
inline void draw(int c, int X, int Y, char *lfb, int pitch, int bpp, int size) {
  for (int x = 0; x < 16; ++x) {
    for (int y = 0; y < 8; ++y) {
      for (int dx = 0; dx < size; ++dx)
        for (int dy = 0; dy < size; ++dy) {
	  int base=((X+x)*size+dx)*pitch+((Y+y)*size+dy)*bpp/8;
          *(lfb + base + 0) = font_get(c, x, y) ? 0xFF : 0;
          *(lfb + base + 1) = font_get(c, x, y) ? 0xFF : 0;
          *(lfb + base + 2) = font_get(c, x, y) ? 0xFF : 0;
          // y * size + dy + X, x * size + dx + Y,
          // font_get(C, x, y)}
        }
    }
  }
}
