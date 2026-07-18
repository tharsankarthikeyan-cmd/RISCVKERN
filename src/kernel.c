#include <stdint.h>
#include <stddef.h>
#include "plic_mmap.h"
#include "handler.h"
#include "debug.h"
#include "paging.h"
#include "common.h"
#include "pmm.h"

extern void init_traps(void);

 
void kmain(void) {
  plic_mmap();
  paging_init_2();
  ecall_print((uint8_t*)"El Psy Kongroo\r\n", 14);
  flush_paging();
  ecall_print((uint8_t*)"El Psy Kongroo\r\n", 14);
  ecall_print((uint8_t*)"El Psy Kongroo\r\n", 14);
  init_traps(); 
  ecall_print((uint8_t*)"El Psy Kongroo\r\n", 14);
  pmm_init();
  void* code_is_dead_mismatch = page_alloc(12288);
  code_is_dead_mismatch = page_alloc(15000);
  free_pages(code_is_dead_mismatch);
  ((volatile uint8_t*)(0x10000000 + 0xFFFFFFC000000000))[1] = 0x01;
	while(1) {
		// Read input from the UART
    asm volatile("wfi");
	}
	return;
}
