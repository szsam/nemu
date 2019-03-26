#include <stdlib.h>
#include <stdarg.h>

#include "cpu/rtl.h"
#include "cpu/tb.h"

void generate_rtl(RTLOpcode op, int nb_args, ...) {
	va_list ap;
	va_start(ap, nb_args);

	RTLInsn *rtl = (RTLInsn *)malloc(sizeof(RTLInsn));
	list_add_tail(&rtl->list, &cur_tblock->rtl_insns);
	++cur_tblock->rtl_instr_cnt; 
	rtl->opcode = op;

	for (int i = 0; i < nb_args; i++) {
		rtl->args[i] = va_arg(ap, RTLArg);
	}
	
	va_end(ap);
}
