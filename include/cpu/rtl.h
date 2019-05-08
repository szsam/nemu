#ifndef __CPU_RTL_H__
#define __CPU_RTL_H__

#include "nemu.h"
// #include "util/c_op.h"
#include "cpu/relop.h"
#include "cpu/rtl-wrapper.h"
#include "cpu/cc.h"
// #include "cpu/rtl-generate.h"
#include "util/list.h"

// extern rtlreg_t t0, t1, t2, t3, at;
// extern const rtlreg_t tzero;
#define t0 cpu.t0
#define t1 cpu.t1
#define t2 cpu.t2
#define t3 cpu.t3
#define at cpu.at
#define tzero cpu.tzero

typedef enum {
	RTLOP_j, RTLOP_jr, RTLOP_jrelop, RTLOP_setrelop, RTLOP_exit, RTLOP_li, RTLOP_lm, RTLOP_sm, 
	RTLOP_lr_l, RTLOP_lr_w, RTLOP_lr_b, RTLOP_sr_l, RTLOP_sr_w, RTLOP_sr_b,

	RTLOP_add, RTLOP_sub, RTLOP_and, RTLOP_or, RTLOP_xor, RTLOP_shl, RTLOP_shr, RTLOP_sar, 
	RTLOP_mul_lo, RTLOP_mul_hi, RTLOP_imul_lo, RTLOP_imul_hi,
	RTLOP_div_q, RTLOP_div_r, RTLOP_idiv_q, RTLOP_idiv_r,

	RTLOP_addi, RTLOP_subi, RTLOP_andi, RTLOP_ori, RTLOP_xori, RTLOP_shli, RTLOP_shri, RTLOP_sari, 
	RTLOP_mul_loi, RTLOP_mul_hii, RTLOP_imul_loi, RTLOP_imul_hii,
	RTLOP_div_qi, RTLOP_div_ri, RTLOP_idiv_qi, RTLOP_idiv_ri,

	RTLOP_div64_q, RTLOP_div64_r, RTLOP_idiv64_q, RTLOP_idiv64_r,

	RTLOP_set_CF, RTLOP_set_OF, RTLOP_set_ZF, RTLOP_set_SF,
	RTLOP_get_CF, RTLOP_get_OF, RTLOP_get_ZF, RTLOP_get_SF,

	RTLOP_pio_read, RTLOP_pio_write,

	RTLOP_eob, RTLOP_discard
} RTLOpcode;

typedef uintptr_t RTLArg;

#define RTL_NUM_ARGS 4

typedef struct  {
	RTLOpcode opcode;
	RTLArg args[RTL_NUM_ARGS];
	struct list_head list;
}RTLInsn;

typedef struct {
	const void *op_handler;
	RTLArg args[RTL_NUM_ARGS];
}RTLInterp;

void generate_rtl(RTLOpcode, int, ...);

/* RTL basic instructions */

static inline void generate_rtl_j(vaddr_t target) {
	generate_rtl(RTLOP_j, 1, target);
}

static inline void generate_rtl_jr(rtlreg_t *target) {
	generate_rtl(RTLOP_jr, 1, target);
}

static inline void generate_rtl_jrelop(const rtlreg_t *src1, 
		const rtlreg_t *src2, uint32_t relop, vaddr_t target) {
	generate_rtl(RTLOP_jrelop, 4, src1, src2, relop, target);
}

static inline void generate_rtl_setrelop(rtlreg_t *dest,
    const rtlreg_t *src1, const rtlreg_t *src2, uint32_t relop) {
	generate_rtl(RTLOP_setrelop, 4, dest, src1, src2, relop);
}

static inline void generate_rtl_exit(int state) {
	generate_rtl(RTLOP_exit, 1, state);
}

static inline void generate_rtl_li(rtlreg_t* dest, uint32_t imm) {
	generate_rtl(RTLOP_li, 2, dest, imm);
}

