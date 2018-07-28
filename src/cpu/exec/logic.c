#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
  rtl_and(&t0, id_src->val, id_dest->val);
  rtl_cc_set_op(MAKE_CC_OP(CC_OP_AND, id_dest->width), 
		  &t0, id_dest->val, id_src->val);

//  rtl_set_CF(&tzero);
//  rtl_set_OF(&tzero);
//  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(id_dest->val, id_src->val, id_dest->val);
  operand_write(id_dest, id_dest->val);
  rtl_cc_set_op(MAKE_CC_OP(CC_OP_AND, id_dest->width), 
		  id_dest->val, &tzero, &tzero);

//  rtl_set_CF(&tzero);
//  rtl_set_OF(&tzero);
//  rtl_update_ZFSF(id_dest->val, id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(id_dest->val, id_src->val, id_dest->val);
  operand_write(id_dest, id_dest->val);
  rtl_cc_set_op(MAKE_CC_OP(CC_OP_XOR, id_dest->width), 
		  id_dest->val, &tzero, &tzero);

//  rtl_set_CF(&tzero);
//  rtl_set_OF(&tzero);
//  rtl_update_ZFSF(id_dest->val, id_dest->width);

  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(id_dest->val, id_src->val, id_dest->val);
  operand_write(id_dest, id_dest->val);
  rtl_cc_set_op(MAKE_CC_OP(CC_OP_OR, id_dest->width), 
		  id_dest->val, &tzero, &tzero);

//  rtl_set_CF(&tzero);
//  rtl_set_OF(&tzero);
//  rtl_update_ZFSF(id_dest->val, id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
  rtl_sar(id_dest->val, id_dest->val, id_src->val);
  operand_write(id_dest, id_dest->val);
//  rtl_update_ZFSF(id_dest->val, id_dest->width);
  
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(id_dest->val, id_dest->val, id_src->val);
  operand_write(id_dest, id_dest->val);
//  rtl_update_ZFSF(id_dest->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(id_dest->val, id_dest->val, id_src->val);
  operand_write(id_dest, id_dest->val);
//  rtl_update_ZFSF(id_dest->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;
  rtl_setcc(&t2, cc);

  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  rtl_not(id_dest->val);
  operand_write(id_dest, id_dest->val);

  print_asm_template1(not);
}

make_EHelper(rol) {
  // dest <- (dest << src) | (dest >> (32 -src))
  rtl_shl(&t0, id_dest->val, id_src->val);
  rtl_li(&t1, 32);
  rtl_sub(&t1, &t1, id_src->val); // t1 <- (32-src)
  rtl_shr(&t1, id_dest->val, &t1);
  rtl_or(id_dest->val, &t0, &t1);
  operand_write(id_dest, id_dest->val);

  // unnecessary to update CF and OF in NEMU

  print_asm_template2(rol);
}
