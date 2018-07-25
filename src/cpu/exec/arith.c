#include "cpu/exec.h"

make_EHelper(add) {
  // save old destination operand
  rtl_mv(&t3, id_dest->val);
  rtl_add(id_dest->val, id_dest->val, id_src->val);
  operand_write(id_dest, id_dest->val);
  rtl_cc_set_op(MAKE_CC_OP(CC_OP_ADD, id_dest->width), 
		  id_dest->val, &t3, id_src->val);
  
//  rtl_update_ZFSF(id_dest->val, id_dest->width);
//  
//  // CF <- (result < dest)
//  rtl_setrelop(RELOP_LTU, &t1, id_dest->val, &t3);
//  rtl_set_CF(&t1);
//  
//  // OF <- (MSB[src] == MSB[dest] && MSB[result] != MSB[dest])
//  rtl_xor(&t1, &t3, id_src->val);
//  rtl_not(&t1);
//  rtl_xor(&t2, &t3, &t0);
//  rtl_and(&t1, &t1, &t2);
//  rtl_msb(&t1, &t1, id_dest->width);
//  rtl_set_OF(&t1);

  print_asm_template2(add);
}

make_EHelper(sub) {
  // save old destination operand
  rtl_mv(&t3, id_dest->val);
  rtl_sub(id_dest->val, id_dest->val, id_src->val);
  operand_write(id_dest, id_dest->val);
  rtl_cc_set_op(MAKE_CC_OP(CC_OP_SUB, id_dest->width),
		  id_dest->val, &t3, id_src->val);

//  rtl_update_ZFSF(id_dest->val, id_dest->width);
//
//  // CF <- (dest < src)
//  rtl_setrelop(RELOP_LTU, &t1, &t3, id_src->val);
//  rtl_set_CF(&t1);
//
//  // OF <- (MSB[src] != MSB[dest] && MSB[result] == MSB[src])
//  rtl_xor(&t1, &t3, id_src->val);
//  rtl_xor(&t2, id_src->val, id_dest->val);
//  rtl_not(&t2);
//  rtl_and(&t1, &t1, &t2);
//  rtl_msb(&t1, &t1, id_dest->width);
//  rtl_set_OF(&t1);

  print_asm_template2(sub);
}

make_EHelper(cmp) {
  rtl_sub(&t0, id_dest->val, id_src->val);
  rtl_cc_set_op(MAKE_CC_OP(CC_OP_SUB, id_dest->width), 
		  &t0, id_dest->val, id_src->val);

//  rtl_update_ZFSF(&t0, id_dest->width);
//
//  // CF <- (dest < src)
//  rtl_setrelop(RELOP_LTU, &t1, id_dest->val, id_src->val);
//  rtl_set_CF(&t1);
//
//  // OF <- (MSB[src] != MSB[dest] && MSB[result] == MSB[src])
//  rtl_xor(&t1, id_dest->val, id_src->val);
//  rtl_xor(&t2, id_src->val, &t0);
//  rtl_not(&t2);
//  rtl_and(&t3, &t1, &t2);
//  rtl_msb(&t3, &t3, id_dest->width);
//  rtl_set_OF(&t3);

  print_asm_template2(cmp);
}

make_EHelper(inc) {
  rtl_addi(id_dest->val, id_dest->val, 1);
  operand_write(id_dest, id_dest->val);
  rtl_cc_set_op(MAKE_CC_OP(CC_OP_INC, id_dest->width), 
		  id_dest->val, &tzero, &tzero);

//  rtl_update_ZFSF(id_dest->val, id_dest->width);
  // TODO update OF

  print_asm_template1(inc);
}

make_EHelper(dec) {
  rtl_subi(id_dest->val, id_dest->val, 1);
  operand_write(id_dest, id_dest->val);
  rtl_cc_set_op(MAKE_CC_OP(CC_OP_DEC, id_dest->width), 
		  id_dest->val, &tzero, &tzero);

//  rtl_update_ZFSF(id_dest->val, id_dest->width);
  // TODO update OF

  print_asm_template1(dec);
}

make_EHelper(neg) {
//  rtl_setrelop(RELOP_EQ, &t1, id_dest->val, &tzero);
//  rtl_not(&t1);
//  rtl_set_CF(&t1);

  rtl_sub(id_dest->val, &tzero, id_dest->val);
  operand_write(id_dest, id_dest->val);

//  rtl_update_ZFSF(id_dest->val, id_dest->width);

  print_asm_template1(neg);
}

make_EHelper(adc) {
  rtl_add(&t2, id_dest->val, id_src->val);
//  rtl_setrelop(RELOP_LTU, &t3, &t2, id_dest->val);
  rtl_get_CF(&t1);
  rtl_add(&t2, &t2, &t1);

//  rtl_update_ZFSF(&t2, id_dest->width);

//  rtl_setrelop(RELOP_LTU, &t0, &t2, id_dest->val);
//  rtl_or(&t0, &t3, &t0);
//  rtl_set_CF(&t0);
//
//  rtl_xor(&t0, id_dest->val, id_src->val);
//  rtl_not(&t0);
//  rtl_xor(&t1, id_dest->val, &t2);
//  rtl_and(&t0, &t0, &t1);
//  rtl_msb(&t0, &t0, id_dest->width);
//  rtl_set_OF(&t0);

  operand_write(id_dest, &t2);

  print_asm_template2(adc);
}

