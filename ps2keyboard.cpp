#include "ps2keyboard.h"
#include "debug_port0xe9.h"
bool getkey(unsigned char *c) {
  if ((inb(0x64) & 1) == 0) {
    return false;
  }
  *c = inb(0x60);
  return true;
}
