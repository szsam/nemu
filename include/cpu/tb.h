#ifndef __TB__H__
#define __TB__H__

#include "common.h"
#include "cpu/rtl.h"
#include "util/uthash.h"
#include "util/list.h"

typedef struct {
	vaddr_t eip_start;
	vaddr_t eip_end;
	int guest_instr_cnt;
	int rtl_instr_cnt;
	int exec_time;
	struct list_head rtl_insns;

	bool has_init_interp; 
	RTLInterp *rtl_interp;

	UT_hash_handle hh;
}TranslationBlock;

extern TranslationBlock *cur_tblock;

void interpret_tblock(TranslationBlock *);

void optimize_tblock(TranslationBlock *);

void print_tblock(const TranslationBlock *);


#endif
