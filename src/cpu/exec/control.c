#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(decoding.jmp_eip);

  decoding.is_control = true;
  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = decoding.opcode & 0xf;
  rtl_setcc(&t0, cc);
  rtl_jrelop(RELOP_NE, &t0, &tzero, decoding.jmp_eip);

  decoding.is_control = true;
  print_asm("j%s %x", get_cc_name(cc), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  rtl_jr(id_dest->val);

  decoding.is_control = true;
  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  // rtl_push(&decoding.seq_eip);
  rtl_li(&t0, decoding.seq_eip);
  rtl_push(&t0);
  rtl_j(decoding.jmp_eip);

  decoding.is_control = true;
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  rtl_pop(&t0);
  rtl_jr(&t0);

  decoding.is_control = true;
  print_asm("ret");
}

make_EHelper(ret_i) {
  rtl_pop(&t0);
  rtl_jr(&t0);
  rtl_add(&cpu.esp, &cpu.esp, id_dest->val);

  decoding.is_control = true;
  print_asm("ret");
}

make_EHelper(call_rm) {
  rtl_li(&t0, decoding.seq_eip);
  rtl_push(&t0);
  rtl_jr(id_dest->val);

  decoding.is_control = true;
  print_asm("call *%s", id_dest->str);
}
