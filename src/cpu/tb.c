#include "cpu/tb.h"
#include "cpu/rtl-interpret.h"

static void (*interpret_rtl_lr[3])(rtlreg_t* , int) = { 
	interpret_rtl_lr_l, interpret_rtl_lr_w, interpret_rtl_lr_b
};

static void (*interpret_rtl_sr[3])(int, const rtlreg_t*) = { 
	interpret_rtl_sr_l, interpret_rtl_sr_w, interpret_rtl_sr_b
};

static void (*interpret_rtl_set_eflags[4])(const rtlreg_t*) = {
	interpret_rtl_set_CF, interpret_rtl_set_OF,
	interpret_rtl_set_ZF, interpret_rtl_set_SF,
};

static void (*interpret_rtl_get_eflags[4])(rtlreg_t*) = {
	interpret_rtl_get_CF, interpret_rtl_get_OF,
	interpret_rtl_get_ZF, interpret_rtl_get_SF,
};

static void (*interpret_rtl_arith_logic[16])(
		rtlreg_t*, const rtlreg_t*, const rtlreg_t*) = {
	interpret_rtl_add, interpret_rtl_sub, interpret_rtl_and, interpret_rtl_or,
	interpret_rtl_xor, interpret_rtl_shl, interpret_rtl_shr, interpret_rtl_sar,
	interpret_rtl_mul_lo, interpret_rtl_mul_hi, interpret_rtl_imul_lo, interpret_rtl_imul_hi,
	interpret_rtl_div_q, interpret_rtl_div_r, interpret_rtl_idiv_q, interpret_rtl_idiv_r,
};

static void (*interpret_rtl_arith_logic_imm[16])(
		rtlreg_t* dest, const rtlreg_t* src1, int imm) = {
	interpret_rtl_addi, interpret_rtl_subi, interpret_rtl_andi, interpret_rtl_ori,
	interpret_rtl_xori, interpret_rtl_shli, interpret_rtl_shri, interpret_rtl_sari,
	interpret_rtl_mul_loi, interpret_rtl_mul_hii, 
	interpret_rtl_imul_loi, interpret_rtl_imul_hii,
	interpret_rtl_div_qi, interpret_rtl_div_ri, 
	interpret_rtl_idiv_qi, interpret_rtl_idiv_ri,
};

void interpret_tblock(const TranslationBlock *tb) {
	RTLInstr *rtl;
	list_for_each_entry(rtl, &tb->rtl_instr_list.list, list) {
		switch (rtl->type) {
			case J:
				interpret_rtl_j(rtl->j.target);
				break;
			case JR:
				interpret_rtl_jr(rtl->jr.target);
				break;
			case JRELOP:
				interpret_rtl_jrelop(rtl->jrelop.relop, rtl->jrelop.src1,
						rtl->jrelop.src2, rtl->jrelop.target);
				break;
			case SETRELOP:
				interpret_rtl_setrelop(rtl->setrelop.relop, rtl->setrelop.dest,
						rtl->setrelop.src1, rtl->setrelop.src2);
				break;
			case EXIT:
				interpret_rtl_exit(rtl->exit.state);
				break;
			case LI:
				interpret_rtl_li(rtl->li.dest, rtl->li.imm);
				break;
			case LM:
				interpret_rtl_lm(rtl->lm.dest, rtl->lm.addr, rtl->lm.len);
				break;
			case SM:
				interpret_rtl_sm(rtl->sm.addr, rtl->sm.len, rtl->sm.src1);
				break;
			case LR_L: case LR_W: case LR_B: 
				interpret_rtl_lr[rtl->type - LR_L](rtl->lr.dest, rtl->lr.r);
				break;
			case SR_L: case SR_W: case SR_B:
				interpret_rtl_sr[rtl->type - SR_L](rtl->sr.r, rtl->sr.src1);
				break;
			case SET_CF: case SET_OF: case SET_ZF: case SET_SF:
				interpret_rtl_set_eflags[rtl->type - SET_CF](rtl->set_eflags.src);
				break;
			case GET_CF: case GET_OF: case GET_ZF: case GET_SF:
				interpret_rtl_get_eflags[rtl->type - GET_CF](rtl->get_eflags.dest);
				break;
			case ADD: case SUB: case AND: case OR: case XOR: 
			case SHL: case SHR: case SAR:
			case MUL_LO: case MUL_HI: case IMUL_LO: case IMUL_HI:
			case DIV_Q: case DIV_R: case IDIV_Q: case IDIV_R:
				interpret_rtl_arith_logic[rtl->type - ADD](
					rtl->alu.dest, rtl->alu.src1, rtl->alu.src2);
				break;
			case ADDI: case SUBI: case ANDI: case ORI: case XORI: 
			case SHLI: case SHRI: case SARI:
			case MUL_LOI: case MUL_HII: case IMUL_LOI: case IMUL_HII:
			case DIV_QI: case DIV_RI: case IDIV_QI: case IDIV_RI:
				interpret_rtl_arith_logic_imm[rtl->type - ADDI](
					rtl->alui.dest, rtl->alui.src1, rtl->alui.imm);
				break;
			default:
				TODO();
		}
	}
}
