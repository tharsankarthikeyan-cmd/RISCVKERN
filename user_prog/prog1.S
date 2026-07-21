
.section .text
.global _start

li t1, 0xdeadc0de
_start:
    j _start    # Jump to the '_start' label infinitely
