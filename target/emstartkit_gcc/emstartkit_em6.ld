/* Linker script for emstartkit
 *
 * 
 */
OUTPUT_FORMAT ("elf32-littlearc", "elf32-bigarc", "elf32-littlearc")
OUTPUT_ARCH(arc)
SEARCH_DIR(.)
GROUP(-lgcc -lc)
PROVIDE(hardware_init_hook = 0);
PROVIDE(software_init_hook = 0);
PROVIDE(software_term_hook = 0);
STARTUP(start.o)

MEMORY
{
	vtable : ORIGIN = 0x0, LENGTH = 0x100
	iccm : ORIGIN = 0x100, LENGTH = 32K - 0x100
	ddr : ORIGIN = 0x10000000, LENGTH = 128m
}

SECTIONS
{
	.vector : { 
		KEEP (*(.vector))
	} > vtable

	.text :
	{
		__text = .;
		*(.text .text.* .gnu.linkonce.t.*)
	} >iccm

	.rodata : ALIGN (4)
	{
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		. = ALIGN (8);
		_etext = .;
	} >iccm

	__idata_start = .;
	.data : ALIGN (8)
	{
		__data_start = .;
		*(.data .data.* .gnu.linkonce.d.*)
		__SDATA_BEGIN__ = .;
		*(.sdata .sdata.* .gnu.linkonce.s.*)
		. = ALIGN (8);
		_edata = .;
	} >ddr
	__idata_end = __idata_start + SIZEOF(.data);

	.bss :
	{
		__bss_start = .;
		PROVIDE (__sbss_start = .);
		PROVIDE (___sbss_start = .);
		*(.dynsbss)
		*(.sbss .sbss.* .gnu.linkonce.sb.*)
		*(.scommon)
		*(.dynbss)
		PROVIDE (__sbss_end = .);
		PROVIDE (___sbss_end = .);
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
		. = ALIGN (8);
		_end = .;
		__end = .;
		__bss_end = .;
		PROVIDE (end = .);
		__start_heap = .;
		__end_heap = 0x10000000 + 128m;
	} >ddr

/* Stabs debugging sections.  */
	.stab          0 : { *(.stab) }
	.stabstr       0 : { *(.stabstr) }
	.stab.excl     0 : { *(.stab.excl) }
	.stab.exclstr  0 : { *(.stab.exclstr) }
	.stab.index    0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment       0 : { *(.comment) }
  /* DWARF debug sections.
   * Symbols in the DWARF debugging sections are relative to the beginning
   * of the section so we begin them at 0.  */
  /* DWARF 1 */
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
	.debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev   0 : { *(.debug_abbrev) }
	.debug_line     0 : { *(.debug_line) }
	.debug_frame    0 : { *(.debug_frame) }
	.debug_str      0 : { *(.debug_str) }
	.debug_loc      0 : { *(.debug_loc) }
	.debug_macinfo  0 : { *(.debug_macinfo) }
  /* DWARF 2.1 */
	.debug_ranges   0 : { *(.debug_ranges) }
  /* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }
  /* ARC Extension Sections */
	.arcextmap	  0 : { *(.gnu.linkonce.arcextmap.*) }
	/DISCARD/ : { *(.__arc_profile_*) }
	/DISCARD/ : { *(.note.GNU-stack) }
}
