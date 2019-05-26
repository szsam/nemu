#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  cpu.esp -= 12;
  vaddr_write(cpu.esp + 8, 4, cpu.eflags.val);
  vaddr_write(cpu.esp + 4, 4, cpu.cs);
  vaddr_write(cpu.esp + 0, 4, ret_addr);

  cpu.eflags.IF = 0;

  vaddr_t desc_addr = cpu.idtr.base + (NO << 3);
  uint32_t tmp0 = vaddr_read(desc_addr, 4);
  uint32_t tmp1 = vaddr_read(desc_addr + 4, 4);
  vaddr_t handler_addr = (tmp0 & 0xffff) | (tmp1 & 0xffff0000);
  cpu.eip = handler_addr;
}

void dev_raise_intr() {
  cpu.INTR = true;
}
