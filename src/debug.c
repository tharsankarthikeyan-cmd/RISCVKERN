#include <stddef.h>
#include <stdint.h>
#include "debug.h"

uint64_t ecall_print(uint8_t line[], uint64_t len){
  register uint64_t a0 __asm__("a0") = len;
  register uint64_t a1 __asm__("a1") = (uint64_t)(line - 0xFFFFFFC000000000);
  register uint64_t a2 __asm__("a2") = 0;
  register uint64_t a6 __asm__("a6") = 0;
  register uint64_t a7 __asm__("a7") = 0x4442434E;
  __asm__ __volatile__(
    "ecall"
    : "+r"(a0), "+r"(a1)
    : "r"(a2), "r"(a6), "r"(a7)
    : "memory"
  );
  return a0;
}
