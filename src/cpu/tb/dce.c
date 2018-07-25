#include "cpu/tb.h"

// x <- y
#define make_dce_case1(opcode, x, y) \
	case opcode: \
		if (live[x]) { \
			live[x] = 0; live[y] = 1; \
		} \
		else { \
			list_del(&list_item->list); \
			free(list_item); \
			--tb->rtl_instr_cnt; \
		} \
		break;

// x <- y + z
#define make_dce_case2(opcode, x, y, z) \
	case opcode: \
		if (live[x]) { \
			live[x] = 0; live[y] = live[z] = 1;\
		} \
		else { \
			list_del(&list_item->list); \
			free(list_item); \
			--tb->rtl_instr_cnt; \
		} \
		break;

// x <- y + z + w
#define make_dce_case3(opcode, x, y, z, w) \
	case opcode: \
		if (live[x]) { \
			live[x] = 0; live[y] = live[z] = live[w] = 1; \
		} \
		else { \
			list_del(&list_item->list); \
			free(list_item); \
			--tb->rtl_instr_cnt; \
		} \
		break;

#define make_dce_set_eflags(opcode, flag, src) \
	case opcode: \
		if (concat(live_, flag)) { \
			concat(live_, flag) = 0; live[src] = 1; \
		} \
		else { \
			list_del(&list_item->list); \
			free(list_item); \
			--tb->rtl_instr_cnt; \
		} \
		break;

#define make_dce_get_eflags(opcode, flag, dest) \
	case opcode: \
		if (live[dest]) { \
			live[dest] = 0; live_cc = 1; \
		} \
		else { \
			list_del(&list_item->list); \
			free(list_item); \
			--tb->rtl_instr_cnt; \
		} \
		break;

void dead_code_elimination(TranslationBlock *tb) {
	RTLInstrListItem *list_item;
	RTLInstrListItem *tmp;

	// liveness of RTL registers, initialized by their values
	// on exit of a basic block
	bool live[NR_RTLREG] = {
		1, 1, 1, 1, 1, 1, 1, 1, // eight GPRs are live, others are not live
	};
	// bool live_CF = 1, live_OF = 1, live_ZF = 1, live_SF = 1;
	bool live_cc = 1;

	list_for_each_entry_safe_prev(list_item, tmp, &tb->rtl_instr_list.list, list) {
		RTLInstr *rtl = &list_item->rtl_instr;
		switch (rtl->opcode) {
			case JR: live[a2n(rtl->r1)] = 1; break;
			case JRELOP: live[a2n(rtl->r2)] = live[a2n(rtl->r3)] = 1; break;

			make_dce_case2(SETRELOP, a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))

			case LI: 
				if (live[a2n(rtl->r1)]) {
					live[a2n(rtl->r1)] = 0;
				}
				else {
					list_del(&list_item->list);
					free(list_item);
					--tb->rtl_instr_cnt;
				}
				break;

//			case LM: live[a2n(rtl->r2)] = 1; break;
			make_dce_case1(LM, a2n(rtl->r1), a2n(rtl->r2))
			case SM: live[a2n(rtl->r2)] = live[a2n(rtl->r3)] = 1; break;

			make_dce_case1(LR_L, a2n(rtl->r1), rtl->r)
			make_dce_case1(LR_W, a2n(rtl->r1), rtl->r)
			make_dce_case1(LR_B, a2n(rtl->r1), rtl->r)
//			case LR_W: live[rtl->r] = 1; break;
//			case LR_B: live[rtl->r] = 1; break;

			make_dce_case1(SR_L, rtl->r, a2n(rtl->r2))
			case SR_W: live[a2n(rtl->r2)] = 1; break;
			case SR_B: live[a2n(rtl->r2)] = 1; break;

//			make_dce_set_eflags(SET_CF, CF, a2n(rtl->r2))
//			make_dce_set_eflags(SET_OF, OF, a2n(rtl->r2))
//			make_dce_set_eflags(SET_ZF, ZF, a2n(rtl->r2))
//			make_dce_set_eflags(SET_SF, SF, a2n(rtl->r2))

			make_dce_get_eflags(GET_CF, CF, a2n(rtl->r1))
			make_dce_get_eflags(GET_OF, OF, a2n(rtl->r1))
			make_dce_get_eflags(GET_ZF, ZF, a2n(rtl->r1))
			make_dce_get_eflags(GET_SF, SF, a2n(rtl->r1))

			make_dce_case2(ADD    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(SUB    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(AND    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(OR     , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(XOR    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(SHL    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(SHR    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(SAR    , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(MUL_LO , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(MUL_HI , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(IMUL_LO, a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(IMUL_HI, a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(DIV_Q  , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(DIV_R  , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(IDIV_Q , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))
			make_dce_case2(IDIV_R , a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3))

			make_dce_case1(ADDI    , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(SUBI    , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(ANDI    , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(ORI     , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(XORI    , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(SHLI    , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(SHRI    , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(SARI    , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(MUL_LOI , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(MUL_HII , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(IMUL_LOI, a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(IMUL_HII, a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(DIV_QI  , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(DIV_RI  , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(IDIV_QI , a2n(rtl->r1), a2n(rtl->r2))
			make_dce_case1(IDIV_RI , a2n(rtl->r1), a2n(rtl->r2))

			make_dce_case3(DIV64_Q,  a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3), a2n(rtl->r4))
			make_dce_case3(DIV64_R,  a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3), a2n(rtl->r4))
			make_dce_case3(IDIV64_Q, a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3), a2n(rtl->r4))
			make_dce_case3(IDIV64_R, a2n(rtl->r1), a2n(rtl->r2), a2n(rtl->r3), a2n(rtl->r4))

			case PIO_READ: live[a2n(rtl->r2)] = 1; break;
			case PIO_WRITE: live[a2n(rtl->r2)] = live[a2n(rtl->r3)] = 1; break;

			case CC_SET_OP:
				if (live_cc) {
					live[a2n(rtl->r2)] = live[a2n(rtl->r3)] = live[a2n(rtl->r4)] = 1;
					live_cc = 0;
				}
				else {
					list_del(&list_item->list);
					free(list_item);
					--tb->rtl_instr_cnt;
				}

			default: break;
		}
	}
}
