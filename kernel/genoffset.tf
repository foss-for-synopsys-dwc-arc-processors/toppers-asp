$ ======================================================================
$ 
$   TOPPERS/ASP Kernel
$       Toyohashi Open Platform for Embedded Real-Time Systems/
$       Advanced Standard Profile Kernel
$ 
$   Copyright (C) 2011 by Embedded and Real-Time Systems Laboratory
$               Graduate School of Information Science, Nagoya Univ., JAPAN
$  
$	The above copyright holders grant permission gratis to use,
$	duplicate, modify, or redistribute (hereafter called use) this
$	software (including the one made by modifying this software),
$	provided that the following four conditions (1) through (4) are
$	satisfied.
$
$	(1) When this software is used in the form of source code, the above
$    	copyright notice, this use conditions, and the disclaimer shown
$    	below must be retained in the source code without modification.
$
$	(2) When this software is redistributed in the forms usable for the
$    	development of other software, such as in library form, the above
$    	copyright notice, this use conditions, and the disclaimer shown
$    	below must be shown without modification in the document provided
$    	with the redistributed software, such as the user manual.
$
$	(3) When this software is redistributed in the forms unusable for the
$    	development of other software, such as the case when the software
$    	is embedded in a piece of equipment, either of the following two
$   	 conditions must be satisfied:
$
$  		(a) The above copyright notice, this use conditions, and the
$         	disclaimer shown below must be shown without modification in
$     		the document provided with the redistributed software, such as
$      		the user manual.
$
$ 		(b) How the software is to be redistributed must be reported to the
$     		TOPPERS Project according to the procedure described
$     		separately.
$
$	(4) The above copyright holders and the TOPPERS Project are exempt
$    	from responsibility for any type of damage directly or indirectly
$   	caused from the use of this software and are indemnified by any
$    	users or end users of this software from any and all causes of
$    	action whatsoever.
$
$	THIS SOFTWARE IS PROVIDED "AS IS." THE ABOVE COPYRIGHT HOLDERS AND
$	THE TOPPERS PROJECT DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES,
$	INCLUDING, BUT NOT LIMITED TO, ITS APPLICABILITY TO A PARTICULAR
$	PURPOSE. IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS AND THE
$	TOPPERS PROJECT BE LIABLE FOR ANY TYPE OF DAMAGE DIRECTLY OR
$	INDIRECTLY CAUSED FROM THE USE OF THIS SOFTWARE.
$ 
$   $Id: genoffset.tf 2046 2011-04-03 02:33:08Z ertl-hiro $
$ 
$ =====================================================================

$ 
$  generate #define directive
$ 
$FUNCTION DEFINE$
	#define $ARGV[1]$$TAB$$FORMAT("%d", +ARGV[2])$$NL$
$END$

$ 
$  bit search
$ 
$FUNCTION SEARCH_BIT$
	$_val = ARGV[1]$
	$FOREACH _val_bit RANGE(0,7)$
		$IF (_val & 1) != 0$
			$RESULT = _val_bit$
		$END$
		$_val = _val >> 1$
	$END$
$END$

$ 
$  generate the offset and bit places of bit fields
$ 
$FUNCTION DEFINE_BIT$
	$label = ARGV[1]$
	$struct_size = ARGV[2]$
	$output_size = ARGV[3]$

	$top = SYMBOL(label)$
	$IF !LENGTH(top)$
		$ERROR$$FORMAT("label %1% not found", label)$$END$
	$ELSE$
		$val = 0$
		$FOREACH i RANGE(0,struct_size-1)$
			$tmp_val = PEEK(top + i, 1)$
			$IF val == 0 && tmp_val != 0$
				$val = tmp_val$
				$offset = i$
			$END$
		$END$

		$IF val == 0$
			$ERROR$$FORMAT("bit not found in %1%", ARGV[1])$$END$
		$ELSE$
			$val_bit = SEARCH_BIT(val)$
			$RESULT = {}$
			$IF EQ(output_size, "W")$
				$IF SIL_ENDIAN_BIG$
					$val_bit = val_bit + 24 - ((offset & 0x03) << 3)$
				$ELSE$
					$val_bit = val_bit + ((offset & 0x03) << 3)$
				$END$
				$offset = offset & ~0x03$
			$ELSE$$IF EQ(output_size, "H")$
				$IF SIL_ENDIAN_BIG$
					$val_bit = val_bit + 8 - ((offset & 0x01) << 3)$
				$ELSE$
					$val_bit = val_bit + ((offset & 0x01) << 3)$
				$END$
				$offset = offset & ~0x01$
			$END$$END$
		$END$

		#define $label$$TAB$$FORMAT("%d", +offset)$$NL$
		#define $label$_bit$TAB$$FORMAT("%d", +val_bit)$$NL$
		#define $label$_mask$TAB$$FORMAT("0x%x", 1 << val_bit)$$NL$
	$END$
$END$

$ 
$  magic check
$ 
$FUNCTION MAGIC_CHECK$
	$size = ARGV[1]$
	$check = ARGV[2]$

	$label = FORMAT("MAGIC_%d", +size)$
	$top = SYMBOL(label)$
	$IF !LENGTH(top)$
		$ERROR$$FORMAT("label %1% not found", label)$$END$
	$ELSE$
		$FOREACH offset RANGE(1,size)$
			$IF SIL_ENDIAN_BIG$
				$val = PEEK(top + offset - 1, 1)$
			$ELSE$
				$val = PEEK(top + size - offset, 1)$
			$END$
			$IF val != AT(check, offset - 1)$
				$ERROR$$FORMAT("value check of %1% failed", label)$$END$
			$END$
		$END$
	$END$
$END$

$MAGIC_CHECK(1, { 0x12 })$
$MAGIC_CHECK(2, { 0x12, 0x34 })$
$MAGIC_CHECK(4, { 0x12, 0x34, 0x56, 0x78 })$

$ 
$  generate header file
$ 
$FILE "offset.h"$
/* offset.h */$NL$
$NL$
