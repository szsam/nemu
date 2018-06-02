#ifndef __TB__H__
#define __TB__H__

#include "common.h"
#include "cpu/rtl.h"
#include "util/uthash.h"

typedef struct {
	vaddr_t eip_start;
	vaddr_t eip_end;
	RTLInstr rtl_instr_list;
	int guest_instr_cnt;

	UT_hash_handle hh;
}TranslationBlock;

extern TranslationBlock *cur_tblock;

void interpret_tblock(const TranslationBlock *);

#endif
