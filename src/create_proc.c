#include "paging.h"
#include "pmm.h"
#include "create_proc.h"

extern uint64_t _end;

void* create_proc(void){
  uint64_t* new_proc_entry = (uint64_t*)copy_page_data((void*)pte_giga_entry);
  return (void*)new_proc_entry;
}
void* copy_page_data(void* page_entry_addr){
  // allocation a new page for the new entry
  uint64_t* new_tab = (uint64_t*)((uintptr_t)page_alloc(4096) + (uintptr_t)0xFFFFFFC000000000ULL);

  // For All Indivdual Entries with in the given root_page_table
  for(uint64_t i = 0; i < 512; i++){
    
    // Take the current entry
    uint64_t current_entry = ((uint64_t*)page_entry_addr)[i];

    // If it is valid and it is an address
    if((current_entry & 0xF) == 0x1){

      // Take the address
      current_entry = ((current_entry >> 10) & 0xFFFFFFFFFFF) << 12;

      // Add HHK value to make sure that the table is accessible and then recusively call the function to create a new table
      void* next_page_entry = copy_page_data((void*)((uintptr_t)current_entry+(uintptr_t)0xFFFFFFC000000000ULL));
      
      // Make the new page table entry but with a subtraction of HHK for real physical address
      new_tab[i] = (((uintptr_t)next_page_entry - (uintptr_t)0xFFFFFFC000000000ULL) >> 2) | 0x01;
    }
    else if((current_entry & 0x1) == 0x1){
      // Else if It is not address but it is valid then
      //Take the current entry remove the flags
      current_entry = ((current_entry >> 10) & 0xFFFFFFFFFFF) << 10;

      // Add new flags and write to it with U-Mode Permissions
      new_tab[i] = ((uint64_t)current_entry) | 0x0F;
    }
    else{
      // None of the above. Just enter zero
      new_tab[i] = 0x0;
    }
  }
  return (void*)(new_tab);
}

void delete_page_tables(void* page_entry_addr){
  // Take the root pages table entry and add HHK to make it accessible
  uint64_t* current_entry = (uint64_t*)((uintptr_t)page_entry_addr + (uintptr_t)0xFFFFFFC000000000ULL);
  
  // For Each entry in that table
  for(uint64_t i = 0; i < 512; i++){

    // If the entry is an address 
    if((current_entry[i] & 0xF) == 0x1){
      // Take the current address
      uint64_t current_addr = ((current_entry[i] >> 10) & 0xFFFFFFFFFFF) << 12;

      // recusively delete the found out address
      delete_page_tables((void*)current_addr);
      
      // Free this address
      free_pages((void*)current_addr);
    }
    else if((current_entry[i] & 0xF) != 0x1 && (current_entry[i] & 0x1) == 0x1){
      // Else if it not an address and it is valid

      // Take the end of the kernel
      uintptr_t k_end = (uintptr_t)&_end;
      k_end -= (uintptr_t)(0xFFFFFFC000000000ULL);

      // If the current addr is physical entry is greater than the kernel only then must we free
      uint64_t current_addr = ((current_entry[i] >> 10) & 0xFFFFFFFFFFF) << 12;

      if((uintptr_t)current_addr > k_end){
        // Free them if and only if it is greater than kernel.
        free_pages((void*)current_addr);
      }
    }
  }
}

void demand_paging(void* root_page_table, void* stval_reg){
  // Take the current root page table
  uint64_t* current_addr = (uint64_t*)root_page_table;

  // Three different 9 bits are entries
  for(uint8_t i = 0; i < 3; i++){

    // Take the 9 bits based on which loop it is
    uint64_t page_frame_number = (uint64_t)((((uintptr_t)stval_reg >> 12) >> (9*(2-i))) & 0x1FF);

    // If the current entry is valid and it is an address then
    if((current_addr[page_frame_number] & 0xF) == 0x1){
      current_addr = (uint64_t*)((((uintptr_t)current_addr[page_frame_number] >> 10) & 0xFFFFFFFFFFF) << 12);
      current_addr = (uint64_t*)((uintptr_t)current_addr + (uintptr_t)0xFFFFFFC000000000ULL);
    }
    else if((current_addr[page_frame_number] & 0x1) != 0x1){
      uint64_t* new_table_entry = (uint64_t*)(page_alloc(4096));
      for(uint64_t j = 0; j < 512; j++){
        ((uint64_t*)((uintptr_t)new_table_entry + (uintptr_t)0xFFFFFFC000000000ULL))[j] = 0x0;
      }
      if(i < 2){
        current_addr[page_frame_number] = ((uintptr_t)new_table_entry >> 2) | 0x01;
        current_addr = (uint64_t*)((uintptr_t)new_table_entry + (uintptr_t)0xFFFFFFC000000000ULL);
      }
      else{
        current_addr[page_frame_number] = ((uintptr_t)new_table_entry >> 2) | 0x1F;
      }
    }
  }
}
