#include "cpu/tb.h"
#include "cpu/decode.h"
#include <inttypes.h>

#define make_print_rtl_case(name, fmtstr, ...) \
	case concat(RTLOP_, name): \
		Log_write("rtl_" #name "(" fmtstr ")\n", __VA_ARGS__); \
		break;

// rtl register: arg to string
const char *a2s(RTLArg arg) {
	if (arg >= (RTLArg)cpu.gpr && arg < (RTLArg)(cpu.gpr + 8)) {
		return reg_name((rtlreg_t *)arg - &reg_l(0), 4);
	}
    else if (arg == (RTLArg)&tzero)         return "tzero";
    else if (arg == (RTLArg)&t0)            return "t0";
    else if (arg == (RTLArg)&t1)            return "t1";
    else if (arg == (RTLArg)&t2)            return "t2";
    else if (arg == (RTLArg)&t3)            return "t3";
    else if (arg == (RTLArg)&at)            return "at";
    else if (arg == (RTLArg)&id_dest->data) return "id_dest->data";
    else if (arg == (RTLArg)&id_src->data)  return "id_src->data";
    else if (arg == (RTLArg)&id_src2->data) return "id_src2->data";
    else if (arg == (RTLArg)&id_dest->addr) return "id_dest->addr";
    else if (arg == (RTLArg)&id_src->addr)  return "id_src->addr";
    else if (arg == (RTLArg)&id_src2->addr) return "id_src2->addr";
	else assert(0);
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

			make_print_rtl_case(jrelop, "%" PRIdPTR ", %s, %s, 0x%" PRIxPTR, A0, a2s(A1), a2s(A2), A3)
			make_print_rtl_case(setrelop, "%" PRIdPTR ", %s, %s, %s", A0, a2s(A1), a2s(A2), a2s(A3))

			make_print_rtl_case(exit, "%" PRIdPTR, A0)
			make_print_rtl_case(li, "%s, 0x%" PRIxPTR, a2s(A0), A1)
			make_print_rtl_case(lm, "%s, (%s), %" PRIdPTR, a2s(A0), a2s(A1), A2)
			make_print_rtl_case(sm, "(%s), %" PRIdPTR ", %s", a2s(A0), A1, a2s(A2))

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

			make_print_rtl_case(cc_set_op, "0x%" PRIxPTR ", %s, %s, %s", A0, a2s(A1), a2s(A2), a2s(A3));

			default:
				assert(0);
		}
	}
}
