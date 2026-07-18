#include <stddef.h>
#include <stdint.h>
#include "common.h"
#include "pmm.h"


//extern uint64_t is_alloc_bitmap[];
//extern uint64_t is_contigious_bitmap[];

extern uint64_t _end;

#define _RAM_START (uintptr_t)0x80000000ULL
#define _TOTAL_RAM (uintptr_t)0x8000000ULL
#define _RAM_END (_RAM_START + _TOTAL_RAM)
#define _END (uintptr_t)&_end
#define _USABLE_RAM (_TOTAL_RAM - ((_END - (uintptr_t)0xFFFFFFC000000000ULL) - _RAM_START)) 
#define _TOTAL_PAGES _TOTAL_RAM/4096
#define _TOTAL_BITMAP_64 ((_TOTAL_PAGES+63)/64)
uint64_t is_alloc_bitmap[_TOTAL_BITMAP_64];
uint64_t is_contigious_bitmap[_TOTAL_BITMAP_64];

void pmm_init(void){
  // Find the amount of the pages allocated for the kernel
  uint64_t k_occupied = (_TOTAL_RAM - _USABLE_RAM)/4096;

  // Find the 64 bitmaps it takes
  uint64_t k_occupied_bitmaps = k_occupied/64;

  // Find the remainder of the 64 division
  uint64_t k_occupied_remainder = k_occupied % 64;

  // For each of the uint64_t entry coming under the kernel, Fill it with one to not allocate it.
  for(uint64_t i = 0; i < k_occupied_bitmaps; i++){
    is_alloc_bitmap[i] = 0xFFFFFFFFFFFFFFFF;
    is_contigious_bitmap[i] = 0xFFFFFFFFFFFFFFFF;
  }

  // Fill it with one for the remainder.
  is_alloc_bitmap[k_occupied_bitmaps] = (1ULL << k_occupied_remainder) - 1;
  k_occupied_remainder -= 1;
  is_contigious_bitmap[k_occupied_bitmaps] = (1ULL << k_occupied_remainder) - 1;
}

void* page_alloc(uint64_t bytes){
  // Find out the number of pages required by dividing by 4KB;
  uint64_t k_occupied = (_TOTAL_RAM - _USABLE_RAM)/4096;
  uint64_t k_occupied_bitmaps = k_occupied/64;
  uint64_t k_occupied_remainder = k_occupied % 64;
  uint64_t pages_req = bytes/4096;
  pages_req += 1;

  // For Each uint64_t bitmap entry
  for(uint64_t i = k_occupied_bitmaps; i < _TOTAL_BITMAP_64; i++){
    uint64_t current_map = is_alloc_bitmap[i];

    // For Each Entry within the uint64_t
    for(uint64_t j = 0; j < 64; j++){

      // Find the number of free pages using trailing zeros
      uint64_t trail_zero = (uint64_t)__builtin_ctzll(current_map);

      // If the Number of trailing zero is zero then the this current page is occupied, 
      if(trail_zero == 0){
        // bitshift it by one to get to the next one, and then update j by one, the for loops takes care of this
        current_map = current_map >> 1;
      }

      // If the no of trailing zero is not zero meaning there are some free pages ,then
      else{

        // Check if the number of trailing zero is greater than or equal to what we want exactly
        if(trail_zero >= pages_req){

          // If yes, then
          // Create a bitmask where we essential set the number of pages assigned as 1s,for eg if three pages are assigned then 111
          uint64_t mask_pages = (1ULL << pages_req) - 1;

          // Then we bitshift it by the position of j, where j represent where we are in the current uint64_t entry.
          mask_pages = mask_pages << j;

          // Applyt the mask with OR
          is_alloc_bitmap[i] = is_alloc_bitmap[i] | mask_pages;

          // Similar to this , is_contigious_bitmap required one less zero, beacause the last page is not contigious.
          pages_req -= 1;
          mask_pages = (1ULL << pages_req) - 1;
          mask_pages = mask_pages << j;

          // We apply the calculated bitmask
          is_contigious_bitmap[i] = is_contigious_bitmap[i] | mask_pages;

          // Calculate the proper index of the entry from the start by using the below Calculate
          void* return_addr = (void*)_RAM_START + (i*64 + j)*4096;
          // return the address
          return return_addr;
        }

        // If the no of tariling zero is no the number of zeros we require then we shift it by the trailing value to go the next section.
        else{
          current_map = current_map >> trail_zero;

          // Be sure to increment, by the amount of trailing zeros.
          j += trail_zero;
        }
      }
    }
  }
  // If page is found, then return 0x0;
  return (void*)(0);
}



// Free Pages
void free_pages(void* ptr){
  uintptr_t current_ptr = (uintptr_t)ptr;
  uint64_t total_pages = (uint64_t)((current_ptr - _RAM_START)/4096);
  uint64_t bitmap_index = total_pages/64;
  uint64_t bitmap_entry_index = total_pages%64;
  uint64_t current_page_map = is_alloc_bitmap[bitmap_index] >> bitmap_entry_index;
  current_page_map = ~(current_page_map);
  uint64_t tot_occupied_pages = (uint64_t)__builtin_ctzll(current_page_map);
  uint64_t mask_pages = ~(((1ULL << tot_occupied_pages) - 1) << bitmap_entry_index);
  is_alloc_bitmap[bitmap_index] = is_alloc_bitmap[bitmap_index] & mask_pages;
  is_contigious_bitmap[bitmap_index] = is_contigious_bitmap[bitmap_index] & mask_pages;
}
