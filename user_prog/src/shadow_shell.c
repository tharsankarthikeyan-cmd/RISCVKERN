#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern void sys_print(uint64_t x_coor, uint64_t y_coor, uint8_t* string_ptr, uint64_t length);
extern void sys_exit(void);
extern void sys_read(uint8_t* text_buffer);
extern void sys_create_proc();

void _start(){
  sys_print(1,0,(uint8_t*)" ____  _               _                ____  _          _ _  ",61);
  sys_print(2,0,(uint8_t*)"/ ___|| |__   __ _  __| | _____      __/ ___|| |__   ___| | | ",61);
  sys_print(3,0,(uint8_t*)"\\___ \\| '_ \\ / _` |/ _` |/ _ \\ \\ /\\ / /\\___ \\| '_ \\ / _ \\ | | ",61);
  sys_print(4,0,(uint8_t*)" ___) | | | | (_| | (_| | (_) \\ V  V /  ___) | | | |  __/ | | ",61);
  sys_print(5,0,(uint8_t*)"|____/|_| |_|\\__,_|\\__,_|\\___/ \\_/\\_/  |____/|_| |_|\\___|_|_| ",61);
  sys_print(6,0,(uint8_t*)"> ",2);
  uint8_t text_buffer[100];
  uint64_t text_buf_index = 0;
  while(1){
    sys_read(text_buffer);
    while(text_buffer[text_buf_index] != 0x0){
      text_buf_index++;
    }
    sys_print(0,0,text_buffer,text_buf_index);
    text_buf_index = 0;
    sys_create_proc();
  }
  while(1);
}

