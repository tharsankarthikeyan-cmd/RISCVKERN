ENTRY(start);
 
. = 0x80200000;
 
SECTIONS {
	/* Include entry point at start of binary */

  /* Addition for the HHK*/
  . += 0xFFFFFFC000000000;



	.text ALIGN(4K): AT(ADDR(.text) - 0xFFFFFFC000000000) {
		*(.init);
		*(.text);
	}
	.bss ALIGN(4K): AT(ADDR(.bss) - 0xFFFFFFC000000000) {
		PROVIDE(bss_start = .);
		*(.bss);
		. += 4096;
		PROVIDE(stack_top = .);
		. += 4096;
		PROVIDE(global_pointer = .);
		PROVIDE(bss_end = .);
	}
	.rodata ALIGN(4K): AT(ADDR(.rodata) - 0xFFFFFFC000000000) {
		*(.rodata);
	}
	.data ALIGN(4K): AT(ADDR(.data) - 0xFFFFFFC000000000) {
		*(.data);
	}
  . = ALIGN(4096);
  _end = .;
}
