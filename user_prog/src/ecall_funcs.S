.global sys_print

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
