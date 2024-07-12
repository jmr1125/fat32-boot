#include "ps2mouse.h"
#include "port.h"
int mouse_dx = 0, mouse_dy = 0;

inline void mouse_wait(byte a_type) // unsigned char
{
  dword _time_out = 100000; // unsigned int
  if (a_type == 0) {
    while (_time_out--) // Data
    {
      if ((inportb(0x64) & 1) == 1) {
        return;
      }
    }
    return;
  } else {
    while (_time_out--) // Signal
    {
      if ((inportb(0x64) & 2) == 0) {
        return;
      }
    }
    return;
  }
}

inline void mouse_write(byte a_write) // unsigned char
{
  // Wait to be able to send a command
  mouse_wait(1);
  // Tell the mouse we are sending a command
  outportb(0x64, 0xD4);
  // Wait for the final part
  mouse_wait(1);
  // Finally write
  outportb(0x60, a_write);
}

byte mouse_read() {
  // Get's response from mouse
  mouse_wait(0);
  return inportb(0x60);
}

void mouse_install() {
  byte _status; // unsigned char

  // Enable the auxiliary mouse device
  mouse_wait(1);
  outportb(0x64, 0xA8);

  // Enable the interrupts
  mouse_wait(1);
  outportb(0x64, 0x20);
  mouse_wait(0);
  _status = (inportb(0x60) | 2);
  mouse_wait(1);
  outportb(0x64, 0x60);
  mouse_wait(1);
  outportb(0x60, _status);

  // Tell the mouse to use default settings
  mouse_write(0xF6);
  mouse_read(); // Acknowledge

  // Enable the mouse
  mouse_write(0xF4);
  mouse_read(); // Acknowledge

  // Setup the mouse handler
}
