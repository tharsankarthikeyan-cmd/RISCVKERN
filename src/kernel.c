#include <stdint.h>
#include <stddef.h>
#include "plic_mmap.h"
#include "handler.h"
#include "debug.h"
#include "paging.h"
#include "common.h"
#include "pmm.h"
#include "create_proc.h"
#include "proc.h"
#include "mem_cpy.h"
#include "timer.h"

extern void init_traps(void);
extern void enter_proc(void* root_page_tab, trapframe_t* tf);
extern uint8_t user_prog_start[];
extern uint8_t user_prog_end[];
extern uint8_t user_prog_end_2[];

Proc* proc_list;
Proc* end_proc;
Proc* current_proc;

void kmain(void) {
  plic_mmap();
  paging_init_2();
  ecall_print((uint8_t*)"\033[2J", 4);
  flush_paging((uint64_t)pte_giga_entry);
  ecall_print((uint8_t*)"\033[?25l",6);
  pmm_init();
  //ecall_timer_set();
  //init_traps();
  //void* new_root_page = create_proc();
  //flush_paging((uint64_t)new_root_page);
  
  // Create a New Page for Proc Struct Definitions
  proc_list = (Proc*)((uintptr_t)page_alloc(4096) + (uintptr_t)0xFFFFFFC000000000);
  end_proc = (Proc*)proc_list;
  current_proc = (Proc*)proc_list;
  end_proc->next = (Proc*)proc_list;

  // Enable UART
  ((volatile uint8_t*)(0x10000000 + 0xFFFFFFC000000000))[1] = 0x01;

  // Try to init
  init_proc((void*)user_prog_start,(void*)user_prog_end,true);
  init_proc((void*)user_prog_end,(void*)user_prog_end_2,false);
  //delete_page_tables((void*)((uintptr_t)end_proc->root_page_table - (uintptr_t)0xFFFFFFC000000000ULL));
  init_traps();
  //enter_proc(init1.root_page_table, init1.tf);
  //ecall_print((uint8_t*)"El Psy Kongroo\r\n", 14);
	while(1) {
		// Read input from the UART
    asm volatile("wfi");
	}
	return;
}
