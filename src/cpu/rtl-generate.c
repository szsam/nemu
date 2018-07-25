#include <stdlib.h>

#include "cpu/rtl.h"
#include "cpu/tb.h"
// #include "cpu/decode.h"
// #include "cpu/reg.h"

// const rtlreg_t * const rtl_registers[] = {
// 	&cpu.eax, &cpu.ecx, &cpu.edx, &cpu.ebx, 
// 	&cpu.esp, &cpu.ebp, &cpu.esi, &cpu.edi,
// 	&id_dest->val, &id_src->val, 
// 	&tzero, &t0, &t1, &t2, &t3, &at,
// 	&id_src2->val, 
// 	&id_dest->addr, &id_src->addr, &id_src2->addr, 
// };


/* RTL basic instructions */

#define make_generate_rtl_prologue(op) \
	RTLInstrListItem *rtl_instr_list_item \
		= (RTLInstrListItem *)malloc(sizeof(RTLInstrListItem)); \
	RTLInstr *rtl = &rtl_instr_list_item->rtl_instr; \
	rtl->opcode = op; \
	++cur_tblock->rtl_instr_cnt; 

#define make_generate_rtl_epilogue() \
	list_add_tail(&rtl_instr_list_item->list, &cur_tblock->rtl_instr_list.list);

#define copy_param(field, arg) rtl->field = arg;

void generate_rtl_j(vaddr_t target) {
	make_generate_rtl_prologue(J)
	copy_param(target, target)
	make_generate_rtl_epilogue()
}

void generate_rtl_jr(rtlreg_t *target) {
	make_generate_rtl_prologue(JR)
	copy_param(r1, target)
	make_generate_rtl_epilogue()
}

void generate_rtl_jrelop(uint32_t relop,
    const rtlreg_t *src1, const rtlreg_t *src2, vaddr_t target) {
	make_generate_rtl_prologue(JRELOP)
	copy_param(relop, relop)
	copy_param(r2, src1)
	copy_param(r3, src2)
	copy_param(target, target)
	make_generate_rtl_epilogue()
}

void generate_rtl_setrelop(uint32_t relop, rtlreg_t *dest,
    const rtlreg_t *src1, const rtlreg_t *src2) {
	make_generate_rtl_prologue(SETRELOP)
	copy_param(relop, relop)
	copy_param(r1, dest)
	copy_param(r2, src1)
	copy_param(r3, src2)
	make_generate_rtl_epilogue()
}

void generate_rtl_exit(int state) {
	make_generate_rtl_prologue(EXIT)
	copy_param(state, state)
	make_generate_rtl_epilogue()
}

void generate_rtl_li(rtlreg_t* dest, uint32_t imm) {
	make_generate_rtl_prologue(LI)
	copy_param(r1, dest)
	copy_param(imm, imm)
	make_generate_rtl_epilogue()
}

#define make_rtl_arith_logic_body(name, tag) \
  void concat(generate_rtl_, name) (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
	make_generate_rtl_prologue(tag) \
	copy_param(r1, dest) \
	copy_param(r2, src1) \
	copy_param(r3, src2) \
	make_generate_rtl_epilogue() \
  } \
  void concat3(generate_rtl_, name, i) (rtlreg_t* dest, const rtlreg_t* src1, int imm) { \
	make_generate_rtl_prologue(concat(tag, I)) \
	copy_param(r1, dest) \
	copy_param(r2, src1) \
	copy_param(imm, imm) \
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
	copy_param(r1, dest) \
	copy_param(r2, src1_hi) \
	copy_param(r3, src1_lo) \
	copy_param(r4, src2) \
	make_generate_rtl_epilogue() \
}

make_rtl_div64(div64_q, DIV64_Q)
make_rtl_div64(div64_r, DIV64_R)
make_rtl_div64(idiv64_q, IDIV64_Q)
make_rtl_div64(idiv64_r, IDIV64_R)


void generate_rtl_lm(rtlreg_t *dest, const rtlreg_t* addr, int len) {
	make_generate_rtl_prologue(LM)
	copy_param(r1, dest)
	copy_param(r2, addr)
	copy_param(len, len)
	make_generate_rtl_epilogue()
}

void generate_rtl_sm(const rtlreg_t* addr, int len, const rtlreg_t* src1) {
	make_generate_rtl_prologue(SM)
	copy_param(r2, addr)
	copy_param(len, len)
	copy_param(r3, src1)
	make_generate_rtl_epilogue()
}

void generate_rtl_lr_l(rtlreg_t* dest, int r) {
	make_generate_rtl_prologue(LR_L)
	copy_param(r1, dest)
	copy_param(r, r)
	make_generate_rtl_epilogue()
}

void generate_rtl_lr_w(rtlreg_t* dest, int r) {
	make_generate_rtl_prologue(LR_W)
	copy_param(r1, dest)
	copy_param(r, r)
	make_generate_rtl_epilogue()
}

void generate_rtl_lr_b(rtlreg_t* dest, int r) {
	make_generate_rtl_prologue(LR_B)
	copy_param(r1, dest)
	copy_param(r, r)
	make_generate_rtl_epilogue()
}

void generate_rtl_sr_l(int r, const rtlreg_t* src1) {
	make_generate_rtl_prologue(SR_L)
	copy_param(r, r)
	copy_param(r2, src1)
	make_generate_rtl_epilogue()
}

void generate_rtl_sr_w(int r, const rtlreg_t* src1) {
	make_generate_rtl_prologue(SR_W)
	copy_param(r, r)
	copy_param(r2, src1)
	make_generate_rtl_epilogue()
}

void generate_rtl_sr_b(int r, const rtlreg_t* src1) {
	make_generate_rtl_prologue(SR_B)
	copy_param(r, r)
	copy_param(r2, src1)
	make_generate_rtl_epilogue()
}

#define make_rtl_setget_eflags_body(f) \
  void concat(generate_rtl_set_, f) (const rtlreg_t* src) { \
	make_generate_rtl_prologue(concat(SET_, f)) \
	copy_param(r2, src) \
	make_generate_rtl_epilogue() \
  } \
  void concat(generate_rtl_get_, f) (rtlreg_t* dest) { \
	make_generate_rtl_prologue(concat(GET_, f)) \
	copy_param(r1, dest) \
	make_generate_rtl_epilogue() \
  }

make_rtl_setget_eflags_body(CF)
make_rtl_setget_eflags_body(OF)
make_rtl_setget_eflags_body(ZF)
make_rtl_setget_eflags_body(SF)

void generate_rtl_pio_read(rtlreg_t *dest, const rtlreg_t *addr, int len) {
	make_generate_rtl_prologue(PIO_READ)
	copy_param(r1, dest)
	copy_param(r2, addr)
	copy_param(len, len)
	make_generate_rtl_epilogue()
}

void generate_rtl_pio_write(const rtlreg_t *addr, const rtlreg_t *src, int len) {
	make_generate_rtl_prologue(PIO_WRITE)
	copy_param(r2, addr)
	copy_param(r3, src)
	copy_param(len, len)
	make_generate_rtl_epilogue()
}

void generate_rtl_cc_set_op(int op, const rtlreg_t *res,
		const rtlreg_t *dest, const rtlreg_t *src) {
	make_generate_rtl_prologue(CC_SET_OP)
	copy_param(imm, op)
	copy_param(r2, res)
	copy_param(r3, dest)
	copy_param(r4, src)
	make_generate_rtl_epilogue()
}

