extern "C" {
extern void exit_v86(void);
extern void enter_v86(uint16_t cs_seg, uint16_t ip, uint16_t ss_seg,
                      uint16_t esp, uint32_t flags);
}
