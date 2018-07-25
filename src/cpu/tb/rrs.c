#include "cpu/tb.h"

/*
 * r1 <- ... (arith/logic/lm)
 * sr_l(r, r2)
 * where r1 == r2
 * and r2 is not live at the point immediately following the sr_l,
 * then the above two RTL instructions can be optimized to
 * r <- ...
 * */

// x <- y
#define make_rrs_case1(opcode, x, y) \
	case opcode: \
		live[x] = 0; live[y] = 1; \
		break;

// x <- y + z
#define make_rrs_case2(opcode, x, y, z) \
	case opcode: \
		live[x] = 0; live[y] = live[z] = 1;\
		break;

// x <- y + z + w
#define make_rrs_case3(opcode, x, y, z, w) \
	case opcode: \
		live[x] = 0; live[y] = live[z] = live[w] = 1; \
		break;

#define make_rrs_set_eflags(opcode, src) \
	case opcode: \
		live[src] = 1; \
		break;

#define make_rrs_get_eflags(opcode, dest) \
	case opcode: \
		live[dest] = 0; \
		break;

void remove_redundant_sr(TranslationBlock *tb) {
	RTLInstrListItem *list_item;
	RTLInstrListItem *tmp;

	// liveness of RTL registers, initialized by their values
	// on exit of a basic block
	bool live[NR_RTLREG] = {
		1, 1, 1, 1, 1, 1, 1, 1, // eight GPRs are live, others are not live
	};

	list_for_each_entry_safe_prev(list_item, tmp, &tb->rtl_instr_list.list, list) {
		RTLInstr *rtl = &list_item->rtl_instr;
		switch (rtl->opcode) {
			case JR: live[a2n(rtl->r1)] = 1; break;
			case JRELOP: live[a2n(rtl->r2)] = live[a2n(rtl->r3)] = 1; break;

			make_rrs_case2(SETRELOP, a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))

			case LI: 
				live[a2n(rtl->r1)] = 0;
				break;

			make_rrs_case1(LM, a2n(rtl->r1), a2n(rtl->r2))
			case SM: live[a2n(rtl->r2)] = live[a2n(rtl->r3)] = 1; break;

			make_rrs_case1(LR_L, a2n(rtl->r1), rtl->r)
			make_rrs_case1(LR_W, a2n(rtl->r1), rtl->r)
			make_rrs_case1(LR_B, a2n(rtl->r1), rtl->r)

			case SR_L:
			{
				bool has_prev = list_item->list.prev != &tb->rtl_instr_list.list;
				bool rewritten = false;
				if (!live[a2n(rtl->r2)] && has_prev) 
				{
					RTLInstrListItem *prev_item 
						= list_entry(list_item->list.prev, RTLInstrListItem, list);
					RTLInstr *prev_rtl = &prev_item->rtl_instr;
					RTLInstrOpcode op = prev_rtl->opcode;
					if ( ((op >= ADD && op <= IDIV_RI) || op == LM)
							&& prev_rtl->r1 == rtl->r2)
					{
						rewritten = true;
						prev_rtl->r1 = &reg_l(rtl->r);
						list_del(&list_item->list);
						free(list_item);
						--tb->rtl_instr_cnt;
					}
				}
				if (!rewritten) {
					live[rtl->r] = 0; live[a2n(rtl->r2)] = 1;
				}
				break;
			}

			case SR_W: live[a2n(rtl->r2)] = 1; break;
			case SR_B: live[a2n(rtl->r2)] = 1; break;

			make_rrs_set_eflags(SET_CF, a2n(rtl->r2))
			make_rrs_set_eflags(SET_OF, a2n(rtl->r2))
			make_rrs_set_eflags(SET_ZF, a2n(rtl->r2))
			make_rrs_set_eflags(SET_SF, a2n(rtl->r2))

			make_rrs_get_eflags(GET_CF, a2n(rtl->r1))
			make_rrs_get_eflags(GET_OF, a2n(rtl->r1))
			make_rrs_get_eflags(GET_ZF, a2n(rtl->r1))
			make_rrs_get_eflags(GET_SF, a2n(rtl->r1))

			make_rrs_case2(ADD    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(SUB    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(AND    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(OR     , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(XOR    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(SHL    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(SHR    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(SAR    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(MUL_LO , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(MUL_HI , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(IMUL_LO, a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(IMUL_HI, a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(DIV_Q  , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(DIV_R  , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(IDIV_Q , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_rrs_case2(IDIV_R , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))

			make_rrs_case1(ADDI    , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(SUBI    , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(ANDI    , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(ORI     , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(XORI    , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(SHLI    , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(SHRI    , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(SARI    , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(MUL_LOI , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(MUL_HII , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(IMUL_LOI, a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(IMUL_HII, a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(DIV_QI  , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(DIV_RI  , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(IDIV_QI , a2n(rtl->r1), a2n(rtl->r2))
			make_rrs_case1(IDIV_RI , a2n(rtl->r1), a2n(rtl->r2))

			make_rrs_case3(DIV64_Q,  a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3), a2n(rtl->r4))
			make_rrs_case3(DIV64_R,  a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3), a2n(rtl->r4))
			make_rrs_case3(IDIV64_Q, a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3), a2n(rtl->r4))
			make_rrs_case3(IDIV64_R, a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3), a2n(rtl->r4))

			case PIO_READ: live[a2n(rtl->r2)] = 1; break;
			case PIO_WRITE: live[a2n(rtl->r2)] = live[a2n(rtl->r3)] = 1; break;

			case CC_SET_OP: 
				live[a2n(rtl->r2)] = live[a2n(rtl->r3)] = live[a2n(rtl->r4)] = 1;
			default: break;
		}
	}
}
