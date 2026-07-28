
.section .text
.global _start

li a1, 0xdeadbeef
_start:
    ecall
    j _start    # Jump to the '_start' label infinitely
