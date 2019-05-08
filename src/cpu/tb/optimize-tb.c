#include "cpu/tb.h"
#include "cpu/decode.h"
#include "cpu/reg.h"

void dead_code_elimination(TranslationBlock *);
void constant_propogation(TranslationBlock *);
void remove_redundant_sr(TranslationBlock *tb);
void optimize_jcc(TranslationBlock *tb);

void optimize_tblock(TranslationBlock *tb) {
	// constant_propogation(tb);
	dead_code_elimination(tb);
	// remove_redundant_sr(tb);
	// optimize_jcc(tb);
}

