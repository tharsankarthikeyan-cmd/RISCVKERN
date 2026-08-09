#include <stddef.h>
#include <stdint.h>
#include "debug.h"
#include "plic_mmap.h"
#include "handler.h"
#include "proc.h"
#include "create_proc.h"
#include "common.h"
#include "timer.h"

extern void enter_proc(void* root_page_tab, trapframe_t* tf);
extern uint8_t user_prog_end[];
extern uint8_t user_prog_end_2[];

uint8_t command_string[100];
uint8_t command_string_index = 0;

void handler_function(uint64_t ecall_id, uint64_t value, uint64_t trapframe_reg){
  uint64_t scause_reg;
  __asm__ __volatile__(
    "csrr %0, scause\n\t"
    :"=r"(scause_reg)
    :
    :
  );
  if(scause_reg == 8){
    
    // If the Exception is an ecall add 4 bytes to SEPC and then push tio 232(sp)
    __asm__ __volatile__(
      "csrr t0, sepc\n\t"
      "addi t0, t0, 4\n\t"
      "sd t0, 232(%0)\n\t"
      "csrsi sstatus, 2\n\t"
      :
      :"r"(trapframe_reg)
      :"t0","memory"
    );

    if(ecall_id == 0xdeadbeef){
      uint8_t process_string[17] = "\033[ ; HProcess  : ";
      ecall_print((uint8_t*)"\033[0;0HProcess 1: ", 17);
      uint8_t count[6] = {'0','0','0','0','0','\r'};
      uart_int(value,count,5);
      ecall_print((uint8_t*)count,5);
    }
    else if(ecall_id == 0xdeadc0de){
      uint8_t process_string[17] = "\033[ ; HProcess  : ";
      process_string[2] = '0' + 2*(current_proc->pid);
      process_string[4] = '0';
      process_string[14] = '0' + current_proc->pid;
      ecall_print((uint8_t*)process_string, 17);
      uint8_t count[6] = {'0','0','0','0','0','\r'};
      uart_int(value,count,5);
      ecall_print((uint8_t*)count,5);
    }
    else if(ecall_id == 0x2){
      uint8_t print_string[] = "\033[  ;  H";
      uint64_t x_coor;
      uint64_t y_coor;
      uint8_t* string_ptr;
      uint64_t length;
      __asm__ __volatile__(
        "ld %0, 64(%4)\n\t"
        "ld %1, 72(%4)\n\t"
        "ld %2, 80(%4)\n\t"
        "ld %3, 88(%4)\n\t"
        :"=r"(x_coor), "=r"(y_coor), "=r"(string_ptr), "=r"(length)
        :"r"(trapframe_reg)
        :
      );
      uart_int(x_coor,&print_string[2],2);
      uart_int(y_coor,&print_string[5],2);
      ecall_print((uint8_t*)print_string,8);
      for(uint64_t i = 0; i < length; i++){
        print_string[8+i] = string_ptr[i];
      }
      ecall_print((uint8_t*)print_string,8+length);
      uint8_t end_of_line = '\n';
      //ecall_print((uint8_t*)string_ptr,length);
      ecall_print(&end_of_line,1);
    }
    else if(ecall_id == 0x0){
      // Kill Process
      void* current_proc_tables = (void*)((uintptr_t)current_proc->root_page_table - (uintptr_t)0xFFFFFFC000000000ULL);
      delete_page_tables(current_proc_tables);
      Proc* traverse = current_proc;
      while((void*)traverse->next != (void*)current_proc){
        traverse = traverse->next;
      }
      if(current_proc == end_proc){
        end_proc = traverse;
      }
      traverse->next = current_proc->next;
      //current_proc = current_proc->next;
      current_proc = current_proc->next;
      void* root_page_tab = current_proc->root_page_table;
      trapframe_t* tf = current_proc->tf;
      enter_proc(root_page_tab, tf);
    }
  }
  else if((scause_reg & 0xF) == 5){
    ecall_timer_set();
    //ecall_print((uint8_t*)"El Psy Kongroo", 14);
    current_proc = current_proc->next;
    void* root_page_tab = current_proc->root_page_table;
    trapframe_t* tf = current_proc->tf;
    enter_proc(root_page_tab, tf);
  }
  else if(scause_reg == 13 || scause_reg == 12 || scause_reg == 15){
    // Make sure to save the SEPC
    uint64_t sstatus_reg;
    __asm__ __volatile__(
      "csrr %0, sstatus"
      :"=r"(sstatus_reg)
      :
      :
    );
    if(((sstatus_reg >> 7) & 0x1) == 0x1){
      __asm__ __volatile__(
        "csrr t0, sepc\n\t"
        "sd t0, 232(%0)\n\t"
        "csrsi sstatus, 2\n\t"
        :
        :"r"(trapframe_reg)
        :"t0","memory"
      );
    }
    void* stval_reg;
    __asm__ __volatile__(
      "csrr %0, stval\n\t"
      :"=r"(stval_reg)
      :
      :
    );
    uintptr_t root_page_table;
    __asm__ __volatile__(
      "csrr %0, satp\n\t"
      :"=r"(root_page_table)
      :
      :
    );
    root_page_table = ((root_page_table & 0xFFFFFFFFFFF) << 12) + (uintptr_t)0xFFFFFFC000000000ULL;  
    demand_paging((void*)root_page_table,stval_reg);
  }
  else {
    uint64_t sstatus_reg;
    __asm__ __volatile__(
      "csrr %0, sstatus"
      :"=r"(sstatus_reg)
      :
      :
    );

    if(((sstatus_reg >> 7) & 0x1) == 0x1){
      __asm__ __volatile__(
        "csrr t0, sepc\n\t"
        "sd t0, 232(%0)\n\t"
        "csrsi sstatus, 2\n\t"
        :
        :"r"(trapframe_reg)
        :"t0","memory"
      );
    }

    uint32_t* write_ptr = (uint32_t*)((uintptr_t)PLIC_CLAIM_COMPLETE + 0xFFFFFFC000000000);
    uint32_t claim = *write_ptr;
    // Make sure to save the SEPC
    if(claim == 10){
      uint8_t char_t = *(volatile uint8_t*)(0x10000000+0xFFFFFFC000000000);
      uint8_t char_addr[1] = {char_t};
      if(char_addr[0] == 'X' || char_addr[0] == 'x'){
        init_proc(user_prog_end,user_prog_end_2,false);
        ecall_print((uint8_t*)char_addr,1);
        command_string[command_string_index] = char_addr[0];
        command_string_index++;
      }
      else if(char_addr[0] == 'k'){
        ecall_print((uint8_t*)char_addr,1);
        command_string[command_string_index] = char_addr[0];
        command_string_index++;
        void* current_proc_tables = (void*)((uintptr_t)current_proc->root_page_table - (uintptr_t)0xFFFFFFC000000000ULL);
        delete_page_tables(current_proc_tables);
        Proc* traverse = current_proc;
        while((void*)traverse->next != (void*)current_proc){
          traverse = traverse->next;
        }
        if(current_proc == end_proc){
          end_proc = traverse;
        }
        traverse->next = current_proc->next;
        //current_proc = current_proc->next;
        current_proc = current_proc->next;
        void* root_page_tab = current_proc->root_page_table;
        trapframe_t* tf = current_proc->tf;
        *write_ptr = claim;
        enter_proc(root_page_tab, tf);
        //enter_proc(current_proc->root_page_table, current_proc->tf);
      }
      else if(char_addr[0] == '\r'){
        ecall_print((uint8_t*)"\n",1);
        ecall_print((uint8_t*)command_string,command_string_index);
        ecall_print((uint8_t*)"\n",1);
        ecall_print((uint8_t*)"> ",2);
        command_string_index = 0;
      }
      else{
        ecall_print((uint8_t*)char_addr,1);
        command_string[command_string_index] = char_addr[0];
        command_string_index++;
      }
      //*(volatile uint8_t*)(0x10000000) = char_t;
    }
    *write_ptr = claim;
  }
}
