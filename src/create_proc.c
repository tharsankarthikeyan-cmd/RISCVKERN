#include "paging.h"
#include "pmm.h"
#include "create_proc.h"

extern uint64_t _end;

void* create_proc(void){
  uint64_t* new_proc_entry = (uint64_t*)copy_page_data((void*)pte_giga_entry);
  return (void*)new_proc_entry;
}
void* copy_page_data(void* page_entry_addr){
  uint64_t* new_tab = (uint64_t*)((uintptr_t)page_alloc(4096) + (uintptr_t)0xFFFFFFC000000000ULL);
  for(uint64_t i = 0; i < 512; i++){
    uint64_t current_entry = ((uint64_t*)page_entry_addr)[i];
    if((current_entry & 0xF) == 0x1){
      current_entry = ((current_entry >> 10) & 0xFFFFFFFFFFF) << 12;
      void* next_page_entry = copy_page_data((void*)((uintptr_t)current_entry+(uintptr_t)0xFFFFFFC000000000ULL));
      new_tab[i] = (((uintptr_t)next_page_entry - (uintptr_t)0xFFFFFFC000000000ULL) >> 2) | 0x01;
    }
    else if((current_entry & 0x1) == 0x1){
      current_entry = ((current_entry >> 10) & 0xFFFFFFFFFFF) << 10;
      new_tab[i] = ((uint64_t)current_entry) | 0x0F;
    }
    else{
      new_tab[i] = 0x0;
    }
  }
  return (void*)(new_tab);
}

void delete_page_tables(void* page_entry_addr){
  uint64_t* current_entry = (uint64_t*)((uintptr_t)page_entry_addr + (uintptr_t)0xFFFFFFC000000000ULL);
  for(uint64_t i = 0; i < 512; i++){
    if((current_entry[i] & 0xF) == 0x1){
      uint64_t current_addr = ((current_entry[i] >> 10) & 0xFFFFFFFFFFF) << 12;
      delete_page_tables((void*)current_addr);
      free_pages((void*)current_addr);
    }
    else if((current_entry[i] & 0xF) != 0x1 && (current_entry[i] & 0x1) == 0x1){
      uintptr_t k_end = (uintptr_t)&_end;
      k_end -= (uintptr_t)(0xFFFFFFC000000000ULL);
      uint64_t current_addr = ((current_entry[i] >> 10) & 0xFFFFFFFFFFF) << 12;
      if((uintptr_t)current_addr > k_end){
        free_pages((void*)current_addr);
      }
    }
  }
}
