#include "../include/plic_mmap.h"
.extern handler_function

.section .text
.align 4
.global global_handler
global_handler:

  # Conditional for U-Mode Interrupts
  # Swap the SSCRATCH VALUE FOR CHECKING
  csrrw t0, sscratch, t0
  # Check if t0 is zero
  beqz t0, kernel_interrupt

#Interrupts handler_function for user processes
user_proc_interrupt:
  # Swap once more to take care of sscratch
  csrrw t0, sscratch, t0

  #Disable Interrupts
  csrci sstatus, 2

  # Swap SSCRATCH and SP
  csrrw sp, sscratch, sp
  sd t0, 0(sp)
  sd t1, 8(sp)
  sd t2, 16(sp)
  sd t3, 24(sp)
  sd t4, 32(sp)
  sd t5, 40(sp)
  sd t6, 48(sp)
  sd a0, 56(sp)
  sd a1, 64(sp)
  sd a2, 72(sp)
  sd a3, 80(sp)
  sd a4, 88(sp)
  sd a5, 96(sp)
  sd a6, 104(sp)
  sd a7, 112(sp)
  sd s0, 120(sp)
  sd s1, 128(sp)
  sd s2, 136(sp)
  sd s3, 144(sp)
  sd s4, 152(sp)
  sd s5, 160(sp)
  sd s6, 168(sp)
  sd s7, 176(sp)
  sd s8, 184(sp)
  sd s9, 192(sp)
  sd s10, 200(sp)
  sd s11, 208(sp)
  sd ra, 216(sp)

  # FOR CSR
  csrr t0, sstatus
  andi t0, t0, -3
  sd t0, 224(sp)
  csrr t0, sepc
  addi t0, t0, 4
  sd t0, 232(sp)
  sd tp, 240(sp)
  sd gp, 248(sp)
  csrr t0, sscratch
  sd t0, 256(sp)

  # SET sscratch to zero for further Interrupts
  csrw sscratch, x0

  # Enable Interrupts Safely
  csrsi sstatus, 2

  csrr a0, scause
  call handler_function

  ld t0, 0(sp)
  ld t1, 8(sp)
  ld t2, 16(sp)
  ld t3, 24(sp)
  ld t4, 32(sp)
  ld t5, 40(sp)
  ld t6, 48(sp)
  ld a0, 56(sp)
  ld a1, 64(sp)
  ld a2, 72(sp)
  ld a3, 80(sp)
  ld a4, 88(sp)
  ld a5, 96(sp)
  ld a6, 104(sp)
  ld a7, 112(sp)
  ld s0, 120(sp)
  ld s1, 128(sp)
  ld s2, 136(sp)
  ld s3, 144(sp)
  ld s4, 152(sp)
  ld s5, 160(sp)
  ld s6, 168(sp)
  ld s7, 176(sp)
  ld s8, 184(sp)
  ld s9, 192(sp)
  ld s10, 200(sp)
  ld s11, 208(sp)
  ld ra, 216(sp)
  
  # CSR Registers
  ld t0, 224(sp)
  csrw sstatus, t0
  ld t0, 232(sp)
  csrw sepc, t0
  ld tp, 240(sp)
  ld gp, 248(sp)

# CLAIM COMPLETE CYCLE.
  li t0, 0xFFFFFFC000000000
  li t1, 0x0C201004
  add t0, t0, t1
  lw t1, 0(t0)
  sw t1, 0(t0)

  ld t0, 0(sp)
  ld t1, 8(sp)
  csrw sscratch, sp
  ld sp, 256(sp)
  sret

# Interrupts handler_function for keyboard Interrupts during kernel mode
kernel_interrupt:

  # Swap once more to take care of the sscratch 
  csrrw t0, sscratch, t0

  # 1. Allocate 256 bytes on the stack (16-byte aligned for RV64)
  addi sp, sp, -256

  # 2. Store all 64-bit registers using 'sd' (Store Doubleword)
  sd x1,   0(sp)
  sd x3,   8(sp)
  sd x4,   16(sp)
  sd x5,   24(sp)
  sd x6,   32(sp)
  sd x7,   40(sp)
  sd x8,   48(sp)
  sd x9,   56(sp)
  sd x10,  64(sp)
  sd x11,  72(sp)
  sd x12,  80(sp)
  sd x13,  88(sp)
  sd x14,  96(sp)
  sd x15,  104(sp)
  sd x16,  112(sp)
  sd x17,  120(sp)
  sd x18,  128(sp)
  sd x19,  136(sp)
  sd x20,  144(sp)
  sd x21,  152(sp)
  sd x22,  160(sp)
  sd x23,  168(sp)
  sd x24,  176(sp)
  sd x25,  184(sp)
  sd x26,  192(sp)
  sd x27,  200(sp)
  sd x28,  208(sp)
  sd x29,  216(sp)
  sd x30,  224(sp)
  sd x31,  232(sp)

  # 3. Execute your high-level C/C++ trap handler
  csrr a0, scause
  call handler_function

  # 4. Restore all 64-bit registers using 'ld' (Load Doubleword)
  ld x1,   0(sp)
  ld x3,   8(sp)
  ld x4,   16(sp)
  ld x5,   24(sp)
  ld x6,   32(sp)
  ld x7,   40(sp)
  ld x8,   48(sp)
  ld x9,   56(sp)
  ld x10,  64(sp)
  ld x11,  72(sp)
  ld x12,  80(sp)
  ld x13,  88(sp)
  ld x14,  96(sp)
  ld x15,  104(sp)
  ld x16,  112(sp)
  ld x17,  120(sp)
  ld x18,  128(sp)
  ld x19,  136(sp)
  ld x20,  144(sp)
  ld x21,  152(sp)
  ld x22,  160(sp)
  sd x23,  168(sp)
  ld x24,  176(sp)
  ld x25,  184(sp)
  ld x26,  192(sp)
  ld x27,  200(sp)
  ld x28,  208(sp)
  ld x29,  216(sp)
  ld x30,  224(sp)
  ld x31,  232(sp)

  # 5. Free the 256 bytes of stack space
  addi sp, sp, 256

  # 6. Return from supervisor trap
  sret
