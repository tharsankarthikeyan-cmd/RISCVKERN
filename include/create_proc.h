#ifndef CREATE_PROC_H
#define CREATE_PROC_H

#include "paging.h"
#include "pmm.h"

void* create_proc(void);
void* copy_page_data(void* page_entry_addr);


#endif // !CREATE_PROC_H

