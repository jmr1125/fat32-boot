#pragma once
#include "port.h"
#include "pci.h"
/*VBE*/
#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF
#define VBE_DISPI_INDEX_ID 0x0
#define VBE_DISPI_INDEX_XRES 0x1
#define VBE_DISPI_INDEX_YRES 0x2
#define VBE_DISPI_INDEX_BPP 0x3
#define VBE_DISPI_INDEX_ENABLE 0x4
#define VBE_DISPI_INDEX_BANK 0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH 0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 0x7
#define VBE_DISPI_INDEX_X_OFFSET 0x8
#define VBE_DISPI_INDEX_Y_OFFSET 0x9
#define VBE_DISPI_DISABLED 0x00
#define VBE_DISPI_ENABLED 0x01
#define VBE_DISPI_GETCAPS 0x02
#define VBE_DISPI_8BIT_DAC 0x20
#define VBE_DISPI_LFB_ENABLED 0x40
#define VBE_DISPI_NOCLEARMEM 0x80
USHORT vbe_read(USHORT, USHORT);
void vbe_write(USHORT, USHORT);
void vbe_set(USHORT, USHORT, USHORT);

// Define Bochs BGA device IDs
constexpr uint16_t BOCHS_VENDOR_ID = 0x1234;
constexpr uint16_t BOCHS_DEVICE_ID = 0x1111;
uint32_t enumGetLFBaddr(uint8_t *BUS, uint8_t *DEVICE, uint8_t *FUNCTION);
void draw_pixel(unsigned x, unsigned y, unsigned color, unsigned *vga);

#define WHITE_TXT 0x07 /* light gray on black text */
void k_clear_screen();
unsigned int k_printf(const char *message, unsigned int line);
