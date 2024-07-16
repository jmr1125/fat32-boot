
#define waitACK()                                                              \
  while (inb(0x60) != 0xfa)                                                    \
    ;

bool getkey(unsigned char *c);
void initialize_scancode();

struct key {
  enum KEY {
    unknown = 0,
    normal,
    escape,
    backspace,
    tab,
    enter,
    left_control,
    left_shift,
    right_shift,
    left_alt,
    space,
    capslock,
    f1,
    f2,
    f3,
    f4,
    f5,
    f6,
    f7,
    f8,
    f9,
    f10,
    numlock,
    scroll_lock,
    left,
    up,
    right,
    down,
    insert,
    Delete,
    end,
    page_up,
    page_down,
    f11,
    f12
  };
  unsigned char c;
  KEY k;
  bool pressed;
};
extern key scancode1[256];

const char *get_key_name(key::KEY k);
