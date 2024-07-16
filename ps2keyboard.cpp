#include "ps2keyboard.h"
#include "debug_port0xe9.h"
bool getkey(unsigned char *c) {
  if ((inb(0x64) & 1) == 0) {
    return false;
  }
  *c = inb(0x60);
  return true;
}
const char *get_key_name(key::KEY k) {
  switch (k) {
  case key::unknown:
    return "unknown";
  case key::normal:
    return "normal";
  case key::enter:
    return "enter";
  case key::backspace:
    return "backspace";
  case key::capslock:
    return "capslock";
  case key::space:
    return "space";
  case key::escape:
    return "escape";
  case key::tab:
    return "tab";
  case key::left_control:
    return "left_control";
  case key::left_shift:
    return "left_shift";
  case key::left_alt:
    return "left_alt";
  case key::right_shift:
    return "right_shift";
  case key::numlock:
    return "numlock";
  case key::scroll_lock:
    return "scroll_lock";
    // f1-f12
  case key::f1:
    return "f1";
  case key::f2:
    return "f2";
  case key::f3:
    return "f3";
  case key::f4:
    return "f4";
  case key::f5:
    return "f5";
  case key::f6:
    return "f6";
  case key::f7:
    return "f7";
  case key::f8:
    return "f8";
  case key::f9:
    return "f9";
  case key::f10:
    return "f10";
  case key::f11:
    return "f11";
  case key::f12:
    return "f12";
  default:
    trap();
    return "unknown";
  }
}
void initialize_scancode() {
  for (int i = 0; i < 256; ++i) {
    if (scancode1[i].k == key::unknown) {
      scancode1[i].c = 0;
      scancode1[i].k = key::unknown;
      scancode1[i].pressed = false;
    }
  }
  for (int i = 0; i < 128; ++i) {
    scancode1[i + 0x80].c = scancode1[i].c;
    scancode1[i + 0x80].k = scancode1[i].k;
    scancode1[i + 0x80].pressed = true;
  }
}
key scancode1[256] = {
    [0x00] = {.c = 0, .k = key::unknown, .pressed = false},
    [0x01] = {.c = 0, .k = key::escape, .pressed = false},
    [0x02] = {.c = '1', .k = key::normal, .pressed = false},
    [0x03] = {.c = '2', .k = key::normal, .pressed = false},
    [0x04] = {.c = '3', .k = key::normal, .pressed = false},
    [0x05] = {.c = '4', .k = key::normal, .pressed = false},
    [0x06] = {.c = '5', .k = key::normal, .pressed = false},
    [0x07] = {.c = '6', .k = key::normal, .pressed = false},
    [0x08] = {.c = '7', .k = key::normal, .pressed = false},
    [0x09] = {.c = '8', .k = key::normal, .pressed = false},
    [0x0a] = {.c = '9', .k = key::normal, .pressed = false},
    [0x0b] = {.c = '0', .k = key::normal, .pressed = false},
    [0x0c] = {.c = '-', .k = key::normal, .pressed = false},
    [0x0d] = {.c = '=', .k = key::normal, .pressed = false},
    [0x0e] = {.c = '\b', .k = key::backspace, .pressed = false},
    [0x0f] = {.c = '\t', .k = key::tab, .pressed = false},
    [0x10] = {.c = 'q', .k = key::normal, .pressed = false},
    [0x11] = {.c = 'w', .k = key::normal, .pressed = false},
    [0x12] = {.c = 'e', .k = key::normal, .pressed = false},
    [0x13] = {.c = 'r', .k = key::normal, .pressed = false},
    [0x14] = {.c = 't', .k = key::normal, .pressed = false},
    [0x15] = {.c = 'y', .k = key::normal, .pressed = false},
    [0x16] = {.c = 'u', .k = key::normal, .pressed = false},
    [0x17] = {.c = 'i', .k = key::normal, .pressed = false},
    [0x18] = {.c = 'o', .k = key::normal, .pressed = false},
    [0x19] = {.c = 'p', .k = key::normal, .pressed = false},
    [0x1a] = {.c = '[', .k = key::normal, .pressed = false},
    [0x1b] = {.c = ']', .k = key::normal, .pressed = false},
    [0x1c] = {.c = '\n', .k = key::enter, .pressed = false},
    [0x1d] = {.c = 0, .k = key::left_control, .pressed = false},
    [0x1e] = {.c = 'a', .k = key::normal, .pressed = false},
    [0x1f] = {.c = 's', .k = key::normal, .pressed = false},
    [0x20] = {.c = 'd', .k = key::normal, .pressed = false},
    [0x21] = {.c = 'f', .k = key::normal, .pressed = false},
    [0x22] = {.c = 'g', .k = key::normal, .pressed = false},
    [0x23] = {.c = 'h', .k = key::normal, .pressed = false},
    [0x24] = {.c = 'j', .k = key::normal, .pressed = false},
    [0x25] = {.c = 'k', .k = key::normal, .pressed = false},
    [0x26] = {.c = 'l', .k = key::normal, .pressed = false},
    [0x27] = {.c = ';', .k = key::normal, .pressed = false},
    [0x28] = {.c = '\'', .k = key::normal, .pressed = false},
    [0x29] = {.c = '`', .k = key::normal, .pressed = false},
    [0x2a] = {.c = 0, .k = key::left_shift, .pressed = false},
    [0x2b] = {.c = '\\', .k = key::normal, .pressed = false},
    [0x2c] = {.c = 'z', .k = key::normal, .pressed = false},
    [0x2d] = {.c = 'x', .k = key::normal, .pressed = false},
    [0x2e] = {.c = 'c', .k = key::normal, .pressed = false},
    [0x2f] = {.c = 'v', .k = key::normal, .pressed = false},
    [0x30] = {.c = 'b', .k = key::normal, .pressed = false},
    [0x31] = {.c = 'n', .k = key::normal, .pressed = false},
    [0x32] = {.c = 'm', .k = key::normal, .pressed = false},
    [0x33] = {.c = ',', .k = key::normal, .pressed = false},
    [0x34] = {.c = '.', .k = key::normal, .pressed = false},
    [0x35] = {.c = '/', .k = key::normal, .pressed = false},
    [0x36] = {.c = 0, .k = key::right_shift, .pressed = false},
    [0x37] = {.c = '*', .k = key::normal, .pressed = false},
    [0x38] = {.c = 0, .k = key::left_alt, .pressed = false},
    [0x39] = {.c = ' ', .k = key::space, .pressed = false},
    [0x3a] = {.c = 0, .k = key::capslock, .pressed = false},
    [0x3b] = {.c = 0, .k = key::f1, .pressed = false},
    [0x3c] = {.c = 0, .k = key::f2, .pressed = false},
    [0x3d] = {.c = 0, .k = key::f3, .pressed = false},
    [0x3e] = {.c = 0, .k = key::f4, .pressed = false},
    [0x3f] = {.c = 0, .k = key::f5, .pressed = false},
    [0x40] = {.c = 0, .k = key::f6, .pressed = false},
    [0x41] = {.c = 0, .k = key::f7, .pressed = false},
    [0x42] = {.c = 0, .k = key::f8, .pressed = false},
    [0x43] = {.c = 0, .k = key::f9, .pressed = false},
    [0x44] = {.c = 0, .k = key::f10, .pressed = false},
    [0x45] = {.c = 0, .k = key::numlock, .pressed = false},
    [0x46] = {.c = 0, .k = key::scroll_lock, .pressed = false},
    [0x47] = {.c = '7', .k = key::normal, .pressed = false},
    [0x48] = {.c = '8', .k = key::normal, .pressed = false},
    [0x49] = {.c = '9', .k = key::normal, .pressed = false},
    [0x4a] = {.c = '-', .k = key::normal, .pressed = false},
    [0x4b] = {.c = '4', .k = key::normal, .pressed = false},
    [0x4c] = {.c = '5', .k = key::normal, .pressed = false},
    [0x4d] = {.c = '6', .k = key::normal, .pressed = false},
    [0x4e] = {.c = '+', .k = key::normal, .pressed = false},
    [0x4f] = {.c = '1', .k = key::normal, .pressed = false},
    [0x50] = {.c = '2', .k = key::normal, .pressed = false},
    [0x51] = {.c = '3', .k = key::normal, .pressed = false},
    [0x52] = {.c = '-', .k = key::normal, .pressed = false},
    [0x53] = {.c = '.', .k = key::normal, .pressed = false},
    [0x57] = {.c = 0, .k = key::f11, .pressed = false},
    [0x58] = {.c = 0, .k = key::f12, .pressed = false},
};
