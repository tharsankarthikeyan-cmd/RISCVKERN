.global page_flush
page_flush:
  mv t0, a0
  li t1, 0x8000000000000000
  srli t0, t0, 12
  or t0, t0, t1
  csrw satp, t0
  sfence.vma
  ret

