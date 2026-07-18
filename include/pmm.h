#ifndef PMM_H
#include <stdint.h>
#include <stddef.h>
#include "common.h"

void pmm_init(void);
void* page_alloc(uint64_t bytes);
void free_pages(void* ptr);

#endif // PMM_H!
