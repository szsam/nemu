#include "cpu/tb.h"
#include "cpu/rtl-interpret.h"

#define make_interpret_rtl_case(tag, instr, ...) \
	case tag: \
		concat(interpret_rtl_, instr) (__VA_ARGS__); \
		break;

// rtl register: number to address
#define n2a(n) (rtlreg_t *)rtl_registers[n]

void interpret_tblock(const TranslationBlock *tb) {
	RTLInstr *rtl = tb->rtl_instr_arr;
	int i = 0;
	for (; i < tb->rtl_instr_cnt; ++i, ++rtl) {
		switch (rtl->opcode) {
			make_interpret_rtl_case(J, j, rtl->target)
			make_interpret_rtl_case(JR, jr, n2a(rtl->r1))

			make_interpret_rtl_case(JRELOP, jrelop, rtl->relop, 
					n2a(rtl->r2), n2a(rtl->r3), rtl->target)
			make_interpret_rtl_case(SETRELOP, setrelop, rtl->relop, 
					n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))

			make_interpret_rtl_case(EXIT, exit, rtl->state)
			make_interpret_rtl_case(LI, li, n2a(rtl->r1), rtl->imm)
			make_interpret_rtl_case(LM, lm, n2a(rtl->r1), n2a(rtl->r2), rtl->len)
			make_interpret_rtl_case(SM, sm, n2a(rtl->r2), rtl->len, n2a(rtl->r3))

			make_interpret_rtl_case(LR_L, lr_l, n2a(rtl->r1), rtl->r2)
			make_interpret_rtl_case(LR_W, lr_w, n2a(rtl->r1), rtl->r2)
			make_interpret_rtl_case(LR_B, lr_b, n2a(rtl->r1), rtl->r2)

			make_interpret_rtl_case(SR_L, sr_l, rtl->r1, n2a(rtl->r2))
			make_interpret_rtl_case(SR_W, sr_w, rtl->r1, n2a(rtl->r2))
			make_interpret_rtl_case(SR_B, sr_b, rtl->r1, n2a(rtl->r2))

			make_interpret_rtl_case(SET_CF, set_CF, n2a(rtl->r1))
			make_interpret_rtl_case(SET_OF, set_OF, n2a(rtl->r1))
			make_interpret_rtl_case(SET_ZF, set_ZF, n2a(rtl->r1))
			make_interpret_rtl_case(SET_SF, set_SF, n2a(rtl->r1))

			make_interpret_rtl_case(GET_CF, get_CF, n2a(rtl->r1))
			make_interpret_rtl_case(GET_OF, get_OF, n2a(rtl->r1))
			make_interpret_rtl_case(GET_ZF, get_ZF, n2a(rtl->r1))
			make_interpret_rtl_case(GET_SF, get_SF, n2a(rtl->r1))

			make_interpret_rtl_case(ADD    , add    , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(SUB    , sub    , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(AND    , and    , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(OR     , or     , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(XOR    , xor    , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(SHL    , shl    , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(SHR    , shr    , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(SAR    , sar    , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(MUL_LO , mul_lo , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(MUL_HI , mul_hi , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(IMUL_LO, imul_lo, n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(IMUL_HI, imul_hi, n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(DIV_Q  , div_q  , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(DIV_R  , div_r  , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(IDIV_Q , idiv_q , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))
			make_interpret_rtl_case(IDIV_R , idiv_r , n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3))

			make_interpret_rtl_case(ADDI    , addi    , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(SUBI    , subi    , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(ANDI    , andi    , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(ORI     , ori     , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(XORI    , xori    , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(SHLI    , shli    , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(SHRI    , shri    , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(SARI    , sari    , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(MUL_LOI , mul_loi , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(MUL_HII , mul_hii , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(IMUL_LOI, imul_loi, n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(IMUL_HII, imul_hii, n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(DIV_QI  , div_qi  , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(DIV_RI  , div_ri  , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(IDIV_QI , idiv_qi , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)
			make_interpret_rtl_case(IDIV_RI , idiv_ri , n2a(rtl->r1), n2a(rtl->r2), rtl->imm)

			make_interpret_rtl_case(DIV64_Q, div64_q, 
					n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3), n2a(rtl->r4))
			make_interpret_rtl_case(DIV64_R, div64_r, 
					n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3), n2a(rtl->r4))
			make_interpret_rtl_case(IDIV64_Q, idiv64_q, 
					n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3), n2a(rtl->r4))
			make_interpret_rtl_case(IDIV64_R, idiv64_r, 
					n2a(rtl->r1), n2a(rtl->r2), n2a(rtl->r3), n2a(rtl->r4))

			make_interpret_rtl_case(PIO_READ, pio_read, n2a(rtl->r1), n2a(rtl->r2), rtl->len)
			make_interpret_rtl_case(PIO_WRITE, pio_write, n2a(rtl->r2), n2a(rtl->r3), rtl->len)

			default:
				assert(0);
		}
	}
}


void optimize_tblock(TranslationBlock *tb) {
	tb->rtl_instr_arr = (RTLInstr *)malloc(sizeof(RTLInstr) * tb->rtl_instr_cnt);
	RTLInstrListItem *rtl_list_item;
	int i = 0;
	list_for_each_entry(rtl_list_item, &tb->rtl_instr_list.list, list) {
		tb->rtl_instr_arr[i++] = rtl_list_item->rtl_instr;
	}
	assert(i == tb->rtl_instr_cnt);
}



