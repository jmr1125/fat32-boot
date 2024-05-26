#include "pci.h"
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func,
                           uint8_t offset) {
  uint32_t address;
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot; // device
  uint32_t lfunc = (uint32_t)func;
  uint16_t tmp = 0;

  // Create configuration address as per Figure 1
  address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                       (offset & 0xFC) | ((uint32_t)0x80000000));

  // Write out the address
  outb(0xCF8, address);
  // Read in the data
  // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
  tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
  return tmp;
}

void pciConfigWriteByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset,
                        uint8_t val) {
  uint32_t address;
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot; // device
  uint32_t lfunc = (uint32_t)func;
  uint16_t tmp = 0;

  // Create configuration address as per Figure 1
  address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                       /* (offset & 0xFC)*/ offset | ((uint32_t)0x80000000));

  // Write out the address
  outl(0xCF8, address);
  // Read in the data
  // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
  // tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
  outl(0xcfc, val);
}

// Function to read a 32-bit value from a PCI configuration register
uint32_t readPCIConfigRegister(uint8_t bus, uint8_t device, uint8_t function,
                               uint16_t offset) {
  // Ensure the two least significant bits are cleared for the offset
  offset &= 0xFC;

  uint32_t address =
      (1u << 31) | (bus << 16) | (device << 11) | (function << 8) | offset;
  asm volatile("outl %0, %%dx" : : "a"(address), "d"(0xcf8));

  uint32_t value;
  asm volatile("inl %%dx, %0" : "=a"(value) : "d"(0xcfc));
  return value;
}
void writePCIConfigRegister(uint8_t bus, uint8_t device, uint8_t function,
                            uint16_t offset, uint32_t value) {
  // Ensure the two least significant bits are cleared for the offset
  offset &= 0xFC;

  uint32_t address =
      (1u << 31) | (bus << 16) | (device << 11) | (function << 8) | offset;
  asm volatile("outl %0, %%dx" : : "a"(address), "d"(0xcf8));

  asm volatile("outl %0, %%dx" : : "a"(value), "d"(0xcfc));
}
