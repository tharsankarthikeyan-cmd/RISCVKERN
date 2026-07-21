#include <stddef.h>
#include <stdint.h>

void* mem_cpy(void* dest, const void* src, size_t n){
  uint8_t* d = (uint8_t*)dest;
  const uint8_t* s = (const uint8_t*)src;
  while(n--){
    *d++ = *s++;
  }
  return dest;
}
