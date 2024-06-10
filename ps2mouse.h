#include "port.h"
#define MOUSE_READ_PORT 0x60
#define MOUSE_WRITE_PORT 0x64
void mouse_wait(byte);
void mouse_write(byte);
byte mouse_read();
void mouse_install();
