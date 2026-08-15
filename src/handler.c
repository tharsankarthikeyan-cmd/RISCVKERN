#include <stddef.h>
#include <stdint.h>
#include "debug.h"
#include "plic_mmap.h"
#include "handler.h"
#include "proc.h"
#include "create_proc.h"
#include "common.h"
#include "timer.h"
#include "str_cmp.h"
#include "mem_cpy.h"

extern void enter_proc(void* root_page_tab, trapframe_t* tf);
extern uint8_t user_prog_end[];
extern uint8_t user_prog_end_2[];

uint8_t current_char;
uint8_t text_buffer[100];
uint64_t text_buf_index = 0;

// Head Handler Function for All Types of Interrupts
void handler_function(uint64_t ecall_id, uint64_t value, uint64_t trapframe_reg){
  // Get the scause Register using Inline Assembly to get the type of interrupts
  uint64_t scause_reg;
  __asm__ __volatile__(
    "csrr %0, scause\n\t"
    :"=r"(scause_reg)
    :
    :
  );

  // If the scause = 0x8 then the type if a synchronous software interrupts generally ecalls
  if(scause_reg == 8){ 
    // If the Exception is an ecall add 4 bytes to SEPC and then push to 232(sp)
    // SP is now actually moved due to function calls and variable declaration so we must somehow save the trapframe_reg
    // Hence the trapframe_reg is used to load the SEPC's updated value
    __asm__ __volatile__(
      "csrr t0, sepc\n\t"
      "addi t0, t0, 4\n\t"
      "sd t0, 232(%0)\n\t"
      "csrsi sstatus, 2\n\t"
      :
      :"r"(trapframe_reg)
      :"t0","memory"
    );

    // This is Just a test for 0xdeadbeef
    if(ecall_id == 0xdeadbeef){
      uint8_t process_string[17] = "\033[ ; HProcess  : ";
      ecall_print((uint8_t*)"\033[0;0HProcess 1: ", 17);
      uint8_t count[6] = {'0','0','0','0','0','\r'};
      uart_int(value,count,5);
      ecall_print((uint8_t*)count,5);
    }

    // Again This is just a test
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

    // ECALL INTERFACE ECALL_ID = 0x2: sys_print(x,y,string_ptr,length)
    else if(ecall_id == 0x2){
      // This is the string which handles the position of the print string in the screen
      uint8_t print_string[] = "\033[  ;  H";
      uint64_t x_coor;
      uint64_t y_coor;
      uint8_t* string_ptr;
      uint64_t length;

      // During ecall the user_prog performs ecall after the required assignment of argument registers which is then pushed to the trapframe_reg
      // We now use the trapframe_reg in order to load values into the required variable
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

      // Printing the Formatted Print String
      ecall_print((uint8_t*)print_string,8+length);

      // This section is specfically for end of line character
      uint8_t end_of_line = '\n';
      ecall_print(&end_of_line,1);
    }

    // ECALL INTERFACE ECALL_ID = 0x0: sys_exit();
    else if(ecall_id == 0x0){

      // Kill Process

      // Take the current proc root page table
      void* current_proc_tables = (void*)((uintptr_t)current_proc->root_page_table - (uintptr_t)0xFFFFFFC000000000ULL);

      // Recursively use the delete_page_tables function to delete the required pages
      delete_page_tables(current_proc_tables);

      // Make sure to eliminate that process from the Process Control Block
      Proc* traverse = current_proc;

      // This loop finds the process previous to the current process
      while((void*)traverse->next != (void*)current_proc){
        traverse = traverse->next;
      }

      // This is a special case for the last process
      if(current_proc == end_proc){
        end_proc = traverse;
      }

      // The Previous Process must be mapped to the current deleted processes next value
      traverse->next = current_proc->next;

      // The Current Proc Pointer must be moved to the current deleted processes next process
      current_proc = current_proc->next;

      // Take the new current_proc root page table and trapframe
      void* root_page_tab = current_proc->root_page_table;
      trapframe_t* tf = current_proc->tf;

      // Enter the current proc using the root_page_table and trapframe
      enter_proc(root_page_tab, tf);
    }

    // ECALL INTERFACE: ECALL_ID = 0x3: Right now ignore this
    else if(ecall_id == 0x3){
      current_proc->proc_state = false;
      current_proc = current_proc->next;
      while(current_proc->proc_state != 0x1){
        current_proc = current_proc->next;
      }
      // Gather the root_page_table and trapframe
      void* root_page_tab = current_proc->root_page_table;
      trapframe_t* tf = current_proc->tf;

      // Enter the process if and only if the current_proc states that the state is ACTIVE
      enter_proc(root_page_tab, tf);
    }
  }

  // When a Timer Interrupts Hits
  else if((scause_reg & 0xF) == 5){
    // Set the ecall timer one second in the future
    ecall_timer_set();

    // Enable Other Interrupts
    __asm__ __volatile__(
      "csrsi sstatus, 2\n\t"
      :
      :
      :
    );

    // Move the current_proc pointer to the next process of the current_proc
    current_proc = current_proc->next;
    while(current_proc->proc_state != 0x1){
      current_proc = current_proc->next;
    }

    // Gather the root_page_table and trapframe
    void* root_page_tab = current_proc->root_page_table;
    trapframe_t* tf = current_proc->tf;

    // Enter the process if and only if the current_proc states that the state is ACTIVE
    enter_proc(root_page_tab, tf);
  }

  // When Page Fault/Load Store Fault hits
  else if(scause_reg == 13 || scause_reg == 12 || scause_reg == 15){
    // Make sure to save the SEPC without the addition of 4
    // So that we can return and execute the same instruction once more
    // Get the sstatus value
    uint64_t sstatus_reg;
    __asm__ __volatile__(
      "csrr %0, sstatus"
      :"=r"(sstatus_reg)
      :
      :
    );

    // If the interrupt is from U-Mode
    // We still haven't save the SEPC to the trapframe_reg
    // Hence before turning on interrupts and then losing the SEPC
    // We must save the SEPC and then turn on the interrupt
    // But this is only if the interrupt is from U-Mode
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

    // Gather the value of stval and root_page_table
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

    // Call Demand Paging Function
    demand_paging((void*)root_page_table,stval_reg);
  }

  // This is for keyboard section
  else {
    // Get Sstatus register
    uint64_t sstatus_reg;
    __asm__ __volatile__(
      "csrr %0, sstatus"
      :"=r"(sstatus_reg)
      :
      :
    );

    // If the interrupt is from U-Mode
    // We still haven't save the SEPC to the trapframe_reg
    // Hence before turning on interrupts and then losing the SEPC
    // We must save the SEPC and then turn on the interrupt
    // But this is only if the interrupt is from U-Mode
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
    
    // Getting the Correct Value for UART Buffer Access
    uint32_t* write_ptr = (uint32_t*)((uintptr_t)PLIC_CLAIM_COMPLETE + 0xFFFFFFC000000000);

    // Get the value via pointer dereference
    uint32_t claim = *write_ptr;

    // If the UART Claim then
    if(claim == 10){
      // Get the Character
      uint8_t char_t = *(volatile uint8_t*)(0x10000000+0xFFFFFFC000000000);
      uint8_t char_addr[1] = {char_t};

      // This is just testing phase for new process execution if 'x' is pressed
      if(char_addr[0] == 'X' || char_addr[0] == 'x'){
        init_proc(user_prog_end,user_prog_end_2,false);
        ecall_print((uint8_t*)char_addr,1);
        current_char = char_addr[0];
      }

      // Process Killing if K is pressed
      else if(char_addr[0] == 'k'){
        ecall_print((uint8_t*)char_addr,1);
        current_char = char_addr[0];
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
        current_proc = current_proc->next;
        while(current_proc->proc_state != 0x1){
          current_proc = current_proc->next;
        }
        void* root_page_tab = current_proc->root_page_table;
        trapframe_t* tf = current_proc->tf;
        *write_ptr = claim;
        enter_proc(root_page_tab, tf);
      }

      // This is most important part
      // On Enter key, we must be able to loop through all the waiting processes and then finally return the memcpy the string into the waiting processes buffer
      else if(char_addr[0] == '\r'){
        ecall_print((uint8_t*)"\n",1);
        ecall_print((uint8_t*)"> ",2);
        ecall_print((uint8_t*)text_buffer,text_buf_index);
        current_char = '\r';
        // Add End of string character
        text_buffer[text_buf_index] = '\0';
        Proc* traverse = current_proc;
        while(traverse->next != current_proc || traverse->next == current_proc){
          if(traverse->proc_state == 0x0){
            flush_paging((uint64_t)traverse->root_page_table);
            // Gather the destination from the trapframe via a1 register
            trapframe_t* traverse_trapframe = traverse->tf;
            mem_cpy((uint8_t*)traverse_trapframe->func_args[1],(uint8_t*)text_buffer,(size_t)text_buf_index);
            traverse->proc_state = 0x1;
            flush_paging((uint64_t)current_proc->root_page_table);
          }
          if(traverse->next == current_proc){
            break;
          }
          else{
            traverse = traverse->next;
          }
        }
        text_buf_index = 0;
      }
      
      // Else if it is just a simple key just print it to the screen
      else{
        ecall_print((uint8_t*)char_addr,1);
        current_char = char_addr[0];
        text_buffer[text_buf_index] = char_addr[0];
        text_buf_index++;
      }
      

    }
    *write_ptr = claim;
  }
}
