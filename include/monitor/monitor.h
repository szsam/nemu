#ifndef __MONITOR_H__
#define __MONITOR_H__

#include <signal.h>
#include <stdio.h>

enum { NEMU_STOP, NEMU_RUNNING, NEMU_END, NEMU_ABORT, NEMU_INTERRUPT };
extern volatile sig_atomic_t nemu_state;

enum { RR_NONE, RR_RECORD, RR_REPLAY };
extern int rr_mode;
extern FILE *rrlog_io_fp;
extern FILE *rrlog_intr_fp;
extern int next_intr_tb_num;

#endif
