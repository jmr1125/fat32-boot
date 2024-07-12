#define def_int(x) extern void int_##x()
#define def_irq(x) extern void irq_##x()
extern "C" {
def_int(0);
def_int(1);
def_int(2);
def_int(3);
def_int(4);
def_int(5);
def_int(6);
def_int(7);
def_int(8);
def_int(9);
def_int(10);
def_int(11);
def_int(12);
def_int(13);
def_int(14);
def_int(15);
def_int(16);
def_int(17);
def_int(18);
def_int(19);
def_irq(0);
def_irq(1);
def_irq(2);
def_irq(3);
def_irq(4);
def_irq(5);
def_irq(6);
def_irq(7);
def_irq(8);
def_irq(9);
def_irq(10);
def_irq(11);
def_irq(12);
def_irq(13);
def_irq(14);
def_irq(15);
def_irq(16);
}
