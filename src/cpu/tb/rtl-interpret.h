#ifndef __CPU_RTL_INTERPRET_H__
#define __CPU_RTL_INTERPRET_H__

#include "nemu.h"
#include "util/c_op.h"
#include "cpu/relop.h"
#include "cpu/rtl.h"
// #include "cpu/rtl-wrapper.h"
#include "device/port-io.h"

// extern rtlreg_t t0, t1, t2, t3, at;
// extern const rtlreg_t tzero;

void decoding_set_jmp(bool is_jmp);
bool interpret_relop(uint32_t relop, const rtlreg_t src1, const rtlreg_t src2);

/* RTL basic instructions */

static inline void interpret_rtl_j(vaddr_t target) {
  cpu.eip = target;
//  decoding_set_jmp(true);
}

static inline void interpret_rtl_jr(rtlreg_t *target) {
  cpu.eip = *target;
//  decoding_set_jmp(true);
}

static inline void interpret_rtl_jrelop(uint32_t relop,
    const rtlreg_t *src1, const rtlreg_t *src2, vaddr_t target) {
  bool is_jmp = interpret_relop(relop, *src1, *src2);
  if (is_jmp) cpu.eip = target;
//  decoding_set_jmp(is_jmp);
}

static inline void interpret_rtl_setrelop(uint32_t relop, rtlreg_t *dest,
    const rtlreg_t *src1, const rtlreg_t *src2) {
  *dest = interpret_relop(relop, *src1, *src2);
}

void interpret_rtl_exit(int state);

static inline void interpret_rtl_li(rtlreg_t* dest, uint32_t imm) {
  *dest = imm;
}

#define make_rtl_arith_logic(name) \
  static inline void concat(interpret_rtl_, name) (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = concat(c_, name) (*src1, *src2); \
  } \
  static inline void concat3(interpret_rtl_, name, i) (rtlreg_t* dest, const rtlreg_t* src1, int imm) { \
    *dest = concat(c_, name) (*src1, imm); \
  }

make_rtl_arith_logic(add)
make_rtl_arith_logic(sub)
make_rtl_arith_logic(and)
make_rtl_arith_logic(or)
make_rtl_arith_logic(xor)
make_rtl_arith_logic(shl)
make_rtl_arith_logic(shr)
make_rtl_arith_logic(sar)
make_rtl_arith_logic(mul_lo)
make_rtl_arith_logic(mul_hi)
make_rtl_arith_logic(imul_lo)
make_rtl_arith_logic(imul_hi)
make_rtl_arith_logic(div_q)
make_rtl_arith_logic(div_r)
make_rtl_arith_logic(idiv_q)
make_rtl_arith_logic(idiv_r)

static inline void interpret_rtl_div64_q(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline void interpret_rtl_div64_r(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend % divisor;
}

static inline void interpret_rtl_idiv64_q(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline void interpret_rtl_idiv64_r(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend % divisor;
}

static inline void interpret_rtl_lm(rtlreg_t *dest, const rtlreg_t* addr, int len) {
  *dest = vaddr_read(*addr, len);
}

static inline void interpret_rtl_sm(const rtlreg_t* addr, int len, const rtlreg_t* src1) {
  vaddr_write(*addr, len, *src1);
}

static inline void interpret_rtl_lr_l(rtlreg_t* dest, int r) {
  *dest = reg_l(r);
}

static inline void interpret_rtl_lr_w(rtlreg_t* dest, int r) {
  *dest = reg_w(r);
}

static inline void interpret_rtl_lr_b(rtlreg_t* dest, int r) {
  *dest = reg_b(r);
}

static inline void interpret_rtl_sr_l(int r, const rtlreg_t* src1) {
  reg_l(r) = *src1;
}

static inline void interpret_rtl_sr_w(int r, const rtlreg_t* src1) {
  reg_w(r) = *src1;
}

static inline void interpret_rtl_sr_b(int r, const rtlreg_t* src1) {
  reg_b(r) = *src1;
}

#define make_rtl_setget_eflags(f) \
  static inline void concat(interpret_rtl_set_, f) (const rtlreg_t* src) { \
    cpu.eflags.f = *src; \
  } \
  void concat(interpret_rtl_get_, f) (rtlreg_t* dest);

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

static inline 
void interpret_rtl_pio_read(rtlreg_t *dest, const rtlreg_t *addr, int len) {
	*dest = pio_read(*addr, len);
}

static inline
void interpret_rtl_pio_write(const rtlreg_t *addr, const rtlreg_t *src, int len) {
	pio_write(*addr, *src, len);
}

static inline
void interpret_rtl_cc_set_op(int op, const rtlreg_t *res,
		const rtlreg_t *dest, const rtlreg_t *src) {
	cc_op = op;
	cc_res = *res;
	cc_dest = *dest;
	cc_src = *src;
}

#endif
