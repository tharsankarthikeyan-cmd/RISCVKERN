
.section .text
.global _start

li a0, 0xdeadbeef
li a1, 0x0
li t0, 0x5000
sd a0, 0(t0)
_start:
    addi a1, a1, 1
    ecall
    j _start    # Jump to the '_start' label infinitely
