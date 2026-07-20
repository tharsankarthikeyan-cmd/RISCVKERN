#include <stddef.h>
#include <stdint.h>
#include "proc.h"
#include "pmm.h"
#include "paging.h"
#include "create_proc.h"

Proc init1;
void init_proc(void){
  // STEP 1: Create a new page for the root page table and copy the the kernel page table;
  void* init_root_page_table = copy_page_data((void*)pte_giga_entry);

  // STEP 2: Make Special User Process Entries to this table
  // Create a new page for the user process.
  void* user_prog = page_alloc(4096);
  ((uint64_t*)init_root_page_table)[0] = ((uintptr_t)user_prog << 2) | 0x11;

  // STEP 3: Create a new trapframe and Kernel Stack
  void* k_stack = page_alloc(4096);
  k_stack += 4096;
  k_stack -= sizeof(trapframe_t);
  trapframe_t* init_trap_frame = (trapframe_t*)((uintptr_t)k_stack + (uintptr_t)0xFFFFFFC000000000ULL);

  init_trap_frame->stack_pointer = 0x5000;
  init_trap_frame->global_pointer = 0;
  init_trap_frame->thread_pointer = 0;
  for(uint8_t i = 0; i < 7; i++){
    init_trap_frame->temp_reg[i] = 0;
  }
  for(uint8_t i = 0; i < 12; i++){
    init_trap_frame->saved_reg[i] = 0;
  }
  for(uint8_t i = 0; i < 8; i++){
    init_trap_frame->func_args[i] = 0;
  }
  init_trap_frame->except_pc = 0x1000;
  init_trap_frame->ret_addr = 0;
  init_trap_frame->s_stat = 0x20;

  // Enter the entries into the Proc Struct
  init1.pid = 0;
  init1.root_page_table = init_root_page_table;
  init1.tf = init_trap_frame;
}
