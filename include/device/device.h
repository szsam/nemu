#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "common.h"

#ifdef HAS_IOE

void init_serial();
void init_timer();
void init_vga();
void init_i8042();

void timer_intr();
void send_key(uint8_t, bool);
void update_screen();

void device_update();

int save_key_queue(FILE *);
int load_key_queue(FILE *);

#endif

#endif
