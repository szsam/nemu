#ifndef __MONITOR_H__
#define __MONITOR_H__

#include <signal.h>

enum { NEMU_STOP, NEMU_RUNNING, NEMU_END, NEMU_ABORT, NEMU_INTERRUPT };
extern volatile sig_atomic_t nemu_state;

enum { RR_NONE, RR_RECORD, RR_REPLAY};
extern int rr_mode;

#endif
