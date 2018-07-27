#include "cpu/tb.h"
#include "cpu/decode.h"
#include "cpu/reg.h"

void dead_code_elimination(TranslationBlock *);
void constant_propogation(TranslationBlock *);
void remove_redundant_sr(TranslationBlock *tb);
void optimize_jcc(TranslationBlock *tb);

// address to number
int rtlreg_a2n(const rtlreg_t *r) {
	if (r >= &cpu.eax && r <= &cpu.edi)
		return r - &cpu.eax;
	else if (r == &id_dest->data) return R_DEST_VAL;
	else if (r == &id_src->data) return R_SRC_VAL;
	else if (r == &tzero) return R_TZERO;
	else if (r == &t0) return R_T0;
	else if (r == &t1) return R_T1; 
	else if (r == &t2) return R_T2; 
	else if (r == &t3) return R_T3; 
	else if (r == &at) return R_AT; 
	else if (r == &id_src2->data) return R_SRC2_VAL;
	else if (r == &id_dest->addr) return R_DEST_ADDR;
	else if (r == &id_src->addr) return R_SRC_ADDR;
	else if (r == &id_src2->addr) return R_SRC2_ADDR;
	else assert(0);
}

void optimize_tblock(TranslationBlock *tb) {
	constant_propogation(tb);
	dead_code_elimination(tb);
	remove_redundant_sr(tb);
	optimize_jcc(tb);

	tb->rtl_instr_arr = (RTLInstr *)calloc(tb->rtl_instr_cnt + 1, sizeof(RTLInstr));
	RTLInstrListItem *rtl_list_item;
	int i = 0;
	list_for_each_entry(rtl_list_item, &tb->rtl_instr_list.list, list) {
		tb->rtl_instr_arr[i++] = rtl_list_item->rtl_instr;
	}
	assert(i == tb->rtl_instr_cnt);
	tb->rtl_instr_arr[i].opcode = EOB;
}

