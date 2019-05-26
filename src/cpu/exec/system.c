#include "cpu/exec.h"
// #include "device/port-io.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  rtl_lm(&t0, id_dest->addr, 2);
  rtl_host_sm(&t0, &cpu.idtr.limit, 2);
  rtl_addi(id_dest->addr, id_dest->addr, 2);
  rtl_lm(&t0, id_dest->addr, 4);
  rtl_host_sm(&t0, &cpu.idtr.base, 4);

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  // push eflags, cs, eip
  rtl_host_lm(&t0, &cpu.eflags, 4);
  rtl_push(&t0);
  rtl_host_lm(&t0, &cpu.cs, 2);
  rtl_push(&t0);
  // rtl_host_lm(&t0, eip, 4);
  rtl_li(&t0, *eip);
  rtl_push(&t0);

  // IDT descriptor's address = idtr.base + (n << 3)
  uint8_t n = id_dest->imm;
  rtl_host_lm(&t0, &cpu.idtr.base, 4);
  rtl_addi(&t0, &t0, n << 3);

  // load IDT descriptor into t1 and t2
  rtl_lm(&t1, &t0, 4);
  rtl_addi(&t0, &t0, 4);
  rtl_lm(&t2, &t0, 4);

  // get pointer to interrupt handler
  // (t1 & 0xffff) | (t2 & 0xffff0000)
  rtl_andi(&t1, &t1, 0xffff);
  rtl_andi(&t2, &t2, 0xffff0000);
  rtl_or(&t1, &t1, &t2);
  rtl_jr(&t1);
  
  decoding.is_control = true;

  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST)
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  // pop eip, cs, eflags
  rtl_pop(&t0);
  rtl_jr(&t0);
  rtl_pop(&t0);
  rtl_host_sm(&t0, &cpu.cs, 2);
  rtl_pop(&t0);
  rtl_host_sm(&t0, &cpu.eflags, 4);

  decoding.is_control = true;

  print_asm("iret");
}

// uint32_t pio_read_l(ioaddr_t);
// uint32_t pio_read_w(ioaddr_t);
// uint32_t pio_read_b(ioaddr_t);
// void pio_write_l(ioaddr_t, uint32_t);
// void pio_write_w(ioaddr_t, uint32_t);
// void pio_write_b(ioaddr_t, uint32_t);

extern bool diff_test_fix;

make_EHelper(in) {
  rtl_pio_read(&t0, id_src->val, id_dest->width);
  operand_write(id_dest, &t0);

#if defined(DIFF_TEST)
  diff_test_skip_qemu();
#endif

  print_asm_template2(in);
}

make_EHelper(out) {
  rtl_pio_write(id_dest->val, id_src->val, id_src->width);

#if defined(DIFF_TEST)
  diff_test_skip_qemu();
#endif

  print_asm_template2(out);
}

make_EHelper(sti) {
  cpu.eflags.IF = 1;

  print_asm("sti");
}
