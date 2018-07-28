#include "cpu/rtl.h"

/* Condition Code */
rtlreg_t cc_op, cc_res, cc_dest, cc_src;

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O: rtl_get_OF(dest); break;
    case CC_B: rtl_get_CF(dest); break;
    case CC_E: rtl_get_ZF(dest); break;
    case CC_BE: // *dest = (cpu.eflags.ZF || cpu.eflags.CF); break;
		rtl_get_ZF(&t0);
		rtl_get_CF(&t1);
		rtl_or(dest, &t0, &t1);
		break;
    case CC_S: rtl_get_SF(dest);  break;
    case CC_L: // *dest = (cpu.eflags.SF != cpu.eflags.OF); break;
		rtl_get_SF(&t0);
		rtl_get_OF(&t1);
		rtl_xor(dest, &t0, &t1);
		break;
    case CC_LE: // *dest = (cpu.eflags.ZF || (cpu.eflags.SF != cpu.eflags.OF)); break;
		rtl_get_SF(&t0);
		rtl_get_OF(&t1);
		rtl_xor(&t0, &t0, &t1);
		rtl_get_ZF(&t1); 
		rtl_or(dest, &t0, &t1);
		break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}

void interpret_rtl_get_ZF(rtlreg_t* dest)
{
	int width = cc_op & 0x7;
	uint32_t mask = (~0u >> ((4 - width) << 3));
	*dest = ((cc_res & mask) == 0);
}

void interpret_rtl_get_SF(rtlreg_t* dest)
{
	int width = cc_op & 0x7;
	int n = width * 8 - 1;
	*dest = (cc_res >> n) & 0x1;
}

void interpret_rtl_get_CF(rtlreg_t* dest)
{
	switch (cc_op)
	{
		case MAKE_CC_OP(CC_OP_ADD, 4):
		{
			uint32_t res = cc_res;
			*dest = __builtin_add_overflow((uint32_t)cc_dest, (uint32_t)cc_src, &res);
			break;
		}
		case MAKE_CC_OP(CC_OP_SUB, 4):
		{
			uint32_t res = cc_res;
			*dest = __builtin_sub_overflow((uint32_t)cc_dest, (uint32_t)cc_src, &res);
			break;
		}
		case MAKE_CC_OP(CC_OP_SUB, 1):
		{
			uint8_t res = cc_res;
			*dest = __builtin_sub_overflow((uint8_t)cc_dest, (uint8_t)cc_src, &res);
			break;
		}
		case MAKE_CC_OP(CC_OP_SUB, 2):
		{
			uint16_t res = cc_res;
			*dest = __builtin_sub_overflow((uint16_t)cc_dest, (uint16_t)cc_src, &res);
			break;
		}
		case MAKE_CC_OP(CC_OP_AND, 4):
		case MAKE_CC_OP(CC_OP_AND, 1):
		case MAKE_CC_OP(CC_OP_AND, 2):
		case MAKE_CC_OP(CC_OP_OR, 4):
		case MAKE_CC_OP(CC_OP_OR, 1):
		case MAKE_CC_OP(CC_OP_OR, 2):
		case MAKE_CC_OP(CC_OP_XOR, 4):
		case MAKE_CC_OP(CC_OP_XOR, 1):
		case MAKE_CC_OP(CC_OP_XOR, 2):
		{
			*dest = 0;
			break;
		}

		default: assert(0);
	}
}

void interpret_rtl_get_OF(rtlreg_t* dest)
{
	switch (cc_op)
	{
		case MAKE_CC_OP(CC_OP_SUB, 4):
		{
			int32_t res = cc_res;
			*dest = __builtin_sub_overflow((int32_t)cc_dest, (int32_t)cc_src, &res);
			break;
		}
		case MAKE_CC_OP(CC_OP_SUB, 1):
		{
			int8_t res = cc_res;
			*dest = __builtin_sub_overflow((int8_t)cc_dest, (int8_t)cc_src, &res);
			break;
		}
		case MAKE_CC_OP(CC_OP_SUB, 2):
		{
			int16_t res = cc_res;
			*dest = __builtin_sub_overflow((int16_t)cc_dest, (int16_t)cc_src, &res);
			break;
		}
		case MAKE_CC_OP(CC_OP_AND, 4):
		case MAKE_CC_OP(CC_OP_AND, 1):
		case MAKE_CC_OP(CC_OP_AND, 2):
		case MAKE_CC_OP(CC_OP_OR, 4):
		case MAKE_CC_OP(CC_OP_OR, 1):
		case MAKE_CC_OP(CC_OP_OR, 2):
		case MAKE_CC_OP(CC_OP_XOR, 4):
		case MAKE_CC_OP(CC_OP_XOR, 1):
		case MAKE_CC_OP(CC_OP_XOR, 2):
		{
			*dest = 0;
			break;
		}
		case MAKE_CC_OP(CC_OP_DEC, 4):
		case MAKE_CC_OP(CC_OP_DEC, 1):
		case MAKE_CC_OP(CC_OP_DEC, 2):
		{
			// TODO implement this correctly
			*dest = 0;
			break;
		}

		default: assert(0);
	}
}
