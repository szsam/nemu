#include "cpu/tb.h"
#include "cpu/rtl-interpret.h"

#define make_interpret_rtl_case(tag, instr, ...) \
	case tag: \
		concat(interpret_rtl_, instr) (__VA_ARGS__); \
		break;

void interpret_tblock(const TranslationBlock *tb) {
	RTLInstr *rtl;
	list_for_each_entry(rtl, &tb->rtl_instr_list.list, list) {
		switch (rtl->type) {
			make_interpret_rtl_case(J, j, rtl->j.target)
			make_interpret_rtl_case(JR, jr, rtl->jr.target)

			make_interpret_rtl_case(JRELOP, jrelop, rtl->jrelop.relop, rtl->jrelop.src1,
					rtl->jrelop.src2, rtl->jrelop.target)
			make_interpret_rtl_case(SETRELOP, setrelop, rtl->setrelop.relop, rtl->setrelop.dest,
					rtl->setrelop.src1, rtl->setrelop.src2)

			make_interpret_rtl_case(EXIT, exit, rtl->exit.state)
			make_interpret_rtl_case(LI, li, rtl->li.dest, rtl->li.imm)
			make_interpret_rtl_case(LM, lm, rtl->lm.dest, rtl->lm.addr, rtl->lm.len)
			make_interpret_rtl_case(SM, sm, rtl->sm.addr, rtl->sm.len, rtl->sm.src1)

			make_interpret_rtl_case(LR_L, lr_l, rtl->lr.dest, rtl->lr.r)
			make_interpret_rtl_case(LR_W, lr_w, rtl->lr.dest, rtl->lr.r)
			make_interpret_rtl_case(LR_B, lr_b, rtl->lr.dest, rtl->lr.r)

			make_interpret_rtl_case(SR_L, sr_l, rtl->sr.r, rtl->sr.src1)
			make_interpret_rtl_case(SR_W, sr_w, rtl->sr.r, rtl->sr.src1)
			make_interpret_rtl_case(SR_B, sr_b, rtl->sr.r, rtl->sr.src1)

			make_interpret_rtl_case(SET_CF, set_CF, rtl->set_eflags.src)
			make_interpret_rtl_case(SET_OF, set_OF, rtl->set_eflags.src)
			make_interpret_rtl_case(SET_ZF, set_ZF, rtl->set_eflags.src)
			make_interpret_rtl_case(SET_SF, set_SF, rtl->set_eflags.src)

			make_interpret_rtl_case(GET_CF, get_CF, rtl->get_eflags.dest)
			make_interpret_rtl_case(GET_OF, get_OF, rtl->get_eflags.dest)
			make_interpret_rtl_case(GET_ZF, get_ZF, rtl->get_eflags.dest)
			make_interpret_rtl_case(GET_SF, get_SF, rtl->get_eflags.dest)

			make_interpret_rtl_case(ADD    , add    , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(SUB    , sub    , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(AND    , and    , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(OR     , or     , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(XOR    , xor    , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(SHL    , shl    , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(SHR    , shr    , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(SAR    , sar    , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(MUL_LO , mul_lo , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(MUL_HI , mul_hi , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(IMUL_LO, imul_lo, rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(IMUL_HI, imul_hi, rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(DIV_Q  , div_q  , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(DIV_R  , div_r  , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(IDIV_Q , idiv_q , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
			make_interpret_rtl_case(IDIV_R , idiv_r , rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);

			make_interpret_rtl_case(ADDI    , addi    , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(SUBI    , subi    , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(ANDI    , andi    , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(ORI     , ori     , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(XORI    , xori    , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(SHLI    , shli    , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(SHRI    , shri    , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(SARI    , sari    , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(MUL_LOI , mul_loi , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(MUL_HII , mul_hii , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(IMUL_LOI, imul_loi, rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(IMUL_HII, imul_hii, rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(DIV_QI  , div_qi  , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(DIV_RI  , div_ri  , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(IDIV_QI , idiv_qi , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
			make_interpret_rtl_case(IDIV_RI , idiv_ri , rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);

			make_interpret_rtl_case(DIV64_Q, div64_q, 
					rtl->div64.dest, rtl->div64.src1_hi, rtl->div64.src1_lo, rtl->div64.src2);
			make_interpret_rtl_case(DIV64_R, div64_r, 
					rtl->div64.dest, rtl->div64.src1_hi, rtl->div64.src1_lo, rtl->div64.src2);
			make_interpret_rtl_case(IDIV64_Q, idiv64_q, 
					rtl->div64.dest, rtl->div64.src1_hi, rtl->div64.src1_lo, rtl->div64.src2);
			make_interpret_rtl_case(IDIV64_R, idiv64_r, 
					rtl->div64.dest, rtl->div64.src1_hi, rtl->div64.src1_lo, rtl->div64.src2);

			make_interpret_rtl_case(PIO_READ, pio_read, 
				rtl->pio_read.dest, rtl->pio_read.addr, rtl->pio_read.len);
			make_interpret_rtl_case(PIO_WRITE, pio_write, 
				rtl->pio_write.addr, rtl->pio_write.src, rtl->pio_write.len);

			default:
				assert(0);
		}
	}
}