make_EHelper(sbb) {
  rtl_sub(&t2, id_dest->val, id_src->val);
//  rtl_setrelop(RELOP_LTU, &t3, id_dest->val, &t2);
  rtl_get_CF(&t1);
  rtl_sub(&t2, &t2, &t1);

//  rtl_update_ZFSF(&t2, id_dest->width);
//
//  rtl_setrelop(RELOP_LTU, &t0, id_dest->val, &t2);
//  rtl_or(&t0, &t3, &t0);
//  rtl_set_CF(&t0);
//
//  rtl_xor(&t0, id_dest->val, id_src->val);
//  rtl_xor(&t1, id_dest->val, &t2);
//  rtl_and(&t0, &t0, &t1);
//  rtl_msb(&t0, &t0, id_dest->width);
//  rtl_set_OF(&t0);

  operand_write(id_dest, &t2);

  print_asm_template2(sbb);
}

make_EHelper(mul) {
  rtl_lr(&id_src->val, R_EAX, id_dest->width);
  rtl_mul_lo(&t1, id_dest->val, id_src->val);

  switch (id_dest->width) {
    case 1:
      rtl_sr_w(R_AX, &t1);
      break;
    case 2:
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      break;
    case 4:
      rtl_mul_hi(&t2, id_dest->val, id_src->val);
      rtl_sr_l(R_EDX, &t2);
      rtl_sr_l(R_EAX, &t1);
      break;
    default: assert(0);
  }

  print_asm_template1(mul);
}

// imul with one operand
make_EHelper(imul1) {
  rtl_lr(&id_src->val, R_EAX, id_dest->width);
  rtl_imul_lo(&t1, id_dest->val, id_src->val);

  switch (id_dest->width) {
    case 1:
      rtl_sr_w(R_AX, &t1);
      break;
    case 2:
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      break;
    case 4:
      rtl_imul_hi(&t2, id_dest->val, id_src->val);
      rtl_sr_l(R_EDX, &t2);
      rtl_sr_l(R_EAX, &t1);
      break;
    default: assert(0);
  }

  print_asm_template1(imul);
}

// imul with two operands
make_EHelper(imul2) {
  rtl_sext(id_src->val, id_src->val, id_src->width);
  rtl_sext(id_dest->val, id_dest->val, id_dest->width);

  rtl_imul_lo(&t1, id_dest->val, id_src->val);
  operand_write(id_dest, &t1);

  print_asm_template2(imul);
}

// imul with three operands
make_EHelper(imul3) {
  rtl_sext(id_src->val, id_src->val, id_src->width);
  rtl_sext(id_src2->val, id_src2->val, id_src->width);
  rtl_sext(id_dest->val, id_dest->val, id_dest->width);

  rtl_imul_lo(&t1, id_src2->val, id_src->val);
  operand_write(id_dest, &t1);

  print_asm_template3(imul);
}

make_EHelper(div) {
  switch (id_dest->width) {
    case 1:
      rtl_lr_w(&t0, R_AX);
      rtl_div_q(&t2, &t0, id_dest->val);
      rtl_div_r(&t3, &t0, id_dest->val);
      rtl_sr_b(R_AL, &t2);
      rtl_sr_b(R_AH, &t3);
      break;
    case 2:
      rtl_lr_w(&t0, R_AX);
      rtl_lr_w(&t1, R_DX);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_div_q(&t2, &t0, id_dest->val);
      rtl_div_r(&t3, &t0, id_dest->val);
      rtl_sr_w(R_AX, &t2);
      rtl_sr_w(R_DX, &t3);
      break;
    case 4:
      rtl_lr_l(&t0, R_EAX);
      rtl_lr_l(&t1, R_EDX);
      rtl_div64_q(&cpu.eax, &t1, &t0, id_dest->val);
      rtl_div64_r(&cpu.edx, &t1, &t0, id_dest->val);
      break;
    default: assert(0);
  }

  print_asm_template1(div);
}

make_EHelper(idiv) {
  switch (id_dest->width) {
    case 1:
      rtl_lr_w(&t0, R_AX);
      rtl_idiv_q(&t2, &t0, id_dest->val);
      rtl_idiv_r(&t3, &t0, id_dest->val);
      rtl_sr_b(R_AL, &t2);
      rtl_sr_b(R_AH, &t3);
      break;
    case 2:
      rtl_lr_w(&t0, R_AX);
      rtl_lr_w(&t1, R_DX);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_idiv_q(&t2, &t0, id_dest->val);
      rtl_idiv_r(&t3, &t0, id_dest->val);
      rtl_sr_w(R_AX, &t2);
      rtl_sr_w(R_DX, &t3);
      break;
    case 4:
      rtl_lr_l(&t0, R_EAX);
      rtl_lr_l(&t1, R_EDX);
      rtl_idiv64_q(&cpu.eax, &t1, &t0, id_dest->val);
      rtl_idiv64_r(&cpu.edx, &t1, &t0, id_dest->val);
      break;
    default: assert(0);
  }

  print_asm_template1(idiv);
}
