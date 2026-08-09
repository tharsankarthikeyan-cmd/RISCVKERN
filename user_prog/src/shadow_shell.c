#include <stdint.h>
#include <stddef.h>

extern void sys_print(uint64_t x_coor, uint64_t y_coor, uint8_t* string_ptr, uint64_t length);
extern void sys_exit(void);
extern void sys_read(void);

void _start(){
  sys_print(1,0,(uint8_t*)" ____  _               _                ____  _          _ _  ",61);
  sys_print(2,0,(uint8_t*)"/ ___|| |__   __ _  __| | _____      __/ ___|| |__   ___| | | ",61);
  sys_print(3,0,(uint8_t*)"\\___ \\| '_ \\ / _` |/ _` |/ _ \\ \\ /\\ / /\\___ \\| '_ \\ / _ \\ | | ",61);
  sys_print(4,0,(uint8_t*)" ___) | | | | (_| | (_| | (_) \\ V  V /  ___) | | | |  __/ | | ",61);
  sys_print(5,0,(uint8_t*)"|____/|_| |_|\\__,_|\\__,_|\\___/ \\_/\\_/  |____/|_| |_|\\___|_|_| ",61);
  sys_print(6,0,(uint8_t*)"> ",2);
  uint8_t text_buffer[100];
  uint64_t text_buf_index = 0;
  while(text_buffer[text_buf_index-1] != '\r'){
    sys_read();
    __asm__ __volatile__(
      "mv %0, a0\n\t"
      :"=r"(text_buffer[text_buf_index])
      :
      :"a0"
    );
    text_buf_index++;
  }
  sys_print(10,10,(uint8_t*)text_buffer,text_buf_index);
  while(1);
}
