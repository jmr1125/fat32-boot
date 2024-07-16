extern unsigned char font[256][16];
bool font_get(int c, int x, int y);
void draw(int c, int X, int Y, char *lfb, int pitch, int bpp);
void draws(const char *s, int X, int Y, char *lfb, int pitch, int bpp);
