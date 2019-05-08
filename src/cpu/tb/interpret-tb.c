#include "cpu/tb.h"
#include "rtl-interpret.h"

#define make_interpret_rtl(name, ...) \
	concat(lbl_, name): \
		concat(interpret_rtl_, name) (__VA_ARGS__); \
		++rtl; \
		goto *rtl->op_handler;

#define PTR(x) (rtlreg_t *)(x)
#define A0 rtl->args[0]
#define A1 rtl->args[1]
#define A2 rtl->args[2]
#define A3 rtl->args[3]

void interpret_tblock(TranslationBlock *tb) {
	static const void *dispatch_table[] = {
		&&lbl_j, &&lbl_jr, &&lbl_jrelop, &&lbl_setrelop, &&lbl_exit, &&lbl_li, &&lbl_lm, &&lbl_sm, 
		&&lbl_lr_l, &&lbl_lr_w, &&lbl_lr_b, &&lbl_sr_l, &&lbl_sr_w, &&lbl_sr_b,

		&&lbl_add, &&lbl_sub, &&lbl_and, &&lbl_or, &&lbl_xor, &&lbl_shl, &&lbl_shr, &&lbl_sar, 
		&&lbl_mul_lo, &&lbl_mul_hi, &&lbl_imul_lo, &&lbl_imul_hi,
		&&lbl_div_q, &&lbl_div_r, &&lbl_idiv_q, &&lbl_idiv_r,

		&&lbl_addi, &&lbl_subi, &&lbl_andi, &&lbl_ori, &&lbl_xori, &&lbl_shli, &&lbl_shri, &&lbl_sari, 
		&&lbl_mul_loi, &&lbl_mul_hii, &&lbl_imul_loi, &&lbl_imul_hii,
		&&lbl_div_qi, &&lbl_div_ri, &&lbl_idiv_qi, &&lbl_idiv_ri,

		&&lbl_div64_q, &&lbl_div64_r, &&lbl_idiv64_q, &&lbl_idiv64_r,

		&&lbl_set_CF, &&lbl_set_OF, &&lbl_set_ZF, &&lbl_set_SF,
		&&lbl_get_CF, &&lbl_get_OF, &&lbl_get_ZF, &&lbl_get_SF,

		&&lbl_pio_read, &&lbl_pio_write,

		&&lbl_eob, &&lbl_discard
	};

	if (!tb->has_init_interp) {
		tb->rtl_interp = (RTLInterp *)calloc(tb->rtl_instr_cnt + 1, sizeof(RTLInterp));
		RTLInsn *insn;
		int i = 0;
		list_for_each_entry(insn, &tb->rtl_insns, list) {
			tb->rtl_interp[i].op_handler = dispatch_table[insn->opcode];
			memcpy(tb->rtl_interp[i].args, insn->args, sizeof(insn->args));
			++i;
		}
		assert(i == tb->rtl_instr_cnt);
		tb->rtl_interp[i].op_handler = &&lbl_eob;
		tb->has_init_interp = 1;
	}

	RTLInterp *rtl = tb->rtl_interp;

	goto *rtl->op_handler;

	make_interpret_rtl(j, A0)
	make_interpret_rtl(jr, PTR(A0))

	make_interpret_rtl(jrelop, PTR(A0), PTR(A1), A2, A3)
	make_interpret_rtl(setrelop, PTR(A0), PTR(A1), PTR(A2), A3)

	make_interpret_rtl(exit, A0)
	make_interpret_rtl(li, PTR(A0), A1)
	make_interpret_rtl(lm, PTR(A0), PTR(A1), A2)
	make_interpret_rtl(sm, PTR(A0), A1, PTR(A2))

	make_interpret_rtl(lr_l, PTR(A0), A1)
	make_interpret_rtl(lr_w, PTR(A0), A1)
	make_interpret_rtl(lr_b, PTR(A0), A1)

	make_interpret_rtl(sr_l, A0, PTR(A1))
	make_interpret_rtl(sr_w, A0, PTR(A1))
	make_interpret_rtl(sr_b, A0, PTR(A1))

	make_interpret_rtl(set_CF, PTR(A0))
	make_interpret_rtl(set_OF, PTR(A0))
	make_interpret_rtl(set_ZF, PTR(A0))
	make_interpret_rtl(set_SF, PTR(A0))

	make_interpret_rtl(get_CF, PTR(A0))
	make_interpret_rtl(get_OF, PTR(A0))
	make_interpret_rtl(get_ZF, PTR(A0))
	make_interpret_rtl(get_SF, PTR(A0))

	make_interpret_rtl(add    , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(sub    , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(and    , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(or     , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(xor    , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(shl    , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(shr    , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(sar    , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(mul_lo , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(mul_hi , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(imul_lo, PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(imul_hi, PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(div_q  , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(div_r  , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(idiv_q , PTR(A0), PTR(A1), PTR(A2))
	make_interpret_rtl(idiv_r , PTR(A0), PTR(A1), PTR(A2))

	make_interpret_rtl(addi    , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(subi    , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(andi    , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(ori     , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(xori    , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(shli    , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(shri    , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(sari    , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(mul_loi , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(mul_hii , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(imul_loi, PTR(A0), PTR(A1), A2)
	make_interpret_rtl(imul_hii, PTR(A0), PTR(A1), A2)
	make_interpret_rtl(div_qi  , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(div_ri  , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(idiv_qi , PTR(A0), PTR(A1), A2)
	make_interpret_rtl(idiv_ri , PTR(A0), PTR(A1), A2)

	make_interpret_rtl(div64_q,   PTR(A0), PTR(A1), PTR(A2), PTR(A3))
	make_interpret_rtl(div64_r,   PTR(A0), PTR(A1), PTR(A2), PTR(A3))
	make_interpret_rtl(idiv64_q,  PTR(A0), PTR(A1), PTR(A2), PTR(A3))
	make_interpret_rtl(idiv64_r,  PTR(A0), PTR(A1), PTR(A2), PTR(A3))

	make_interpret_rtl(pio_read,  PTR(A0), PTR(A1), A2)
	make_interpret_rtl(pio_write, PTR(A0), PTR(A1), A2)

	lbl_discard: ++rtl; goto *rtl->op_handler;
	lbl_eob: return;
}
