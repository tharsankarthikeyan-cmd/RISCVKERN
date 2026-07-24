#include "../include/plic_mmap.h"
.global enter_proc
.extern page_flush
.extern mem_cpy
.extern user_prog_start
.extern user_prog_end
enter_proc:
  /* satp */
  /* make sure that satp entry with proper address is the first entry*/
  /* subtract 0xFFFFFFC000000000 */
  mv t0, a0
  li t1, 0xFFFFFFC000000000
  sub t0, t0, t1
  mv a0, t0
  call page_flush

  /*sscratch entry for trapframe*/
  csrw sscratch, a1
  mv t0, a1

  /*Allow Kernel Mem Access*/
  li t1, 0x40020
  csrw sstatus, t1

  /*memcpy the program*/
  li t0, 0x1000
  la t1, user_prog_start
  la t2, user_prog_end
  sub t3, t2, t1
  mv a0, t0
  mv a1, t1
  mv a2, t3
  call mem_cpy

  /* set the stack_pointer to trapframe*/
  csrr t0, sscratch
  mv sp, t0
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

  ld t0, 224(sp)
  csrw sstatus, t0

  ld t1, 232(sp)          # except_pc (sepc)
  csrw sepc, t1

  ld tp, 240(sp)          # Thread Pointer
  ld gp, 248(sp)          # Global Pointer

  # CLAIM/COMPLETE SAFETY CHECK
  li t0, 0xFFFFFFC000000000
  li t1, 0x0C201004
  add t0, t0, t1
  lw t1, 0(t0)
  sw t1, 0(t0)

  ld t2, 256(sp)          # stack_pointer field
  ld t0, 0(sp)
  ld t1, 8(sp)
  mv sp, t2
  sret
