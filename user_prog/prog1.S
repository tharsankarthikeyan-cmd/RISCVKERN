
.section .text
.global _start

li a1, 0xdeadc0de
li a2, 0x0
call _start
_start:
    addi a2, a2, 1 
    ecall
    j _start    # Jump to the '_start' label infinitely
