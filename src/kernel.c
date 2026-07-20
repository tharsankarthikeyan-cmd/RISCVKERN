#include <stdint.h>
#include <stddef.h>
#include "plic_mmap.h"
#include "handler.h"
#include "debug.h"
#include "paging.h"
#include "common.h"
#include "pmm.h"
#include "create_proc.h"

extern void init_traps(void);

 
void kmain(void) {
  plic_mmap();
  paging_init_2();
  ecall_print((uint8_t*)"El Psy Kongroo\r\n", 14);
  flush_paging((uint64_t)pte_giga_entry);
  ecall_print((uint8_t*)"El Psy Kongroo\r\n", 14);
  pmm_init();
  init_traps(); 
  void* new_root_page = create_proc();
  flush_paging((uint64_t)new_root_page);
  ecall_print((uint8_t*)"El Psy Kongroo\r\n", 14);
  ((volatile uint8_t*)(0x10000000 + 0xFFFFFFC000000000))[1] = 0x01;
	while(1) {
		// Read input from the UART
    asm volatile("wfi");
	}
	return;
}
