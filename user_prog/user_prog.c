#include <stdint.h>
#include <stddef.h>
void _start() {
  // Pointer to the volatile UART memory address
  volatile uint32_t* test_addr1 = (volatile uint32_t*)0xdeadbeef;
  volatile uint32_t* test_addr2 = (volatile uint32_t*)0xdeadc0de;
  *test_addr1 = 0xdeadc0de;
  *test_addr2 = 0xdeadbeef;
  while(1);
}
