#ifndef __RTL_WRAPPER_H__
#define __RTL_WRAPPER_H__

#include "macro.h"

#define RTL_PREFIX generate

#define rtl_j         concat3(RTL_PREFIX, _, rtl_j       )
#define rtl_jr        concat3(RTL_PREFIX, _, rtl_jr      )
#define rtl_jrelop    concat3(RTL_PREFIX, _, rtl_jrelop  )
#define rtl_setrelop  concat3(RTL_PREFIX, _, rtl_setrelop)
#define rtl_exit      concat3(RTL_PREFIX, _, rtl_exit    )
#define rtl_li        concat3(RTL_PREFIX, _, rtl_li      )
#define rtl_add       concat3(RTL_PREFIX, _, rtl_add     )
#define rtl_sub       concat3(RTL_PREFIX, _, rtl_sub     )
#define rtl_and       concat3(RTL_PREFIX, _, rtl_and     )
#define rtl_or        concat3(RTL_PREFIX, _, rtl_or      )
#define rtl_xor       concat3(RTL_PREFIX, _, rtl_xor     )
#define rtl_shl       concat3(RTL_PREFIX, _, rtl_shl     )
#define rtl_shr       concat3(RTL_PREFIX, _, rtl_shr     )
#define rtl_sar       concat3(RTL_PREFIX, _, rtl_sar     )
#define rtl_mul_lo    concat3(RTL_PREFIX, _, rtl_mul_lo  )
#define rtl_mul_hi    concat3(RTL_PREFIX, _, rtl_mul_hi  )
#define rtl_imul_lo   concat3(RTL_PREFIX, _, rtl_imul_lo )
#define rtl_imul_hi   concat3(RTL_PREFIX, _, rtl_imul_hi )
#define rtl_div_q     concat3(RTL_PREFIX, _, rtl_div_q   )
#define rtl_div_r     concat3(RTL_PREFIX, _, rtl_div_r   )
#define rtl_idiv_q    concat3(RTL_PREFIX, _, rtl_idiv_q  )
#define rtl_idiv_r    concat3(RTL_PREFIX, _, rtl_idiv_r  )
#define rtl_addi      concat3(RTL_PREFIX, _, rtl_addi    )
#define rtl_subi      concat3(RTL_PREFIX, _, rtl_subi    )
#define rtl_andi      concat3(RTL_PREFIX, _, rtl_andi    )
#define rtl_ori       concat3(RTL_PREFIX, _, rtl_ori     )
#define rtl_xori      concat3(RTL_PREFIX, _, rtl_xori    )
#define rtl_shli      concat3(RTL_PREFIX, _, rtl_shli    )
#define rtl_shri      concat3(RTL_PREFIX, _, rtl_shri    )
#define rtl_sari      concat3(RTL_PREFIX, _, rtl_sari    )
#define rtl_mul_loi   concat3(RTL_PREFIX, _, rtl_mul_loi )
#define rtl_mul_hii   concat3(RTL_PREFIX, _, rtl_mul_hii )
#define rtl_imul_loi  concat3(RTL_PREFIX, _, rtl_imul_loi)
#define rtl_imul_hii  concat3(RTL_PREFIX, _, rtl_imul_hii)
#define rtl_div_qi    concat3(RTL_PREFIX, _, rtl_div_qi  )
#define rtl_div_ri    concat3(RTL_PREFIX, _, rtl_div_ri  )
#define rtl_idiv_qi   concat3(RTL_PREFIX, _, rtl_idiv_qi )
#define rtl_idiv_ri   concat3(RTL_PREFIX, _, rtl_idiv_ri )
#define rtl_div64_q   concat3(RTL_PREFIX, _, rtl_div64_q )
#define rtl_div64_r   concat3(RTL_PREFIX, _, rtl_div64_r )
#define rtl_idiv64_q  concat3(RTL_PREFIX, _, rtl_idiv64_q)
#define rtl_idiv64_r  concat3(RTL_PREFIX, _, rtl_idiv64_r)
#define rtl_lm        concat3(RTL_PREFIX, _, rtl_lm      )
#define rtl_sm        concat3(RTL_PREFIX, _, rtl_sm      )
#define rtl_host_lm   concat3(RTL_PREFIX, _, rtl_host_lm )
#define rtl_host_sm   concat3(RTL_PREFIX, _, rtl_host_sm )
#define rtl_lr_l      concat3(RTL_PREFIX, _, rtl_lr_l    )
#define rtl_lr_w      concat3(RTL_PREFIX, _, rtl_lr_w    )
#define rtl_lr_b      concat3(RTL_PREFIX, _, rtl_lr_b    )
#define rtl_sr_l      concat3(RTL_PREFIX, _, rtl_sr_l    )
#define rtl_sr_w      concat3(RTL_PREFIX, _, rtl_sr_w    )
#define rtl_sr_b      concat3(RTL_PREFIX, _, rtl_sr_b    )
#define rtl_hostcall  concat3(RTL_PREFIX, _, rtl_hostcall)
#define rtl_set_CF    concat3(RTL_PREFIX, _, rtl_set_CF  )
#define rtl_set_OF    concat3(RTL_PREFIX, _, rtl_set_OF  )
#define rtl_set_ZF    concat3(RTL_PREFIX, _, rtl_set_ZF  )
#define rtl_set_SF    concat3(RTL_PREFIX, _, rtl_set_SF  )
#define rtl_get_CF    concat3(RTL_PREFIX, _, rtl_get_CF  )
#define rtl_get_OF    concat3(RTL_PREFIX, _, rtl_get_OF  )
#define rtl_get_ZF    concat3(RTL_PREFIX, _, rtl_get_ZF  )
#define rtl_get_SF    concat3(RTL_PREFIX, _, rtl_get_SF  )
#define rtl_pio_read  concat3(RTL_PREFIX, _, rtl_pio_read)
#define rtl_pio_write concat3(RTL_PREFIX, _, rtl_pio_write)
#define rtl_cc_set_op concat3(RTL_PREFIX, _, rtl_cc_set_op)

#endif
