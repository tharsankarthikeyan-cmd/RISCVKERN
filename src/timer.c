#include <stdint.h>
#include <stddef.h>
#include "timer.h"

void ecall_timer_set(void){
  __asm__ __volatile__(
    "rdtime t0\n\t"
    "li t1, 0x100000000\n\t"
    "add a0, t0, t1\n\t"
    "li a7, 0x54494D45\n\t"
    "li a6, 0x0\n\t"
    "ecall\n\t"
    :
    :
    :"a0","a1","a6","a7","t1","t0"
  );
}
