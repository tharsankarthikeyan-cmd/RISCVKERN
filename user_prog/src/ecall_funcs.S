.global sys_print
.global sys_exit
.global sys_read
.global sys_create_proc

sys_print:
  mv a4, a3
  mv a3, a2
  mv a2, a1
  mv a1, a0
  li a0, 0x2
  ecall
  ret

sys_exit:
  li a0, 0x0
  ecall
  ret

sys_read:
  mv a1, a0
  li a0, 0x3
  ecall
  ret

sys_create_proc:
  mv a2, a1
  mv a1, a0
  li a0, 0x1
  ecall
  ret
