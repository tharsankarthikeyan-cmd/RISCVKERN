#include <stddef.h>
#include <stdint.h>
#include "proc.h"
#include "pmm.h"
#include "paging.h"
#include "create_proc.h"
#include "mem_cpy.h"
#include "common.h"
#include "elf_parse.h"

extern void enter_proc(void* root_page_tab, trapframe_t* tf, void* prog_address_start, void* prog_address_end);

//Proc init1;
void init_proc(void* prog_address_start, void* prog_address_end, bool is_init){
  // STEP 1: Create a new page for the root page table and copy the the kernel page table;
  void* init_root_page_table = copy_page_data((void*)pte_giga_entry);

  // STEP 2: Make Special User Process Entries to this table
  // Create a new page for the user process.
  //void* user_prog_mega = page_alloc(4096); // This is for 2MB entry
  //void* user_prog_kilo = page_alloc(4096); // This is for 4KB entry
  //void* store_prog_1 = page_alloc(8192);
  //((uint64_t*)init_root_page_table)[0] = ((uintptr_t)user_prog_mega >> 2) | 0x01;
  //((uint64_t*)((uintptr_t)user_prog_mega + (uintptr_t)0xFFFFFFC000000000ULL))[0] = ((uintptr_t)user_prog_kilo >> 2) | 0x01;
  //((uint64_t*)((uintptr_t)user_prog_kilo + (uintptr_t)0xFFFFFFC000000000ULL))[0] = ((uintptr_t)store_prog_1 >> 2) | 0x1F;
  //store_prog_1 += 4096;
  //((uint64_t*)((uintptr_t)user_prog_kilo + (uintptr_t)0xFFFFFFC000000000ULL))[1] = ((uintptr_t)store_prog_1 >> 2) | 0x1F;

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
  init_trap_frame->s_stat = 0x40020;

  // Enter the entries into the Proc Struct
  //init1.pid = 0;
  //init1.root_page_table = init_root_page_table;
  //init1.tf = init_trap_frame;
  Proc* process;
  if(is_init){
    process = (Proc*)(end_proc);
  }
  else{
    process = (Proc*)((uintptr_t)(end_proc) + sizeof(Proc));
  }
  if(is_init){
    process->pid = 1;
  }
  else{
    process->pid = end_proc->pid + 1;
  }
  process->root_page_table = init_root_page_table;
  process->tf = init_trap_frame;
  process->next = (Proc*)(end_proc->next);
  if((process->pid % 2) == 0){
    process->proc_state = false;
  }
  else{
    process->proc_state = true;
  }
  end_proc->next = (Proc*)process;
  end_proc = (Proc*)process;

  // Save the current processes tables
  void* prev_page_tab = current_proc->root_page_table;
  
  // Flush the New Processes Table.
  flush_paging((uint64_t)process->root_page_table);
  
  // Allow User Access
  //   li t1, 0x40022
  //   csrw sstatus, t1
  __asm__ __volatile__(
    "li t1, 0x40022\n\t"
    "csrw sstatus, t1\n\t"
    :
    :
    :"t1"
  );


  // We load the Program and Keep
  Elf64_Ehdr* elf_file = (Elf64_Ehdr*)prog_address_start;
  Elf64_Phdr* elf_phdr = (Elf64_Phdr*)((uintptr_t)elf_file + (uintptr_t)elf_file->e_phoff);
  
  for(uint8_t i = 0; i < elf_file->e_phnum; i++){
    if(elf_phdr[i].p_type == 0x1){
      void* current_addr = (void*)((uintptr_t)elf_file + (uintptr_t)elf_phdr[i].p_offset);
      mem_cpy((void*)elf_phdr[i].p_vaddr,current_addr,(size_t)elf_phdr[i].p_filesz);
    }
  }

  if(is_init){
    flush_paging((uint64_t)pte_giga_entry);
  }
  else{
  // Reflush the current proc tables
  flush_paging((uint64_t)current_proc->root_page_table);
  }
  // Loading and Entering the Process.
  //enter_proc(process->root_page_table, process->tf, prog_address_start, prog_address_end);
}
