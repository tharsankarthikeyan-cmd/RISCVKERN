#ifndef PLIC_MMAP
#define PLIC_MMAP
#include <stddef.h>
#include <stdint.h>

#define PLIC_BASE 0x0C000000ULL
#define PLIC_SOURCE_10_PRIORITY ((volatile uint32_t*)(PLIC_BASE + 0x000028ULL))
#define PLIC_ENABLE_CTX_1 ((volatile uint32_t*)(PLIC_BASE + 0x002080ULL))
#define PLIC_THERSHOLD ((volatile uint32_t*)(PLIC_BASE + 0x201000ULL))
#define PLIC_CLAIM_COMPLETE ((volatile uint32_t*)(PLIC_BASE + 0x201004ULL))

void plic_mmap(void);


#endif // PLIC_MMAP

