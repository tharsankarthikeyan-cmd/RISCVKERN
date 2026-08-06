.global sys_print

sys_print:
  mov a3, a2
  mov a2, a1
  mov a1, a0
  li a0, 0x2
  ecall
