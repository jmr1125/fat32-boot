#include "vbe.h"

USHORT vbe_read(USHORT index) {
  WRITE_PORT_USHORT(VBE_DISPI_IOPORT_INDEX, index);
  return READ_PORT_USHORT(VBE_DISPI_IOPORT_DATA);
}
void vbe_write(USHORT index, USHORT value) {
  WRITE_PORT_USHORT(VBE_DISPI_IOPORT_INDEX, index);
  WRITE_PORT_USHORT(VBE_DISPI_IOPORT_DATA, value);
}

void vbe_set(USHORT xres, USHORT yres, USHORT bpp) {
  vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
  vbe_write(VBE_DISPI_INDEX_XRES, xres);
  vbe_write(VBE_DISPI_INDEX_YRES, yres);
  vbe_write(VBE_DISPI_INDEX_BPP, bpp);
  vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}



// Function to enumerate PCI devices and read the linear framebuffer address
uint32_t enumGetLFBaddr(uint8_t *BUS, uint8_t *DEVICE, uint8_t *FUNCTION) {
  for (uint8_t bus = 0; bus < 255; ++bus) {
    for (uint8_t device = 0; device < 32; ++device) {
      for (uint8_t function = 0; function < 8; ++function) {
        // Read vendor ID to check if a device exists
        uint16_t vendorID =
            readPCIConfigRegister(bus, device, function, PCI_CONFIG_VENDOR_ID);
        if (vendorID == 0xFFFF) {
          // No device present, move to the next device
          continue;
        }

        // Read device ID
        uint16_t deviceID = readPCIConfigRegister(bus, device, function,
                                                  PCI_CONFIG_DEVICE_ID) >>
                            16;

        // Check if it's the Bochs BGA device
        if (vendorID == BOCHS_VENDOR_ID && deviceID == BOCHS_DEVICE_ID) {
          // disable both I/O and memory decode
          uint32_t command =
              readPCIConfigRegister(bus, device, function, PCI_CONFIG_COMMAND);
          writePCIConfigRegister(bus, device, function, PCI_CONFIG_COMMAND,
                                 command & 0b1111111111111100);
          // Read the linear framebuffer address from BAR 0
          uint32_t framebufferAddress =
              readPCIConfigRegister(bus, device, function, PCI_CONFIG_BAR0);

          writePCIConfigRegister(bus, device, function, PCI_CONFIG_COMMAND,
                                 command);
          framebufferAddress &= 0xfffffffffffffff0; // 0xfd000000
          // framebufferAddress &= 0xffffff8000000000UL;
          //   Output the result
          *BUS = bus, *DEVICE = device, *FUNCTION = function;
          return framebufferAddress;

          // You can add further logic or exit the loop if needed
        }
      }
    }
  }
  return 0xe0000000;
}
void draw_pixel(unsigned x, unsigned y, unsigned color, unsigned *vga) {
  unsigned pitch = (800 * 32 / 8);
  unsigned stride = (32 / 8);
  unsigned offset = (y * pitch) + (x * stride);
  vga[offset / 4] = color;
}


/* k_clear_screen : to clear the entire text screen */
void k_clear_screen() {
  char *vidmem = (char *)0xb8000;
  unsigned int i = 0;
  while (i < (80 * 25 * 2)) {
    vidmem[i] = ' ';
    i++;
    vidmem[i] = WHITE_TXT;
    i++;
  };
};

/* k_printf : the message and the line # */
unsigned int k_printf(const char *message, unsigned int line) {
  char *vidmem = (char *)0xb8000;
  unsigned int i = 0;

  i = (line * 80 * 2);

  while (*message != 0) {
    if (*message == '\n') // check for a new line
    {
      line++;
      i = (line * 80 * 2);
      //*message++;
      message++;
    } else {
      vidmem[i] = *message;
      //*message++;
      message++;
      i++;
      vidmem[i] = WHITE_TXT;
      i++;
    };
  };

  return (1);
}
