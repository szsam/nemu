#ifndef __TB__H__
#define __TB__H__

#include "common.h"
#include "cpu/rtl.h"
#include "util/uthash.h"

typedef struct {
	vaddr_t eip_start;
	vaddr_t eip_end;
	int guest_instr_cnt;
	int rtl_instr_cnt;
	bool has_saved_label;
	int exec_time;
	RTLInstrListItem rtl_instr_list;
	RTLInstr *rtl_instr_arr;

	UT_hash_handle hh;
}TranslationBlock;

extern TranslationBlock *cur_tblock;

void interpret_tblock(TranslationBlock *);

void optimize_tblock(TranslationBlock *);

void print_tblock(const TranslationBlock *);


// extern const rtlreg_t * const rtl_registers[];
 
enum { R_DEST_VAL = R_EDI + 1, R_SRC_VAL,
	R_TZERO, R_T0, R_T1, R_T2, R_T3, R_AT,
	R_SRC2_VAL,
	R_DEST_ADDR, R_SRC_ADDR, R_SRC2_ADDR,
};

#define NR_RTLREG 20

int rtlreg_a2n(const rtlreg_t *);
#define a2n(a) rtlreg_a2n(a)

#endif
