#ifndef CREATE_PROC_H
#define CREATE_PROC_H

#include "paging.h"
#include "pmm.h"

void* create_proc(void);
void* copy_page_data(void* page_entry_addr);
void delete_page_tables(void* page_entry_addr);
void demand_paging(void* root_page_table, void* stval_reg);

#endif // !CREATE_PROC_H

