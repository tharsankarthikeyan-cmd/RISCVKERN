#include <stdint.h>
#include <stddef.h>


extern void sys_print(uint64_t x_coor, uint64_t y_coor, uint8_t* string_ptr, uint64_t length);
extern void sys_exit(void);
extern void sys_read(uint8_t* text_buffer);
extern void sys_create_proc();

void _start(void){
  uint8_t os_str[24] = "Shadow Operating System";
  sys_print(0,0,(uint8_t*)os_str,24);
  sys_exit();
}
