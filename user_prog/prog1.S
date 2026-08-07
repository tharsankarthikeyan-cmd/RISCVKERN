
.section .text
.global _start

li a0, 0xdeadc0de
li a1, 0x0
call _start
_start:
    addi a1, a1, 1 
    ecall
    j _start    # Jump to the '_start' label infinitely
