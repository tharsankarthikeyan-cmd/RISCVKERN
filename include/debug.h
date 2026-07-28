#ifndef DEBUG_H
#define DEBUG_H
#include <stddef.h>
#include <stdint.h>


 
uint64_t ecall_print(uint8_t line[], uint64_t len);
void uart_int(uint64_t number,uint8_t vga_str[],uint8_t size);
#endif // !DEBUG

