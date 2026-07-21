/* Initial Boiler Plate */
.section .init

.option norvc

.type start, @function
.global start
start:
	.cfi_startproc
/* End of Initial Boiler Plate */


/* Push the Compiler Settings */
.option push

/* Make sure Linker Relax is not used */
.option norelax

/* Load the Stack with Subtract */
la sp, stack_top
/*li t0, 0xFFFFFFC000000000
sub sp, sp, t0*/
done1:
la t0, kernel_end
ld t1, 0(t0)
done2:
/* Reset the SATP */
csrw satp, zero

jal done3
/* Paging Work */
.align 12
pte_entry_lvl1:
  .dword ((0x00000000)|0xCF)
  .dword ((0x40000000/4)|0xCF)
  .dword ((0x80000000/4)|0xCF)
  .dword ((0xC0000000/4)|0xCF)
  .space 2016
  .dword ((0x00000000/4)|0xCF)
  .dword ((0x40000000/4)|0xCF)
  .dword ((0x80000000/4)|0xCF)
  .dword ((0xC0000000/4)|0xCF)
  .space 2016

addr_early_kernel_main:
  .dword early_kernel_main

bss_section:
  .dword bss_start
  .dword bss_end

kernel_end:
  .dword _end

done3:
/* Paging Enable */
la t0, pte_entry_lvl1
srli t0, t0, 12
li t1, 0x8000000000000000
or t1, t1, t0
done4:

la t0, addr_early_kernel_main
ld t3, 0(t0)
/*li t2, 0xFFFFFFD000000000
add t0, t0, t2*/
csrw satp, t1
done5:
sfence.vma
fence.i
done6:
jr t3


early_kernel_main:
	/* Clear the BSS section */
  la t0, bss_section
	ld t5, 0(t0)
	ld t6, 8(t0)

/*bss_clear:
	sd zero, (t5)
	addi t5, t5, 8
	bltu t5, t6, bss_clear*/

  /* Setting up the real stack_top */
  la sp, stack_top
  /* Setting up gp */
  la gp, global_pointer
  done7:
.option pop

  tail kmain

	.cfi_endproc

.section .rodata
.align 4
.global user_prog_start
.global user_prog_end

user_prog_start:
  .incbin "user_prog/prog1.bin"
user_prog_end:

.end
