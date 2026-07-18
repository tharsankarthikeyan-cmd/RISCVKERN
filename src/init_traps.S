.extern global_handler

.section .text
.global init_traps
init_traps:
  la t0, global_handler
  csrw stvec, t0
  li t0, 0x200
  csrs sie, t0
  li t0, 0x2
  csrs sstatus, t0
  ret

  
