#include <stddef.h>
#include <stdint.h>
#include "debug.h"
#include "plic_mmap.h"
#include "handler.h"
#include "proc.h"
#include "common.h"
#include "timer.h"

extern void enter_proc(void* root_page_tab, trapframe_t* tf);

void handler_function(uint64_t scause_reg,uint64_t prog_id){
  if(scause_reg == 8){
    if(prog_id == 0xdeadbeef){
      ecall_print((uint8_t*)"Celeb17", 7);
    }
    else if(prog_id == 0xdeadc0de){
      ecall_print((uint8_t*)"El Psy Kongroo", 14);
    }
  }
  if((scause_reg & 0xF) == 5){
    ecall_timer_set();
    //ecall_print((uint8_t*)"El Psy Kongroo", 14);
    current_proc = current_proc->next;
    void* root_page_tab = current_proc->root_page_table;
    trapframe_t* tf = current_proc->tf;
    enter_proc(root_page_tab, tf);
  }
  else {
    uint32_t* write_ptr = (uint32_t*)((uintptr_t)PLIC_CLAIM_COMPLETE + 0xFFFFFFC000000000);
    uint32_t claim = *write_ptr;
    if(claim == 10){
      uint8_t char_t = *(volatile uint8_t*)(0x10000000+0xFFFFFFC000000000);
      uint8_t char_addr[1] = {char_t};
      ecall_print((uint8_t*)char_addr,1);
      //*(volatile uint8_t*)(0x10000000) = char_t;
    }
    *write_ptr = claim;
  }
}
