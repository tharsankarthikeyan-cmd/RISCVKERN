#include <stddef.h>
#include <stdint.h>
#include "plic_mmap.h"

void plic_mmap(void){
  *PLIC_SOURCE_10_PRIORITY = 1;
  *PLIC_ENABLE_CTX_1 = 0x400;
  *PLIC_THERSHOLD = 0;
}
