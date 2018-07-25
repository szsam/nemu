#ifndef __CPU_RTL_H__
#define __CPU_RTL_H__

#include "nemu.h"
// #include "util/c_op.h"
#include "cpu/relop.h"
#include "cpu/rtl-wrapper.h"
#include "cpu/cc.h"
// #include "cpu/rtl-generate.h"
#include "util/list.h"

extern rtlreg_t t0, t1, t2, t3, at;
extern const rtlreg_t tzero;
extern rtlreg_t cc_op, cc_res, cc_dest, cc_src;

typedef enum {
	J, JR, JRELOP, SETRELOP, EXIT, LI, LM, SM, 
	LR_L, LR_W, LR_B, SR_L, SR_W, SR_B,

	ADD, SUB, AND, OR, XOR, SHL, SHR, SAR, 
	MUL_LO, MUL_HI, IMUL_LO, IMUL_HI,
	DIV_Q, DIV_R, IDIV_Q, IDIV_R,

	ADDI, SUBI, ANDI, ORI, XORI, SHLI, SHRI, SARI, 
	MUL_LOI, MUL_HII, IMUL_LOI, IMUL_HII,
	DIV_QI, DIV_RI, IDIV_QI, IDIV_RI,

	DIV64_Q, DIV64_R, IDIV64_Q, IDIV64_R,

	SET_CF, SET_OF, SET_ZF, SET_SF,
	GET_CF, GET_OF, GET_ZF, GET_SF,

	CC_SET_OP,

	PIO_READ, PIO_WRITE,

	EOB
} RTLInstrOpcode;

typedef struct  {
	RTLInstrOpcode opcode;
	const void *op_handler_label;

	union {
		rtlreg_t *r1;
		vaddr_t target;
	};
	const rtlreg_t *r2;
	const rtlreg_t *r3;
	const rtlreg_t *r4;

	union {
		uint32_t relop;
		int state;
		uint32_t imm;
		int len;
		int r;
	};

}RTLInstr;

typedef struct {
	RTLInstr rtl_instr;
	struct list_head list;
}RTLInstrListItem;

/* RTL basic instructions */

void generate_rtl_j(vaddr_t target);

void generate_rtl_jr(rtlreg_t *target);

void generate_rtl_jrelop(uint32_t relop,
    const rtlreg_t *src1, const rtlreg_t *src2, vaddr_t target);

void generate_rtl_setrelop(uint32_t relop, rtlreg_t *dest,
    const rtlreg_t *src1, const rtlreg_t *src2);

void generate_rtl_exit(int state);

void generate_rtl_li(rtlreg_t* dest, uint32_t imm);

#define make_generate_rtl_arith_logic(name) \
  void concat(generate_rtl_, name) (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2); \
  void concat3(generate_rtl_, name, i) (rtlreg_t* dest, const rtlreg_t* src1, int imm);

make_generate_rtl_arith_logic(add)
make_generate_rtl_arith_logic(sub)
make_generate_rtl_arith_logic(and)
make_generate_rtl_arith_logic(or)
make_generate_rtl_arith_logic(xor)
make_generate_rtl_arith_logic(shl)
make_generate_rtl_arith_logic(shr)
make_generate_rtl_arith_logic(sar)
make_generate_rtl_arith_logic(mul_lo)
make_generate_rtl_arith_logic(mul_hi)
make_generate_rtl_arith_logic(imul_lo)
make_generate_rtl_arith_logic(imul_hi)
make_generate_rtl_arith_logic(div_q)
make_generate_rtl_arith_logic(div_r)
make_generate_rtl_arith_logic(idiv_q)
make_generate_rtl_arith_logic(idiv_r)

void generate_rtl_div64_q(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2);

void generate_rtl_div64_r(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2);

void generate_rtl_idiv64_q(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2);

void generate_rtl_idiv64_r(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2);

void generate_rtl_lm(rtlreg_t *dest, const rtlreg_t* addr, int len);

void generate_rtl_sm(const rtlreg_t* addr, int len, const rtlreg_t* src1);

void generate_rtl_lr_l(rtlreg_t* dest, int r);

void generate_rtl_lr_w(rtlreg_t* dest, int r);

void generate_rtl_lr_b(rtlreg_t* dest, int r);

void generate_rtl_sr_l(int r, const rtlreg_t* src1);

