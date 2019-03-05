#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

int save_pmem(FILE *fp) {
	size_t ret = fwrite(pmem, sizeof(uint8_t), PMEM_SIZE, fp);
	return ret == PMEM_SIZE ? 0 : -1;
}

int load_pmem(FILE *fp) {
	size_t ret = fread(pmem, sizeof(uint8_t), PMEM_SIZE, fp);
	return ret == PMEM_SIZE ? 0 : -1;
}

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int no = is_mmio(addr);
  if (no == -1)
	return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  else
	return mmio_read(addr, len, no);
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int no = is_mmio(addr);
  if (no == -1)
	memcpy(guest_to_host(addr), &data, len);
  else
	mmio_write(addr, len, data, no);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  paddr_write(addr, len, data);
}
