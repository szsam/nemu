#include "cpu/tb.h"
#include "rtl-interpret.h"

#define make_interpret_rtl_case(opcode, name, ...) \
	case opcode: \
		concat(interpret_rtl_, name) (__VA_ARGS__); \
		break;

// rtl register: number to address
// #define n2a(n) (rtlreg_t *)rtl_registers[n]

void interpret_tblock(const TranslationBlock *tb) {
	RTLInstr *rtl = tb->rtl_instr_arr;
	RTLInstr *rtl_end = rtl + tb->rtl_instr_cnt;
	for (; rtl < rtl_end; ++rtl) {
		switch (rtl->opcode) {
			make_interpret_rtl_case(J, j, rtl->target)
			make_interpret_rtl_case(JR, jr, rtl->r1)

			make_interpret_rtl_case(JRELOP, jrelop, rtl->relop, 
					rtl->r2, rtl->r3, rtl->target)
			make_interpret_rtl_case(SETRELOP, setrelop, rtl->relop, 
					rtl->r1, rtl->r2, rtl->r3)

			make_interpret_rtl_case(EXIT, exit, rtl->state)
			make_interpret_rtl_case(LI, li, rtl->r1, rtl->imm)
			make_interpret_rtl_case(LM, lm, rtl->r1, rtl->r2, rtl->len)
			make_interpret_rtl_case(SM, sm, rtl->r2, rtl->len, rtl->r3)

			make_interpret_rtl_case(LR_L, lr_l, rtl->r1, rtl->r)
			make_interpret_rtl_case(LR_W, lr_w, rtl->r1, rtl->r)
			make_interpret_rtl_case(LR_B, lr_b, rtl->r1, rtl->r)

			make_interpret_rtl_case(SR_L, sr_l, rtl->r, rtl->r2)
			make_interpret_rtl_case(SR_W, sr_w, rtl->r, rtl->r2)
			make_interpret_rtl_case(SR_B, sr_b, rtl->r, rtl->r2)

			make_interpret_rtl_case(SET_CF, set_CF, rtl->r2)
			make_interpret_rtl_case(SET_OF, set_OF, rtl->r2)
			make_interpret_rtl_case(SET_ZF, set_ZF, rtl->r2)
			make_interpret_rtl_case(SET_SF, set_SF, rtl->r2)

			make_interpret_rtl_case(GET_CF, get_CF, rtl->r1)
			make_interpret_rtl_case(GET_OF, get_OF, rtl->r1)
			make_interpret_rtl_case(GET_ZF, get_ZF, rtl->r1)
			make_interpret_rtl_case(GET_SF, get_SF, rtl->r1)

			make_interpret_rtl_case(ADD    , add    , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(SUB    , sub    , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(AND    , and    , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(OR     , or     , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(XOR    , xor    , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(SHL    , shl    , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(SHR    , shr    , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(SAR    , sar    , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(MUL_LO , mul_lo , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(MUL_HI , mul_hi , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(IMUL_LO, imul_lo, rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(IMUL_HI, imul_hi, rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(DIV_Q  , div_q  , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(DIV_R  , div_r  , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(IDIV_Q , idiv_q , rtl->r1, rtl->r2, rtl->r3)
			make_interpret_rtl_case(IDIV_R , idiv_r , rtl->r1, rtl->r2, rtl->r3)

			make_interpret_rtl_case(ADDI    , addi    , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(SUBI    , subi    , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(ANDI    , andi    , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(ORI     , ori     , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(XORI    , xori    , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(SHLI    , shli    , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(SHRI    , shri    , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(SARI    , sari    , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(MUL_LOI , mul_loi , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(MUL_HII , mul_hii , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(IMUL_LOI, imul_loi, rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(IMUL_HII, imul_hii, rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(DIV_QI  , div_qi  , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(DIV_RI  , div_ri  , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(IDIV_QI , idiv_qi , rtl->r1, rtl->r2, rtl->imm)
			make_interpret_rtl_case(IDIV_RI , idiv_ri , rtl->r1, rtl->r2, rtl->imm)

			make_interpret_rtl_case(DIV64_Q, div64_q, 
					rtl->r1, rtl->r2, rtl->r3, rtl->r4)
			make_interpret_rtl_case(DIV64_R, div64_r, 
					rtl->r1, rtl->r2, rtl->r3, rtl->r4)
			make_interpret_rtl_case(IDIV64_Q, idiv64_q, 
					rtl->r1, rtl->r2, rtl->r3, rtl->r4)
			make_interpret_rtl_case(IDIV64_R, idiv64_r, 
					rtl->r1, rtl->r2, rtl->r3, rtl->r4)

			make_interpret_rtl_case(PIO_READ, pio_read, rtl->r1, rtl->r2, rtl->len)
			make_interpret_rtl_case(PIO_WRITE, pio_write, rtl->r2, rtl->r3, rtl->len)

			default:
				assert(0);
		}
	}
}
