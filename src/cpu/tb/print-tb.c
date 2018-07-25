#include "cpu/tb.h"

const char * const rtlreg_str[] = {
	"eax", "ecx", "edx", "ebx", 
	"esp", "ebp", "esi", "edi",
	"id_dest->val", "id_src->val", 
	"tzero", "t0", "t1", "t2", "t3", "at",
	"id_src2->val", 
	"id_dest->addr", "id_src->addr", "id_src2->addr", 
};

#define make_print_rtl_case(opcode, name, fmtstr, ...) \
	case opcode: \
		printf("rtl_" #name "(" fmtstr ")\n", __VA_ARGS__); \
		break;

// rtl register: address to string
#define a2s(a) rtlreg_str[a2n(a)]

void print_tblock(const TranslationBlock *tb) {
	RTLInstrListItem *rtl_list_item;
	list_for_each_entry(rtl_list_item, &tb->rtl_instr_list.list, list) {
		RTLInstr *rtl = &rtl_list_item->rtl_instr;
		switch (rtl->opcode) {
			make_print_rtl_case(J, j, "0x%x", rtl->target)
			make_print_rtl_case(JR, jr, "%s", a2s(rtl->r1))

			make_print_rtl_case(JRELOP, jrelop, "%d, %s, %s, 0x%x", rtl->relop, 
					a2s(rtl->r2), a2s(rtl->r3), rtl->target)
			make_print_rtl_case(SETRELOP, setrelop, "%d, %s, %s, %s", rtl->relop, 
					a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))

			make_print_rtl_case(EXIT, exit, "%d", rtl->state)
			make_print_rtl_case(LI, li, "%s, 0x%x", a2s(rtl->r1), rtl->imm)
			make_print_rtl_case(LM, lm, "%s, (%s), %d", a2s(rtl->r1), a2s(rtl->r2), rtl->len)
			make_print_rtl_case(SM, sm, "(%s), %d, %s", a2s(rtl->r2), rtl->len, a2s(rtl->r3))

			make_print_rtl_case(LR_L, lr_l, "%s, %s", a2s(rtl->r1), rtlreg_str[rtl->r])
			make_print_rtl_case(LR_W, lr_w, "%s, %s", a2s(rtl->r1), rtlreg_str[rtl->r])
			make_print_rtl_case(LR_B, lr_b, "%s, %s", a2s(rtl->r1), rtlreg_str[rtl->r])

			make_print_rtl_case(SR_L, sr_l, "%s, %s", rtlreg_str[rtl->r], a2s(rtl->r2))
			make_print_rtl_case(SR_W, sr_w, "%s, %s", rtlreg_str[rtl->r], a2s(rtl->r2))
			make_print_rtl_case(SR_B, sr_b, "%s, %s", rtlreg_str[rtl->r], a2s(rtl->r2))

			make_print_rtl_case(SET_CF, set_CF, "%s", a2s(rtl->r2))
			make_print_rtl_case(SET_OF, set_OF, "%s", a2s(rtl->r2))
			make_print_rtl_case(SET_ZF, set_ZF, "%s", a2s(rtl->r2))
			make_print_rtl_case(SET_SF, set_SF, "%s", a2s(rtl->r2))

			make_print_rtl_case(GET_CF, get_CF, "%s", a2s(rtl->r1))
			make_print_rtl_case(GET_OF, get_OF, "%s", a2s(rtl->r1))
			make_print_rtl_case(GET_ZF, get_ZF, "%s", a2s(rtl->r1))
			make_print_rtl_case(GET_SF, get_SF, "%s", a2s(rtl->r1))

			make_print_rtl_case(ADD    , add    , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(SUB    , sub    , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(AND    , and    , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(OR     , or     , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(XOR    , xor    , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(SHL    , shl    , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(SHR    , shr    , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(SAR    , sar    , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(MUL_LO , mul_lo , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(MUL_HI , mul_hi , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(IMUL_LO, imul_lo, "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(IMUL_HI, imul_hi, "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(DIV_Q  , div_q  , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(DIV_R  , div_r  , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(IDIV_Q , idiv_q , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))
			make_print_rtl_case(IDIV_R , idiv_r , "%s, %s, %s", a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3))

			make_print_rtl_case(ADDI    , addi    , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(SUBI    , subi    , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(ANDI    , andi    , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(ORI     , ori     , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(XORI    , xori    , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(SHLI    , shli    , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(SHRI    , shri    , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(SARI    , sari    , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(MUL_LOI , mul_loi , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(MUL_HII , mul_hii , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(IMUL_LOI, imul_loi, "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(IMUL_HII, imul_hii, "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(DIV_QI  , div_qi  , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(DIV_RI  , div_ri  , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(IDIV_QI , idiv_qi , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)
			make_print_rtl_case(IDIV_RI , idiv_ri , "%s, %s, %d", a2s(rtl->r1), a2s(rtl->r2), rtl->imm)

			make_print_rtl_case(DIV64_Q, div64_q, "%s, %s, %s, %s", 
					a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3), a2s(rtl->r4))
			make_print_rtl_case(DIV64_R, div64_r, "%s, %s, %s, %s", 
					a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3), a2s(rtl->r4))
			make_print_rtl_case(IDIV64_Q, idiv64_q, "%s, %s, %s, %s", 
					a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3), a2s(rtl->r4))
			make_print_rtl_case(IDIV64_R, idiv64_r, "%s, %s, %s, %s", 
					a2s(rtl->r1), a2s(rtl->r2), a2s(rtl->r3), a2s(rtl->r4))

			make_print_rtl_case(PIO_READ, pio_read, "%s, (%s), %d", a2s(rtl->r1), a2s(rtl->r2), rtl->len)
			make_print_rtl_case(PIO_WRITE, pio_write, "(%s), %s, %d", a2s(rtl->r2), a2s(rtl->r3), rtl->len)

			make_print_rtl_case(CC_SET_OP, cc_set_op, "0x%x, %s, %s, %s", 
					rtl->imm, a2s(rtl->r2), a2s(rtl->r3), a2s(rtl->r4))

			default:
				assert(0);
		}
	}
}
