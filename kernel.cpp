//asm(".code16\n");
extern "C" {
extern void k_main(void);
}
void putchar(char c, char a, char *off) {
  *(off) = c;
  *(off + 1) = a;
}
void put(char *str, char attr, char *off) {
  while (*str != 0) {
    putchar(*str, attr, off);
    str++;
    off += 2;
  }
}
void k_main() {
  put("hello from kernel!!!", 0xff, (char *)0xb8000);
}
