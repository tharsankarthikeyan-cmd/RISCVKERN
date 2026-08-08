#include <stdint.h>
#include <stddef.h>

extern void sys_print(uint64_t x_coor, uint64_t y_coor, uint8_t* string_ptr, uint64_t length);

void _start(){
  sys_print(1,0,(uint8_t*)" ____  _               _                ____  _          _ _  ",61);
  sys_print(2,0,(uint8_t*)"/ ___|| |__   __ _  __| | _____      __/ ___|| |__   ___| | | ",61);
  sys_print(3,0,(uint8_t*)"\\___ \\| '_ \\ / _` |/ _` |/ _ \\ \\ /\\ / /\\___ \\| '_ \\ / _ \\ | | ",61);
  sys_print(4,0,(uint8_t*)" ___) | | | | (_| | (_| | (_) \\ V  V /  ___) | | | |  __/ | | ",61);
  sys_print(5,0,(uint8_t*)"|____/|_| |_|\\__,_|\\__,_|\\___/ \\_/\\_/  |____/|_| |_|\\___|_|_| ",61);
  sys_print(6,0,(uint8_t*)"> ",2);
  while(1);
}