#define make_generate_rtl_arith_logic(name) \
  static inline \
  void concat(generate_rtl_, name) (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
	  generate_rtl(concat(RTLOP_, name), 3, dest, src1, src2); \
  } \
  static inline \
  void concat3(generate_rtl_, name, i) (rtlreg_t* dest, const rtlreg_t* src1, int imm) { \
	  generate_rtl(concat3(RTLOP_, name, i), 3, dest, src1, imm); \
  }

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

static inline void generate_rtl_div64_q(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
	generate_rtl(RTLOP_div64_q, 4, dest, src1_hi, src1_lo, src2);
}

static inline void generate_rtl_div64_r(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
	generate_rtl(RTLOP_div64_r, 4, dest, src1_hi, src1_lo, src2);
}

static inline void generate_rtl_idiv64_q(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
	generate_rtl(RTLOP_idiv64_q, 4, dest, src1_hi, src1_lo, src2);
}

static inline void generate_rtl_idiv64_r(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
	generate_rtl(RTLOP_idiv64_r, 4, dest, src1_hi, src1_lo, src2);
}

static inline void generate_rtl_lm(rtlreg_t *dest, const rtlreg_t* addr, int len) {
	generate_rtl(RTLOP_lm, 3, dest, addr, len);
}

static inline void generate_rtl_sm(const rtlreg_t* addr, int len, const rtlreg_t* src1) {
	generate_rtl(RTLOP_sm, 3, addr, len, src1);
}

static inline void generate_rtl_lr_l(rtlreg_t* dest, int r) {
	generate_rtl(RTLOP_lr_l, 2, dest, r);
}

static inline void generate_rtl_lr_w(rtlreg_t* dest, int r) {
	generate_rtl(RTLOP_lr_w, 2, dest, r);
}

static inline void generate_rtl_lr_b(rtlreg_t* dest, int r) {
	generate_rtl(RTLOP_lr_b, 2, dest, r);
}

static inline void generate_rtl_sr_l(int r, const rtlreg_t* src1) {
	generate_rtl(RTLOP_sr_l, 2, r, src1);
}

static inline void generate_rtl_sr_w(int r, const rtlreg_t* src1) {
	generate_rtl(RTLOP_sr_w, 2, r, src1);
}

static inline void generate_rtl_sr_b(int r, const rtlreg_t* src1) {
	generate_rtl(RTLOP_sr_b, 2, r, src1);
}

#define make_generate_rtl_setget_eflags(f) \
  static inline void concat(generate_rtl_set_, f) (const rtlreg_t* src) { \
	  generate_rtl(concat(RTLOP_set_, f), 1, src); \
  } \
  static inline void concat(generate_rtl_get_, f) (rtlreg_t* dest) { \
	  generate_rtl(concat(RTLOP_get_, f), 1, dest); \
  }

make_generate_rtl_setget_eflags(CF)
make_generate_rtl_setget_eflags(OF)
make_generate_rtl_setget_eflags(ZF)
make_generate_rtl_setget_eflags(SF)

// static inline void generate_rtl_cc_set_op(int op, const rtlreg_t *res, 
// 		const rtlreg_t *dest, const rtlreg_t *src) {
// 	generate_rtl(RTLOP_cc_set_op, 4, op, res, dest, src);
// }

static inline void generate_rtl_pio_read(rtlreg_t *dest, const rtlreg_t *addr, int len) {
	generate_rtl(RTLOP_pio_read, 3, dest, addr, len);
}

static inline void generate_rtl_pio_write(const rtlreg_t *addr, const rtlreg_t *src, int len) {
	generate_rtl(RTLOP_pio_write, 3, addr, src, len);
}

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
  rtl_setrelop(dest, src1, &tzero, RELOP_EQ);
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

static inline void rtl_discard(const rtlreg_t *addr) {
	generate_rtl(RTLOP_discard, 1, addr);
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
