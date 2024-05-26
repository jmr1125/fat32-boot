#pragma once
#include "port.h"
#if __cplusplus < 201103L
#define constexpr
#endif
// Define PCI configuration addresses
constexpr uint16_t PCI_CONFIG_VENDOR_ID = 0x00;
constexpr uint16_t PCI_CONFIG_DEVICE_ID = 0x02;
constexpr uint16_t PCI_CONFIG_COMMAND = 0x04;
constexpr uint16_t PCI_CONFIG_BAR0 = 0x10;

uint16_t pciConfigReadWord(uint8_t, uint8_t, uint8_t, uint8_t);
void pciConfigWriteByte(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

uint32_t readPCIConfigRegister(uint8_t bus, uint8_t device, uint8_t function,
                               uint16_t offset);
void writePCIConfigRegister(uint8_t bus, uint8_t device, uint8_t function,
                            uint16_t offset, uint32_t value);
