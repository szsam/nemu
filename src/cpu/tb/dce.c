#include "cpu/tb.h"

enum { R_CC_OP = R_EDI + 1, R_CC_RES, R_CC_DEST, R_CC_SRC };

enum { DCE_NULL = 0, DCE_USE = 1, DCE_DEF = 2 };

const int rtlop_use_def[][RTL_NUM_ARGS] = {
	[RTLOP_j] = { }, 
	[RTLOP_jr] = { DCE_USE }, 
	[RTLOP_jrelop] = { DCE_USE, DCE_USE }, 
	[RTLOP_setrelop] = { DCE_DEF, DCE_USE, DCE_USE }, 
	[RTLOP_exit] = { }, 
	[RTLOP_li] = { DCE_DEF },
   	[RTLOP_lm] = { DCE_DEF, DCE_USE },
	[RTLOP_sm] = { DCE_USE, DCE_NULL, DCE_USE },
   	[RTLOP_host_lm] = { DCE_DEF },
	[RTLOP_host_sm] = { DCE_USE },
	[RTLOP_add ... RTLOP_idiv_r] = { DCE_DEF, DCE_USE, DCE_USE },
	[RTLOP_addi ... RTLOP_idiv_ri] = { DCE_DEF, DCE_USE},
	[RTLOP_div64_q ... RTLOP_idiv64_r] = { DCE_DEF, DCE_USE, DCE_USE, DCE_USE },
	[RTLOP_pio_read] = { DCE_DEF, DCE_USE },
	[RTLOP_pio_write] = { DCE_USE, DCE_USE },
};

// address to index
static inline 
int a2i(RTLArg arg) { return (rtlreg_t *)arg - cpu.rtlregs; }

bool is_dead_code(RTLInsn *insn, bool *live) {
	const int *use_def = rtlop_use_def[insn->opcode];
	int argidx = 0;

	if (use_def[0] == DCE_DEF && !live[a2i(insn->args[0])])
		return true;
	else if (use_def[0] == DCE_DEF) {
		live[a2i(insn->args[0])] = 0;
		argidx = 1;
	}

	for (; argidx < RTL_NUM_ARGS; ++argidx) {
		if (use_def[argidx] == DCE_USE)
			live[a2i(insn->args[argidx])] = 1;
	}
	return false;
}

static inline 
void remove_insn(RTLInsn *insn, TranslationBlock *tb) {
	list_del(&insn->list);
	free(insn);
	--tb->rtl_instr_cnt;
}

void dead_code_elimination(TranslationBlock *tb) {
	RTLInsn *insn;
	RTLInsn *tmp;

	// liveness of RTL registers, initialized by their values
	// on exit of a basic block
	bool live[NR_RTLREGS] = {
        [R_EAX ... R_EDI] = 1,
		[R_CC_OP ... R_CC_SRC] = 1,
		// other registers are not live
	};

	list_for_each_entry_safe_prev(insn, tmp, &tb->rtl_insns, list) {
		switch (insn->opcode) {
			case RTLOP_lr_l: case RTLOP_lr_w: case RTLOP_lr_b:
				if (live[a2i(insn->args[0])]) {
					live[a2i(insn->args[0])] = 0;
					live[insn->args[1]] = 1;
				}
				else remove_insn(insn, tb);
				break;
			   
			case RTLOP_sr_l: 
				if (live[insn->args[0]]) {
					live[insn->args[0]] = 0;
					live[a2i(insn->args[1])] = 1;
				}
				else remove_insn(insn, tb);
				break;
				
			case RTLOP_sr_w: case RTLOP_sr_b:
				live[insn->args[0]] = 1;
				live[a2i(insn->args[1])] = 1;
				break;

			case RTLOP_get_CF ... RTLOP_get_SF:
				live[a2i(insn->args[0])] = 0;
				live[R_CC_OP] = live[R_CC_RES] = 1;
				live[R_CC_DEST] = live[R_CC_SRC] = 1;
				break;

			case RTLOP_discard:
				live[a2i(insn->args[0])] = 0;
				remove_insn(insn, tb);
				break;

			default: 
				if (is_dead_code(insn, live))
					remove_insn(insn, tb);
		}
	}
}
