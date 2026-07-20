#ifndef PROC_H
#define PROC_H
#include <stdint.h>
#include <stddef.h>

typedef struct trapframe_t{
  uint64_t temp_reg[7];
  uint64_t func_args[8];
  uint64_t saved_reg[12];
  uint64_t ret_addr;
  uint64_t s_stat;
  uint64_t except_pc;
  uint64_t thread_pointer;
  uint64_t global_pointer;
  uint64_t stack_pointer;
}__attribute__((packed)) trapframe_t;

typedef struct Proc{
  uint8_t pid;
  void* root_page_table;
  trapframe_t* tf;
}__attribute__((packed)) Proc;

extern Proc init1;

void init_proc(void);


#endif // !PROC_H
