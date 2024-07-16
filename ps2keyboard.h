
#define waitACK()                                                              \
  while (inb(0x60) != 0xfa)                                                    \
    ;

bool getkey(unsigned char* c);
