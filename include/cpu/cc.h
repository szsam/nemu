#ifndef __CC_H__
#define __CC_H__

static inline const char* get_cc_name(int subcode) {
  static const char *cc_name[] = {
    "o", "no", "b", "nb",
    "e", "ne", "be", "nbe",
    "s", "ns", "p", "np",
    "l", "nl", "le", "nle"
  };
  return cc_name[subcode];
}

void rtl_setcc(rtlreg_t*, uint8_t);

typedef enum {CC_OP_ADD, CC_OP_SUB, CC_OP_ADC, CC_OP_SBB, 
	CC_OP_INC, CC_OP_DEC, CC_OP_NEG, 
	CC_OP_AND, CC_OP_OR, CC_OP_XOR, 
	CC_OP_SHIFT}CC_OP;

#define MAKE_CC_OP(op, w) ((op << 3) | w)

#endif
