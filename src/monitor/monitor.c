#include "nemu.h"
#include "monitor/monitor.h"
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#define ENTRY_START 0x100000

void init_difftest();
void init_regex();
void init_wp_pool();
void init_device();

void reg_test();
void init_qemu_reg();
bool gdb_memcpy_to_qemu(uint32_t, void *, int);

FILE *log_fp = NULL;
static char *log_file = NULL;
static char *img_file = NULL;
static int is_batch_mode = false;
int rr_mode = RR_NONE;
FILE *rrlog_fp = NULL;
static char *rrlog_file = NULL;

static inline void init_log() {
#ifdef DEBUG
  if (log_file == NULL) return;
  log_fp = fopen(log_file, "w");
  Assert(log_fp, "Can not open '%s'", log_file);
#endif
}

static inline void init_rrlog() {
  if (rr_mode == RR_NONE) return;

  if (rrlog_file == NULL)
	  panic("Must specify a log file for record/replay (use --rrlog)");

  if (rr_mode == RR_RECORD)
	  rrlog_fp = fopen(rrlog_file, "wb");
  else
	  rrlog_fp = fopen(rrlog_file, "rb");

  Assert(rrlog_fp, "Can not open '%s'", rrlog_file);
}

static inline void welcome() {
  printf("Welcome to NEMU!\n");
  Log("Build time: %s, %s", __TIME__, __DATE__);

#ifdef DEBUG
  Log("Debug: \33[1;32m%s\33[0m", "ON");
  Log("If debug mode is on, A log file will be generated to record every instruction NEMU executes. "
      "This may lead to a large log file. "
      "If it is not necessary, you can turn it off in include/common.h.");
#else
  Log("Debug: \33[1;32m%s\33[0m", "OFF");
#endif

#ifdef DIFF_TEST
  Log("Differential testing: \33[1;32m%s\33[0m", "ON");
  Log("If differential testing mode is on, the result of every instruction will be compared with QEMU. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in include/common.h.");
#else
  Log("Differential testing: \33[1;32m%s\33[0m", "OFF");
#endif

  printf("For help, type \"help\"\n");
}

static inline int load_default_img() {
  const uint8_t img []  = {
    0xb8, 0x34, 0x12, 0x00, 0x00,        // 100000:  movl  $0x1234,%eax
    0xb9, 0x27, 0x00, 0x10, 0x00,        // 100005:  movl  $0x100027,%ecx
    0x89, 0x01,                          // 10000a:  movl  %eax,(%ecx)
    0x66, 0xc7, 0x41, 0x04, 0x01, 0x00,  // 10000c:  movw  $0x1,0x4(%ecx)
    0xbb, 0x02, 0x00, 0x00, 0x00,        // 100012:  movl  $0x2,%ebx
    0x66, 0xc7, 0x84, 0x99, 0x00, 0xe0,  // 100017:  movw  $0x1,-0x2000(%ecx,%ebx,4)
    0xff, 0xff, 0x01, 0x00,
    0xb8, 0x00, 0x00, 0x00, 0x00,        // 100021:  movl  $0x0,%eax
    0xd6,                                // 100026:  nemu_trap
  };

  Log("No image is given. Use the default build-in image.");

  memcpy(guest_to_host(ENTRY_START), img, sizeof(img));

  return sizeof(img);
}

static inline void load_img() {
  long size;
  if (img_file == NULL) {
    size = load_default_img();
  }
  else {
    int ret;

    FILE *fp = fopen(img_file, "rb");
    Assert(fp, "Can not open '%s'", img_file);

    Log("The image is %s", img_file);

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    ret = fread(guest_to_host(ENTRY_START), size, 1, fp);
    assert(ret == 1);

    fclose(fp);
  }

#ifdef DIFF_TEST
  gdb_memcpy_to_qemu(ENTRY_START, guest_to_host(ENTRY_START), size);
#endif
}

static inline void restart() {
  /* Set the initial instruction pointer. */
  cpu.eip = ENTRY_START;

#ifdef DIFF_TEST
  init_qemu_reg();
#endif

  /* Initialize EFLAGS register */
  cpu.eflags.val = 0x2;
}

static inline void parse_args(int argc, char *argv[]) {
  static struct option long_options[] = {
    {"record", no_argument, &rr_mode, RR_RECORD },
    {"replay", no_argument, &rr_mode, RR_REPLAY },
    {"rrlog", required_argument, 0, 0 },
	{ 0, 0, 0, 0}
  };
  int o;
  int option_index;

  while ( (o = getopt_long(argc, argv, "-bl:", long_options, &option_index)) != -1) {
    switch (o) {
      case 0:   
                if (option_index == 2) rrlog_file = optarg;
			   	break;
      case 'b': is_batch_mode = true; break;
      case 'l': log_file = optarg; break;
      case 1:
                if (img_file != NULL) Log("too much argument '%s', ignored", optarg);
                else img_file = optarg;
                break;
      default:
                panic("Usage: %s [-b] [-l log_file] [--record|--replay --rrlog rrlog_file] [img_file]", argv[0]);
    }
  }
}

static void sigint_handler(int signum) {
	nemu_state = NEMU_INTERRUPT;
}

static void set_sigint_handler() {
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_handler = sigint_handler;
  act.sa_flags = SA_RESTART;
  int ret = sigaction(SIGINT, &act, NULL);
  Assert(ret == 0, "Can not set signal handler");
}

int init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Open the log file. */
  init_log();

  /* Open the log file for record/replay. */
  init_rrlog();

  /* Test the implementation of the `CPU_state' structure. */
  reg_test();

#ifdef DIFF_TEST
  /* Fork a child process to perform differential testing. */
  init_difftest();
#endif

  /* Load the image to memory. */
  load_img();

  /* Initialize this virtual computer system. */
  restart();

  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();

  /* Initialize devices. */
  init_device();

  /* Install the handler of SIGINT */
  set_sigint_handler();

  /* Display welcome message. */
  welcome();

  return is_batch_mode;
}
