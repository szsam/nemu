#include <stdlib.h>

#include "cpu/rtl.h"
#include "cpu/tb.h"

/* RTL basic instructions */

#define make_generate_rtl_prologue(tag) \
	RTLInstr *rtl = (RTLInstr *)malloc(sizeof(RTLInstr)); \
	rtl->type = tag;

#define make_generate_rtl_epilogue() \
	list_add_tail(&rtl->list, &cur_tblock->rtl_instr_list.list);

#define copy_param(field_name, arg) rtl->field_name.arg = arg;

void generate_rtl_j(vaddr_t target) {
	make_generate_rtl_prologue(J)
	copy_param(j, target)
	make_generate_rtl_epilogue()
}

void generate_rtl_jr(rtlreg_t *target) {
	make_generate_rtl_prologue(JR)
	copy_param(jr, target)
	make_generate_rtl_epilogue()
}

void generate_rtl_jrelop(uint32_t relop,
    const rtlreg_t *src1, const rtlreg_t *src2, vaddr_t target) {
	make_generate_rtl_prologue(JRELOP)
	copy_param(jrelop, relop)
	copy_param(jrelop, src1)
	copy_param(jrelop, src2)
	copy_param(jrelop, target)
	make_generate_rtl_epilogue()
}

void generate_rtl_setrelop(uint32_t relop, rtlreg_t *dest,
    const rtlreg_t *src1, const rtlreg_t *src2) {

	make_generate_rtl_prologue(SETRELOP)
	copy_param(setrelop, relop)
	copy_param(setrelop, dest)
	copy_param(setrelop, src1)
	copy_param(setrelop, src2)
	make_generate_rtl_epilogue()
}

void generate_rtl_exit(int state) {
	make_generate_rtl_prologue(EXIT)
	copy_param(exit, state)
	make_generate_rtl_epilogue()
}

void generate_rtl_li(rtlreg_t* dest, uint32_t imm) {
	make_generate_rtl_prologue(LI)
	copy_param(li, dest)
	copy_param(li, imm)
	make_generate_rtl_epilogue()
}

#define make_rtl_arith_logic_body(name, tag) \
  void concat(generate_rtl_, name) (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
	make_generate_rtl_prologue(tag) \
	copy_param(alu, dest) \
	copy_param(alu, src1) \
	copy_param(alu, src2) \
	make_generate_rtl_epilogue() \
  } \
  void concat3(generate_rtl_, name, i) (rtlreg_t* dest, const rtlreg_t* src1, int imm) { \
	make_generate_rtl_prologue(concat(tag, I)) \
	copy_param(alui, dest) \
	copy_param(alui, src1) \
	copy_param(alui, imm) \
	make_generate_rtl_epilogue() \
  }

make_rtl_arith_logic_body(add, ADD)
make_rtl_arith_logic_body(sub, SUB)
make_rtl_arith_logic_body(and, AND)
make_rtl_arith_logic_body(or, OR)
make_rtl_arith_logic_body(xor, XOR)
make_rtl_arith_logic_body(shl, SHL)
make_rtl_arith_logic_body(shr, SHR)
make_rtl_arith_logic_body(sar, SAR)
make_rtl_arith_logic_body(mul_lo, MUL_LO)
make_rtl_arith_logic_body(mul_hi, MUL_HI)
make_rtl_arith_logic_body(imul_lo, IMUL_LO)
make_rtl_arith_logic_body(imul_hi, IMUL_HI)
make_rtl_arith_logic_body(div_q, DIV_Q)
make_rtl_arith_logic_body(div_r, DIV_R)
make_rtl_arith_logic_body(idiv_q, IDIV_Q)
make_rtl_arith_logic_body(idiv_r, IDIV_R)

#define make_rtl_div64(instr, tag) \
void concat(generate_rtl_, instr) (rtlreg_t* dest, \
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) { \
	make_generate_rtl_prologue(tag) \
	copy_param(div64, dest) \
	copy_param(div64, src1_hi) \
	copy_param(div64, src1_lo) \
	copy_param(div64, src2) \
	make_generate_rtl_epilogue() \
}

make_rtl_div64(div64_q, DIV64_Q)
make_rtl_div64(div64_r, DIV64_R)
make_rtl_div64(idiv64_q, IDIV64_Q)
make_rtl_div64(idiv64_r, IDIV64_R)


void generate_rtl_lm(rtlreg_t *dest, const rtlreg_t* addr, int len) {
	make_generate_rtl_prologue(LM)
	copy_param(lm, dest)
	copy_param(lm, addr)
	copy_param(lm, len)
	make_generate_rtl_epilogue()
}

void generate_rtl_sm(const rtlreg_t* addr, int len, const rtlreg_t* src1) {
	make_generate_rtl_prologue(SM)
	copy_param(sm, addr)
	copy_param(sm, len)
	copy_param(sm, src1)
	make_generate_rtl_epilogue()
}

void generate_rtl_lr_l(rtlreg_t* dest, int r) {
	make_generate_rtl_prologue(LR_L)
	copy_param(lr, dest)
	copy_param(lr, r)
	make_generate_rtl_epilogue()
}

void generate_rtl_lr_w(rtlreg_t* dest, int r) {
	make_generate_rtl_prologue(LR_W)
	copy_param(lr, dest)
	copy_param(lr, r)
	make_generate_rtl_epilogue()
}

void generate_rtl_lr_b(rtlreg_t* dest, int r) {
	make_generate_rtl_prologue(LR_B)
	copy_param(lr, dest)
	copy_param(lr, r)
	make_generate_rtl_epilogue()
}

void generate_rtl_sr_l(int r, const rtlreg_t* src1) {
	make_generate_rtl_prologue(SR_L)
	copy_param(sr, r)
	copy_param(sr, src1)
	make_generate_rtl_epilogue()
}

void generate_rtl_sr_w(int r, const rtlreg_t* src1) {
	make_generate_rtl_prologue(SR_W)
	copy_param(sr, r)
	copy_param(sr, src1)
	make_generate_rtl_epilogue()
}

void generate_rtl_sr_b(int r, const rtlreg_t* src1) {
	make_generate_rtl_prologue(SR_B)
	copy_param(sr, r)
	copy_param(sr, src1)
	make_generate_rtl_epilogue()
}

#define make_rtl_setget_eflags_body(f) \
  void concat(generate_rtl_set_, f) (const rtlreg_t* src) { \
	make_generate_rtl_prologue(concat(SET_, f)) \
	copy_param(set_eflags, src) \
	make_generate_rtl_epilogue() \
  } \
  void concat(generate_rtl_get_, f) (rtlreg_t* dest) { \
	make_generate_rtl_prologue(concat(GET_, f)) \
	copy_param(get_eflags, dest) \
	make_generate_rtl_epilogue() \
  }

make_rtl_setget_eflags_body(CF)
make_rtl_setget_eflags_body(OF)
make_rtl_setget_eflags_body(ZF)
make_rtl_setget_eflags_body(SF)

void generate_rtl_pio_read(rtlreg_t *dest, const rtlreg_t *addr, int len) {
	make_generate_rtl_prologue(PIO_READ)
	copy_param(pio_read, dest)
	copy_param(pio_read, addr)
	copy_param(pio_read, len)
	make_generate_rtl_epilogue()
}

void generate_rtl_pio_write(const rtlreg_t *addr, const rtlreg_t *src, int len) {
	make_generate_rtl_prologue(PIO_WRITE)
	copy_param(pio_write, addr)
	copy_param(pio_write, src)
	copy_param(pio_write, len)
	make_generate_rtl_epilogue()
}
