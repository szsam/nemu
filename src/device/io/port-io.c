#include "common.h"
#include "device/port-io.h"
#include "monitor/monitor.h"

#define PORT_IO_SPACE_MAX 65536
#define NR_MAP 8

/* "+ 3" is for hacking, see pio_read() below */
static uint8_t pio_space[PORT_IO_SPACE_MAX + 3];

typedef struct {
  ioaddr_t low;
  ioaddr_t high;
  pio_callback_t callback;
} PIO_t;

static PIO_t maps[NR_MAP];
static int nr_map = 0;

typedef struct {
	ioaddr_t addr;
	int len;
	uint32_t data;
} PIO_rec;

static void pio_callback(ioaddr_t addr, int len, bool is_write) {
  int i;
  for (i = 0; i < nr_map; i ++) {
    if (addr >= maps[i].low && addr + len - 1 <= maps[i].high) {
      if (maps[i].callback != NULL) {
        maps[i].callback(addr, len, is_write);
      }
      return;
    }
  }
}

/* device interface */
void* add_pio_map(ioaddr_t addr, int len, pio_callback_t callback) {
  assert(nr_map < NR_MAP);
  assert(addr + len <= PORT_IO_SPACE_MAX);
  maps[nr_map].low = addr;
  maps[nr_map].high = addr + len - 1;
  maps[nr_map].callback = callback;
  nr_map ++;
  return pio_space + addr;
}

/* CPU interface */
uint32_t pio_read(ioaddr_t addr, int len) {
	assert(addr + len - 1 < PORT_IO_SPACE_MAX);

	if (rr_mode == RR_REPLAY) {
		PIO_rec rec;
		Assert(fread(&rec, sizeof(rec), 1, rrlog_io_fp) == 1, 
				"fail to read rrlog");
		Assert(rec.addr == addr && rec.len == len, "replay error");
		return rec.data;
	}
	else {
		uint32_t data = 0;

		pio_callback(addr, len, false);		// prepare data to read
		switch (len) {
			case 4: memcpy(&data, pio_space + addr, 4); break;
			case 2: memcpy(&data, pio_space + addr, 2); break;
			case 1: memcpy(&data, pio_space + addr, 1); break;
			default: assert(0);
		}

		if (rr_mode == RR_RECORD) {
			PIO_rec rec = { addr, len, data };
			Assert(fwrite(&rec, sizeof(rec), 1, rrlog_io_fp) == 1, 
					"fail to log port read");
		}
		return data;
	}
}

void pio_write(ioaddr_t addr, uint32_t data, int len) {
  assert(addr + len - 1 < PORT_IO_SPACE_MAX);
  switch (len) {
    case 4: memcpy(pio_space + addr, &data, 4); break;
    case 2: memcpy(pio_space + addr, &data, 2); break;
    case 1: memcpy(pio_space + addr, &data, 1); break;
    default: assert(0);
  }
  pio_callback(addr, len, true);
}

// /* CPU interface */
// uint32_t pio_read_l(ioaddr_t addr) {
//   return pio_read_common(addr, 4);
// }
// 
// uint32_t pio_read_w(ioaddr_t addr) {
//   return pio_read_common(addr, 2);
// }
// 
// uint32_t pio_read_b(ioaddr_t addr) {
//   return pio_read_common(addr, 1);
// }
// 
// void pio_write_l(ioaddr_t addr, uint32_t data) {
//   pio_write_common(addr, data, 4);
// }
// 
// void pio_write_w(ioaddr_t addr, uint32_t data) {
//   pio_write_common(addr, data, 2);
// }
// 
// void pio_write_b(ioaddr_t addr, uint32_t data) {
//   pio_write_common(addr, data, 1);
// }
