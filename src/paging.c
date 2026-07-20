#include <stddef.h>
#include <stdint.h>
#include "paging.h"
#include "pmm.h"

#define _RAM_TOTAL (uintptr_t)0x8000000ULL
extern void page_flush(uint64_t pte_giga_entry);
extern uint64_t _end; 

__attribute__((aligned(4096))) uint64_t pte_giga_entry[512];
__attribute__((aligned(4096)))uint64_t pte_mega_entry[512];
__attribute__((aligned(4096)))uint64_t pte_kilo_entry[512];

void paging_init(void){
  uint64_t gigs = (uint64_t)&_end;
  gigs = gigs - 0xFFFFFFC000000000;
  gigs = gigs/(1024*1024*1024);
  for(uint64_t i = 0; i < gigs; i++){
    pte_giga_entry[i+256] = (i*(1024*1024*1024) >> 2) | 0xCF;
  }
}

void paging_init_2(void){
  pte_giga_entry[256] = (uint64_t)0x0F;
  pte_giga_entry[257] = (uint64_t)(0x40000000) >> 2 | 0x0F;
  pte_giga_entry[258] = ((uint64_t)pte_mega_entry - 0xFFFFFFC000000000) >> 2 | 0x01;
  uint64_t kernel_kb = (((uint64_t)&_end - 0xFFFFFFC000000000) - 0x80200000)/4096;
  pte_mega_entry[0] = 0x80000000ULL >> 2 | 0x0F;
  //pte_mega_entry[1] = ((uint64_t)pte_kilo_entry - 0xFFFFFFC000000000) >> 2 | 0x01;
  //for(uint64_t i = 0; i < kernel_kb; i++){
    //pte_kilo_entry[i] = (0x80200000ULL + i*4096) >> 2 | 0xCF;
  //}
  uint64_t kernel_mb = (uint64_t)(_RAM_TOTAL / 0x200000);
  for(uint64_t i = 0; i < kernel_mb; i++){
    pte_mega_entry[i] = ((uintptr_t)0x80000000ULL + ((uintptr_t)(0x200000))*i) >> 2 | 0x0F;
  }
}


void flush_paging(uint64_t page_root){
  page_flush((uint64_t)((uintptr_t)(page_root) - (uintptr_t)0xFFFFFFC000000000ULL));
}


