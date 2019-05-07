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

  if (decoding.prev_insn_is_cmp) {
    // fast jcc (optimize for cmp+jcc)
	int invert = cc & 0x1;
	uint32_t relop;
	switch (cc & 0xe) {
		case CC_E:  relop = RELOP_EQ; break;
		case CC_B:  relop = RELOP_LTU; break;
		case CC_BE: relop = RELOP_LEU; break;
		case CC_L:  relop = RELOP_LT; break;
		case CC_LE: relop = RELOP_LE; break;
		default: goto slow_jcc;
	}
	if (invert) relop |= 0x1;
	// TODO: fix cmp(w|b)+jl/jle/jge/jg, must first sign extension cc_dest and cc_src
	rtl_jrelop(relop, &cpu.cc_dest, &cpu.cc_src, decoding.jmp_eip);
  }
  else {
slow_jcc:
    rtl_setcc(&t0, cc);
    rtl_jrelop(RELOP_NE, &t0, &tzero, decoding.jmp_eip);
  }

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
