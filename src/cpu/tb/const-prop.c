#include "cpu/tb.h"

struct {
	bool is_const;
	uint32_t val;
}reg_const[NR_RTLREG];

void constant_propogation(TranslationBlock *tb) {
	RTLInstrListItem *list_item;
	// RTLInstrListItem *tmp;

	for (int i = 0; i < NR_RTLREG; i++) {
		reg_const[i].is_const = false;
	}

	list_for_each_entry(list_item, &tb->rtl_instr_list.list, list) {
		RTLInstr *rtl = &list_item->rtl_instr;
		RTLInstrOpcode op = rtl->opcode;
		if (op == LI) {
			reg_const[a2n(rtl->r1)].is_const = true;
			reg_const[a2n(rtl->r1)].val = rtl->imm;
		}
		else if (op >= ADD && op <= IDIV_R) {
			if (reg_const[a2n(rtl->r3)].is_const) {
				rtl->opcode += ADDI-ADD;
				rtl->imm = reg_const[a2n(rtl->r3)].val;
			}
			reg_const[a2n(rtl->r1)].is_const = false;
		}
		else if (op == LR_L) {
			if (reg_const[rtl->r].is_const) {
				rtl->opcode = LI;
				// rtl->r1 remains unchanged
				rtl->imm = reg_const[rtl->r].val;
				reg_const[a2n(rtl->r1)].is_const = true;
				reg_const[a2n(rtl->r1)].val = rtl->imm;
			}	
			else {
				reg_const[a2n(rtl->r1)].is_const = false;
			}
		}
		else if (op == SR_L) {
			if (reg_const[a2n(rtl->r2)].is_const) {
				rtl->opcode = LI;
				rtl->r1 = &reg_l(rtl->r);
				rtl->imm = reg_const[a2n(rtl->r2)].val;
				reg_const[a2n(rtl->r1)].is_const = true;
				reg_const[a2n(rtl->r1)].val = rtl->imm;
			}	
			else {
				reg_const[rtl->r].is_const = false;
			}
		}
		else if (op == SETRELOP || op == LM || op == DIV64_Q 
			|| op == DIV64_R || op == IDIV64_Q || op == IDIV64_R
			|| op == LR_B || op == LR_W || op == PIO_READ ||
			(op >= ADDI && op <= IDIV_RI)) {
			reg_const[a2n(rtl->r1)].is_const = false;
		}
		else if (op == SR_B || op == SR_W) {
			reg_const[rtl->r].is_const = false;
		}
				

	}
}
