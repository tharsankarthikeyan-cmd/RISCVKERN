#include <stdint.h>
#include <stddef.h>

extern void sys_print(uint64_t x_coor, uint64_t y_coor, uint8_t* string_ptr, uint64_t length);

void _start(){
  uint8_t hello[] = "Hello";
  sys_print(20,20,(uint8_t*)hello,5);
  while(1);
}
