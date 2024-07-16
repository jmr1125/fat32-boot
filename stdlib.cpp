#include "stdlib.h"
#include "debug_port0xe9.h"
void to_bin(uint32_t a, char *buf) {
  for (int i = 0; i < 32; ++i) {
    buf[31 - i] = (a & (1 << i)) % 2 + '0';
  }
  buf[31] = '\0';
}
void to_hex(uint32_t a, char *buf) {
  for (int i = 0; i < 8; ++i) {
    buf[7 - i] = "0123456789abcdef"[((a & (0xf << (i * 4))) >> (i * 4)) & 0xf];
  }
  buf[8] = '\0';
}
uint32_t pow(uint32_t a, uint32_t b) {
  int A = a, B = b;
  uint32_t t = a, r = 1;
  while (b) {
    if (b % 2) {
      r *= t;
    }
    t *= t;
    b >>= 1;
  }
  return r;
}
void to_dec(uint32_t a, char *buf) {
  int pow10[9];
  pow10[0] = 1;
  pow10[1] = 10;
  pow10[2] = 100;
  pow10[3] = 1000;
  pow10[4] = 10000;
  pow10[5] = 100000;
  pow10[6] = 1000000;
  pow10[7] = 10000000;
  pow10[8] = 100000000;
  for (int i = 0; i < 8; ++i) {
    buf[7 - i] = '0' + (a / pow10[i]) % 10;
  }
  buf[8] = '\0';
}

// void memset(void *buf, char value, uint32_t size) {
//   for (int i = 0; i < size; ++i) {
//     ((char *)buf)[i] = value;
//   }
// }
// void memcpy(void *dst, const void *src, uint32_t size) {
//   for (int i = 0; i < size; ++i) {
//     ((char *)dst)[i] = ((char *)src)[i];
//   }
// }
