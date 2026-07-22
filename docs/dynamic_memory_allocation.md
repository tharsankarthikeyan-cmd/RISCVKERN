# Dynamic Memory Allocation in Operating Systems
There are some very essential dynamics memory allocation algorithms in Operating System for Memory and Free Space Management, Given below are some of the very important algorithms

- Page Frame Allocator 
- SLUB/SLAB Allocator
- Virtual Memory Allocator

Within Page Frame Allocator there are two very essential algorithms which are implemented across hobby operating system

- Bitmap Allocator
- Buddy Allocator

## Bitmap Allocator
Page Frame allocator are allocator which allocate pages based on the given amount required and mark them as used. They must not allocate a page already allocated and not yet freed by the Operating System or Process, Furthermore, They generally allocate pages in the form of 4KB pages, In our own Hobby Operating System, We have made a page frame allocator using the Bitmap System

A bitmap system is significantly easier and has less metadata and memory overhead, but the main disadvantage of this system is the O(N), worst case scenario search algorithm
In the bitmap system each page is represented by a single bit.
For eg.
In an normal RISC V system with 128MB of RAM
```text
_mmio_start = 0x00000000
_ram_start = 0x80000000 (2GB)
_kernel_start = 0x80200000 (2GB + 2MB)
_kernel_end = somewhere between _ram_end and _kernel_start
_ram_end = 0x88000000 (128MB + 2GB)
```

The 2MB is used for OpenSBI and The rest of the memory about 0x80000000 is used for MMIO
We must divide the entire RAM space from start to end into 4KB pages and create a Bitmap storing all of it
```text
Total RAM = 0x88000000 - 0x80000000 = 0x8000000 (128MB)
Total 4KB Pages = 0x8000000(128MB) / 4096(4KB) = 32,768 Page Available
No of 64-bit Integers = floor(32,768 / 64) = 512 (64 bit Entries)
```

For each of those 512 Pages We store a single bit storing whether the Page is Allocated or Free.
Furthermore, When allocation we must implement an algorithm to allocate pages contiguously
Hence we would create another 512*64 bit entry to store the state of the page, where contiguous or not.
Now we have two arrays
```c
uint64_t is_page_alloc[512];
uint64_t is_page_contigious[512];
```
Now we must create an algorithm which is used to Allocate and Mark the Pages as Allocated or Contiguous
In RISC V System we have a bit manipulation extension which has proved useful in this scenario called ctz or Count Trailing Zeroes
Hence we would use that algorithm here, but if in case the Board does not come with the ctz extension we must implement our own ctz in order to get the allocator working.

Firstly we must be able to allocate a number of pages for the already existing kernel based on the linker symbol _end, which is at a higher address after Higher Half Kernel Paging 
```c
// Calculate the Number of Pages Required for the kernel
extern uint64_t _end;
#define _RAM_START (uintptr_t)0x80000000ULL
#define _TOTAL_RAM (uintptr_t)0x8000000ULL
#define _RAM_END (_RAM_START + _TOTAL_RAM)
#define _END (uintptr_t)&_end
#define _USABLE_RAM (_TOTAL_RAM - ((_END - (uintptr_t)0xFFFFFFC000000000ULL) - _RAM_START)) 
#define _TOTAL_PAGES _TOTAL_RAM/4096
#define _TOTAL_BITMAP_64 ((_TOTAL_PAGES+63)/64)
// Given above are some definitions regarding the kernel and RAM
// _END is the most important because this is where the kernel ends

// GIVEN BELOW IS THE FUNCTION WHICH HANDLES INITIALIZATION
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
```
A common pattern that you note here is
```text
(1ULL << Variable) - 1;
For eg if we want a bitmap of three 111's like this
then we perform
1ULL << 3 which gives 1000
Subtracting one from it means 111, hence we do that
Further we can shift 111 << variable to reach the desired position based on the address
```

The Given above function makes the entries of the kernel are allocated, Which means the main Function does not search for the bits in the kernel space
Now we create a function which helps us figure out and return an address of free pages based on number of bytes requested for
The function for such is heavily documented and written below
```c
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
```

Further more we require a short algorithm for free as well
```c
// Free Pages
void free_pages(void* ptr){
  // Given Pointer Converted to uintptr_t for arithmetic
  uintptr_t current_ptr = (uintptr_t)ptr;

  // Total No of Pages Before this Address
  uint64_t total_pages = (uint64_t)((current_ptr - _RAM_START)/4096);

  // Divide by 64 the total pages in order to the index in the array
  uint64_t bitmap_index = total_pages/64;

  // Find the remainder to find the entry within one uint64_t in the array
  uint64_t bitmap_entry_index = total_pages%64;

  // Bitshift the Current Entry in array until the start of page given by the address comes
  uint64_t current_page_map = is_contigious_bitmap[bitmap_index] >> bitmap_entry_index;

  // Invert it to make sure that the Allocated 1's turn to 0 so that we can use ctz to count tariling zeros
  current_page_map = ~(current_page_map);

  // Find the number of zeros and take it as total occupied pages
  uint64_t tot_occupied_pages = (uint64_t)__builtin_ctzll(current_page_map);

  // Find the perfect bitmap where the space taken by the pages is zero
  uint64_t mask_pages = ~(((1ULL << tot_occupied_pages) - 1) << bitmap_entry_index);

  // The apply the same bitmap to both
  is_alloc_bitmap[bitmap_index] = is_alloc_bitmap[bitmap_index] & mask_pages;
  is_contigious_bitmap[bitmap_index] = is_contigious_bitmap[bitmap_index] & mask_pages;
}
```
With this our basic bitmap allocator is complete
