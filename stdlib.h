#include "port.h"
template <typename _T> _T min(_T a, _T b) { return a < b ? a : b; }
template <typename _T> _T max(_T a, _T b) { return a > b ? a : b; }

void to_bin(uint32_t a, char *buf);
uint32_t pow(uint32_t a, uint32_t b);
void to_hex(uint32_t a, char *buf);
void to_dec(uint32_t a, char *buf);

// extern "C" {
// void memset(void *buf, char value, uint32_t size);
// void memcpy(void *dst, void *src, uint32_t size);
// }