void generate_rtl_sr_w(int r, const rtlreg_t* src1);

void generate_rtl_sr_b(int r, const rtlreg_t* src1);

#define make_generate_rtl_setget_eflags(f) \
  void concat(generate_rtl_set_, f) (const rtlreg_t* src); \
  void concat(generate_rtl_get_, f) (rtlreg_t* dest);

make_generate_rtl_setget_eflags(CF)
make_generate_rtl_setget_eflags(OF)
make_generate_rtl_setget_eflags(ZF)
make_generate_rtl_setget_eflags(SF)

void generate_rtl_cc_set_op(int, const rtlreg_t *res, 
		const rtlreg_t *dest, const rtlreg_t *src);

void generate_rtl_pio_read(rtlreg_t *dest, const rtlreg_t *addr, int len);
void generate_rtl_pio_write(const rtlreg_t *addr, const rtlreg_t *src, int len);

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t** dest, int r, int width) {
  switch (width) {
    case 4: // rtl_lr_l(*dest, r); return;
		*dest = &reg_l(r); return;
    case 1: rtl_lr_b(*dest, r); return;
    case 2: rtl_lr_w(*dest, r); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, int width, const rtlreg_t* src1) {
  switch (width) {
    case 4: // rtl_sr_l(r, src1); return;
		if (&reg_l(r) != src1) {
			rtl_sr_l(r, src1);
		} 
		return;
    case 1: rtl_sr_b(r, src1); return;
    case 2: rtl_sr_w(r, src1); return;
    default: assert(0);
  }
}

static inline void rtl_mv(rtlreg_t* dest, const rtlreg_t *src1) {
  // dest <- src1
  rtl_add(dest, src1, &tzero);
}

// void cc_gen_ZF(rtlreg_t *);
// void cc_gen_SF(rtlreg_t *);
// void cc_gen_OF(rtlreg_t *);
// void cc_gen_CF(rtlreg_t *);
// void cc_set_op(int, int, rtlreg_t *, rtlreg_t *);

static inline void rtl_not(rtlreg_t* dest) {
  // dest <- ~dest
  rtl_xori(dest, dest, 0xffffffff);
}

static inline void rtl_sext(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  int n = 32 - width * 8;
  // *dest = (int32_t)(*src1 << n) >> n;
  if (n != 0) {
	rtl_shli(dest, src1, n);
  	rtl_sari(dest, dest, n);
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  
  rtl_subi(&cpu.esp, &cpu.esp, 4);
  rtl_sm(&cpu.esp, 4, src1);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  rtl_lm(dest, &cpu.esp, 4);
  rtl_addi(&cpu.esp, &cpu.esp, 4);
}

static inline void rtl_eq0(rtlreg_t* dest, const rtlreg_t* src1) {
  // dest <- (src1 == 0 ? 1 : 0)
  rtl_setrelop(RELOP_EQ, dest, src1, &tzero);
}

static inline void rtl_eqi(rtlreg_t* dest, const rtlreg_t* src1, int imm) {
  // dest <- (src1 == imm ? 1 : 0)
  TODO();
}

static inline void rtl_neq0(rtlreg_t* dest, const rtlreg_t* src1) {
  // dest <- (src1 != 0 ? 1 : 0)
  TODO();
}

static inline void rtl_msb(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  // *dest = (*src1 >> (width * 8 - 1)) & 0x1;
  int n = width * 8 - 1;
  rtl_shri(dest, src1, n);
  rtl_andi(dest, dest, 0x1);
}

// static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
//   // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
//   // cpu.eflags.ZF = (*result & mask) == 0;
//   if (width != 4) {
//     uint32_t mask = 0xffffffffu >> (32 - 8 * width);
//     rtl_andi(&at, result, mask);
//     rtl_eq0(&at, &at);
//   }
//   else
//     rtl_eq0(&at, result);
//   rtl_set_ZF(&at);
// }
// 
// static inline void rtl_update_SF(const rtlreg_t* result, int width) {
//   // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
//   // cpu.eflags.SF = (*result >> (8 * width - 1)) & 0x1;
//   rtl_msb(&at, result, width);
//   rtl_set_SF(&at);
// }
// 
// static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
//   rtl_update_ZF(result, width);
//   rtl_update_SF(result, width);
// }

#endif
