#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#define MAX_LENGTH_OF_EXPR 127

typedef struct watchpoint {
	int NO;
	char expr[MAX_LENGTH_OF_EXPR+1];
	uint32_t value;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */


} WP;

WP *new_wp();
void free_wp(int);
void print_watchpoints();
void check_watchpoints();

#endif
