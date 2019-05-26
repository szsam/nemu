#include "cpu/tb.h"
#include "cpu/decode.h"
#include <inttypes.h>

#define make_print_rtl_case(name, fmtstr, ...) \
	case concat(RTLOP_, name): \
		Log_write("rtl_" #name "(" fmtstr ")\n", __VA_ARGS__); \
		break;

const char *rtlreg_name[] = {
	"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi",
	"cc_op", "cc_res", "cc_dest", "cc_src",
	"t0", "t1", "t2", "t3", "at", "tzero",
	"mdr0", "mdr1", "mdr2", "mar0", "mar1", "mar2"
};

// arg to rtlreg's name
const char *a2s(RTLArg arg) {
	int index = (rtlreg_t *)arg - cpu.rtlregs;
	return rtlreg_name[index];
}

#define A0 insn->args[0]
#define A1 insn->args[1]
#define A2 insn->args[2]
#define A3 insn->args[3]

void print_tblock(const TranslationBlock *tb) {
	RTLInsn *insn;
	list_for_each_entry(insn, &tb->rtl_insns, list) {
		switch (insn->opcode) {
			make_print_rtl_case(j, "0x%" PRIxPTR, A0)
			make_print_rtl_case(jr, "%s", a2s(A0))

			make_print_rtl_case(jrelop,  "%s, %s, %" PRIdPTR ", 0x%" PRIxPTR, a2s(A0), a2s(A1), A2, A3)
			make_print_rtl_case(setrelop, "%s, %s, %s, %" PRIdPTR, a2s(A0), a2s(A1), a2s(A2), A3)

			make_print_rtl_case(exit, "%" PRIdPTR, A0)
			make_print_rtl_case(li, "%s, 0x%" PRIxPTR, a2s(A0), A1)
			make_print_rtl_case(lm, "%s, (%s), %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(sm, "(%s), %" PRIdPTR ", %s", a2s(A0), A1, a2s(A2))

			make_print_rtl_case(host_lm, "%s, 0x%" PRIxPTR ", %" PRIdPTR, a2s(A0), A1, A2)
			make_print_rtl_case(host_sm, "%s, 0x%" PRIxPTR ", %" PRIdPTR, a2s(A0), A1, A2)

			make_print_rtl_case(lr_l, "%s, %s", a2s(A0), reg_name(A1, 4))
			make_print_rtl_case(lr_w, "%s, %s", a2s(A0), reg_name(A1, 2))
			make_print_rtl_case(lr_b, "%s, %s", a2s(A0), reg_name(A1, 1))

			make_print_rtl_case(sr_l, "%s, %s", reg_name(A0, 4), a2s(A1))
			make_print_rtl_case(sr_w, "%s, %s", reg_name(A0, 2), a2s(A1))
			make_print_rtl_case(sr_b, "%s, %s", reg_name(A0, 1), a2s(A1))

			make_print_rtl_case(set_CF, "%s", a2s(A0))
			make_print_rtl_case(set_OF, "%s", a2s(A0))
			make_print_rtl_case(set_ZF, "%s", a2s(A0))
			make_print_rtl_case(set_SF, "%s", a2s(A0))

			make_print_rtl_case(get_CF, "%s", a2s(A0))
			make_print_rtl_case(get_OF, "%s", a2s(A0))
			make_print_rtl_case(get_ZF, "%s", a2s(A0))
			make_print_rtl_case(get_SF, "%s", a2s(A0))

			make_print_rtl_case(add    , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(sub    , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(and    , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(or     , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(xor    , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(shl    , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(shr    , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(sar    , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(mul_lo , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(mul_hi , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(imul_lo, "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(imul_hi, "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(div_q  , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(div_r  , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(idiv_q , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))
			make_print_rtl_case(idiv_r , "%s, %s, %s", a2s(A0), a2s(A1), a2s(A2))

			make_print_rtl_case(addi    , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(subi    , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(andi    , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(ori     , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(xori    , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(shli    , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(shri    , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(sari    , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(mul_loi , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(mul_hii , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(imul_loi, "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(imul_hii, "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(div_qi  , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(div_ri  , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(idiv_qi , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(idiv_ri , "%s, %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)

			make_print_rtl_case(div64_q,  "%s, %s, %s, %s", a2s(A0), a2s(A1), a2s(A2), a2s(A3))
			make_print_rtl_case(div64_r,  "%s, %s, %s, %s", a2s(A0), a2s(A1), a2s(A2), a2s(A3))
			make_print_rtl_case(idiv64_q, "%s, %s, %s, %s", a2s(A0), a2s(A1), a2s(A2), a2s(A3))
			make_print_rtl_case(idiv64_r, "%s, %s, %s, %s", a2s(A0), a2s(A1), a2s(A2), a2s(A3))

			make_print_rtl_case(pio_read, "%s, (%s), %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(pio_write, "(%s), %s, %" PRIdPTR, a2s(A0), a2s(A1), A2)

			make_print_rtl_case(discard, "%s", a2s(A0));

			default:
				assert(0);
		}
	}
}
