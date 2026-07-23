#include <stddef.h>
#include <stdint.h>
#include "debug.h"
#include "plic_mmap.h"
#include "handler.h"

void handler_function(uint64_t scause_reg){
  if(scause_reg == 8){
    ecall_print((uint8_t*)"El Psy Kongroo", 14);
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
