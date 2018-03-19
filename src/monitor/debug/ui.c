#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static int cmd_bt(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Step one instruction exactly", cmd_si },
  { "info", "Generic command for showing things about the program being debugged", cmd_info },
  { "x", "Examine memory: x N EXPR", cmd_x },
  { "p", "Print value of expression EXPR", cmd_p },
  { "w", "Set an watchpoint for an expression: w EXPR", cmd_w },
  { "d", "Delete a watchpoint: d N", cmd_d },
  { "bt", "Print backtrace of all stack frames", cmd_bt },

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");

	if(arg == NULL) {
		/* no argument given */
		cpu_exec(1);
	}
	else {
		int count = atoi(arg);
		cpu_exec(count);
	}
	return 0;
}


static int cmd_info(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");

	if (!strcmp(arg, "r")) {
		/* list registers and their contents */
		for (int i = R_EAX; i <= R_EDI; i++)
			printf("%s\t\t0x%x\t\t%d\n", reg_name(i, 4), reg_l(i), reg_l(i));

		printf("eip\t\t0x%x\t\t%d\n", cpu.eip, cpu.eip);
	}
	else if (!strcmp(arg, "w")) {
		print_watchpoints();
	}
	else
		fprintf(stderr, "Unknown info command '%s'\n", arg);

	return 0;
}

static int cmd_x(char *args) {
	/* extract the first two arguments */
	char *arg1 = strtok(NULL, " ");
	char *arg2 = arg1 + strlen(arg1) + 1;
	int num = atoi(arg1);
	bool success;
	vaddr_t addr = expr(arg2, &success);

	if (!success) return 0;

	for (int i = 0; i < num; i++) {
		vaddr_t addr_temp = addr + 4 * i;
		uint32_t data = vaddr_read(addr_temp, 4);

		if (i % 4 == 0)
		{
			printf("0x%x:\t", addr_temp);
		}
		printf("0x%x\t", data);
		if ((i + 1) % 4 == 0)
			printf("\n");

	}
	printf("\n");
	return 0;
}

static int cmd_p(char *args) {
	char tok[8];
	sscanf(args, "%7s", tok);
	if (!strcmp(tok, "$eflags")) {
		char flags[32] = "[ ";
//		if (cpu.CF) strcat(flags, "CF ");
//		if (cpu.PF) strcat(flags, "PF ");
//		if (cpu.ZF) strcat(flags, "ZF ");
//		if (cpu.SF) strcat(flags, "SF ");
//		if (cpu.IF) strcat(flags, "IF ");
//		if (cpu.DF) strcat(flags, "DF ");
//		if (cpu.OF) strcat(flags, "OF ");
		strcat(flags, "]");
		printf("%s\n", flags);
	}
	else {
		bool success;
		uint32_t res = expr(args, &success);

		if (success)
			printf("%u\n", res);
	}
	return 0;
}

static int cmd_w(char *args) {
	if (strlen(args) > MAX_LENGTH_OF_EXPR) {
		fprintf(stderr, "expression too long\n");
		return 0;
	}
	bool success;
	uint32_t res = expr(args, &success);
	
	if (!success) return 0;

	WP *wp = new_wp();
	strcpy(wp->expr, args);
	wp->value = res;
	
	printf("Watchpoint %d: %s\n", wp->NO, wp->expr);
	return 0;
}

static int cmd_d(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");

	if(arg == NULL) {
		/* no argument given */
		fprintf(stderr, "Need a number. Try 'help d'\n");
	}
	else {
		int number = atoi(arg);
		free_wp(number);
	}
	return 0;
}

static int cmd_bt(char *args) {
//	int num = 0;
//	swaddr_t pc = cpu.eip, frame_ptr = cpu.ebp;
//
//	//if (cpu.eip == ENTRY_START) {
//	//	printf("No stack.\n");
//	//	return 0;
//	//}
//
//	printf("Warning: The number of parameters of a function may be less than four, "
//			"however, four arguments are FORCED to be printed for each function.\n");
//
//	while (frame_ptr) {
//		printf("#%d ", num);
//		if (num++ != 0) printf("0x%08x in ", pc);
//		printf("%s (", func_name(pc));
//		for (int offset = 8; offset <=16; offset += 4) {
//			printf("0x%x, ", swaddr_read(frame_ptr + offset, 4, R_SS) );
//		}
//		printf("0x%x)\n", swaddr_read(frame_ptr + 20, 4, R_SS) );
//
//		pc = swaddr_read(frame_ptr + 4, 4, R_SS);
//		frame_ptr = swaddr_read(frame_ptr, 4, R_SS);
//	}
	return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
