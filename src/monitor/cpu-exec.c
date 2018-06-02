#include <inttypes.h>

#include "nemu.h"
#include "monitor/monitor.h"
#include "monitor/watchpoint.h"
#include "cpu/tb.h"

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

int nemu_state = NEMU_STOP;

bool exec_wrapper(bool);

static uint64_t g_nr_guest_instr = 0;

void nr_guest_instr_add(uint32_t n) {
  g_nr_guest_instr += n;
}

static TranslationBlock *tblocks = NULL;
TranslationBlock *cur_tblock = NULL;

void monitor_statistic() {
  Log("total guest instructions = %" PRIu64, g_nr_guest_instr);
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  if (nemu_state == NEMU_END || nemu_state == NEMU_ABORT) {
    printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
    return;
  }
  nemu_state = NEMU_RUNNING;

  bool print_flag = n < MAX_INSTR_TO_PRINT;

  for (; n > 0; n --) {
    /* Execute one instruction, including instruction fetch,
     * instruction decode, and the actual execution. */
//    exec_wrapper(print_flag);
//    nr_guest_instr_add(1);

#if defined(DIFF_TEST)
	vaddr_t ori_eip = cpu.eip;
#endif

	// search the translation block beginning at eip
	HASH_FIND_INT(tblocks, &cpu.eip, cur_tblock);

	if (cur_tblock == NULL) {
		// translate a block and save RTLs
		cur_tblock = (TranslationBlock *)malloc(sizeof(TranslationBlock));
		cur_tblock->eip_start = cpu.eip;
		cur_tblock->guest_instr_cnt = 0;
		INIT_LIST_HEAD(&cur_tblock->rtl_instr_list.list);
		HASH_ADD_INT(tblocks, eip_start, cur_tblock);

		// translate up to (and include) next control transfer instr
		do {
			++cur_tblock->guest_instr_cnt;
		}while(!exec_wrapper(print_flag));

		cur_tblock->eip_end = cpu.eip;
	}

	cpu.eip = cur_tblock->eip_end;
	interpret_tblock(cur_tblock);
    nr_guest_instr_add(cur_tblock->guest_instr_cnt);

#ifdef DIFF_TEST
  void difftest_step(uint32_t, int);
  difftest_step(ori_eip, cur_tblock->guest_instr_cnt);
#endif

#ifdef DEBUG
    /* TODO: check watchpoints here. */
	check_watchpoints();	

#endif

#ifdef HAS_IOE
    extern void device_update();
    device_update();
#endif

    if (nemu_state != NEMU_RUNNING) {
      if (nemu_state == NEMU_END) {
        printflog("\33[1;31mnemu: HIT %s TRAP\33[0m at eip = 0x%08x\n\n",
            (cpu.eax == 0 ? "GOOD" : "BAD"), cpu.eip - 1);
        monitor_statistic();
      }
      else if (nemu_state == NEMU_ABORT) {
        printflog("\33[1;31mnemu: ABORT\33[0m at eip = 0x%08x\n\n", cpu.eip);
      }
	  return;
    }
  }

  if (nemu_state == NEMU_RUNNING) { nemu_state = NEMU_STOP; }
}
