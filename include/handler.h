#ifndef HANDLER_H
#define HANDLER_H

#include <stddef.h>
#include <stdint.h>

void handler_function(uint64_t scause_reg,uint64_t prog_id, uint64_t value, uint64_t trapframe_reg);

#endif // !
