#ifndef PAGING_H
#define PAGING_H
#include <stddef.h>
#include <stdint.h>


extern uint64_t pte_giga_entry[512];
extern uint64_t pte_mega_entry[512];
extern uint64_t pte_kilo_entry[512];
void paging_init(void);
void paging_init_2(void);
void flush_paging(void);
#endif // !PAGING_H


