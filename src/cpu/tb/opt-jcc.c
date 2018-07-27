#include "cpu/tb.h"

/* optimize cmpl+jcc
 * for example,
 *
 * rtl_sub(t0, ecx, esi)
 * rtl_cc_set_op(0xc, t0, ecx, esi)
 * rtl_get_SF(t0)
 * rtl_get_OF(t1)
 * rtl_xor(t0, t0, t1)
 * rtl_get_ZF(t1)
 * rtl_or(t0, t0, t1)
 * rtl_jrelop(RELOP_NE, t0, tzero, 0x103c40)
 *
 * can be optimized to
 *
 * rtl_sub(t0, ecx, esi)
 * rtl_cc_set_op(0xc, t0, ecx, esi)
 * rtl_jrelop(RELOP_LE, ecx, esi, 0x103c40)
 *
 */

#define GET_PREV_RTL \
	do { \
		head = head->prev; \
		list_item = list_entry(head, RTLInstrListItem, list); \
		rtl = &list_item->rtl_instr; \
	}while(0)

void optimize_jcc_helper(TranslationBlock *tb, 
		RTLInstrOpcode *op_arr, int op_arr_len, int relop) {
	// 3 = sub + cc_set_op + jrelop
	if (tb->rtl_instr_cnt < op_arr_len + 3) return;

	struct list_head *head;
	RTLInstrListItem *list_item;
	RTLInstr *rtl;

	head = &tb->rtl_instr_list.list;

	GET_PREV_RTL;
	if (rtl->opcode != JRELOP) return;
	RTLInstr *last_rtl = rtl;

	for (int i = 0; i < op_arr_len; i++) {
		GET_PREV_RTL;
		if (rtl->opcode != op_arr[i]) return;
	}

	GET_PREV_RTL;
	if (rtl->opcode != CC_SET_OP || rtl->imm != MAKE_CC_OP(CC_OP_SUB, 4))
		return;

	const rtlreg_t *the_dest = rtl->r3;
	const rtlreg_t *the_src = rtl->r4;
	last_rtl->relop = relop;
	last_rtl->r2 = the_dest;
	last_rtl->r3 = the_src;

	// remove the op_arr_len instructions before the last
	head = tb->rtl_instr_list.list.prev;
	head = head->prev;
	struct list_head *prev_head;
	while (op_arr_len--) {
		prev_head = head->prev;
		list_del(head);
		list_item = list_entry(head, RTLInstrListItem, list);
		free(list_item);
		--tb->rtl_instr_cnt;
		head = prev_head;
	}
}

void optimize_jle(TranslationBlock *tb) {
	static RTLInstrOpcode op_arr[] = { OR, GET_ZF, XOR, GET_OF, GET_SF };
	optimize_jcc_helper(tb, op_arr, sizeof(op_arr)/sizeof(op_arr[0]), RELOP_LE);
}

void optimize_jl(TranslationBlock *tb) {
	static RTLInstrOpcode op_arr[] = { XOR, GET_OF, GET_SF };
	optimize_jcc_helper(tb, op_arr, sizeof(op_arr)/sizeof(op_arr[0]), RELOP_LT);
}

void optimize_jbe(TranslationBlock *tb) {
	static RTLInstrOpcode op_arr[] = { OR, GET_CF, GET_ZF };
	optimize_jcc_helper(tb, op_arr, sizeof(op_arr)/sizeof(op_arr[0]), RELOP_LEU);
}

void optimize_jb(TranslationBlock *tb) {
	static RTLInstrOpcode op_arr[] = { GET_CF };
	optimize_jcc_helper(tb, op_arr, sizeof(op_arr)/sizeof(op_arr[0]), RELOP_LTU);
}

void optimize_jg(TranslationBlock *tb) {
	static RTLInstrOpcode op_arr[] = { XORI, OR, GET_ZF, XOR, GET_OF, GET_SF };
	optimize_jcc_helper(tb, op_arr, sizeof(op_arr)/sizeof(op_arr[0]), RELOP_GT);
}

void optimize_jge(TranslationBlock *tb) {
	static RTLInstrOpcode op_arr[] = { XORI, XOR, GET_OF, GET_SF };
	optimize_jcc_helper(tb, op_arr, sizeof(op_arr)/sizeof(op_arr[0]), RELOP_GE);
}

void optimize_ja(TranslationBlock *tb) {
	static RTLInstrOpcode op_arr[] = { XORI, OR, GET_CF, GET_ZF };
	optimize_jcc_helper(tb, op_arr, sizeof(op_arr)/sizeof(op_arr[0]), RELOP_GTU);
}

void optimize_jae(TranslationBlock *tb) {
	static RTLInstrOpcode op_arr[] = { XORI, GET_CF };
	optimize_jcc_helper(tb, op_arr, sizeof(op_arr)/sizeof(op_arr[0]), RELOP_GEU);
}

void optimize_jne(TranslationBlock *tb) {
	static RTLInstrOpcode op_arr[] = { XORI, GET_ZF };
	optimize_jcc_helper(tb, op_arr, sizeof(op_arr)/sizeof(op_arr[0]), RELOP_NE);
}

void optimize_je(TranslationBlock *tb) {
	static RTLInstrOpcode op_arr[] = { GET_ZF };
	optimize_jcc_helper(tb, op_arr, sizeof(op_arr)/sizeof(op_arr[0]), RELOP_EQ);
}

void optimize_jcc(TranslationBlock *tb) {
	optimize_jle(tb);
	optimize_jl(tb);
	optimize_jbe(tb);
	optimize_jb(tb);

	optimize_jg(tb);
	optimize_jge(tb);
	optimize_ja(tb);
	optimize_jae(tb);

	optimize_jne(tb);
	optimize_je(tb);
}

