/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2007 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *	The above copyright holders grant permission gratis to use,
 *	duplicate, modify, or redistribute (hereafter called use) this
 *	software (including the one made by modifying this software),
 *	provided that the following four conditions (1) through (4) are
 *	satisfied.
 *
 *	(1) When this software is used in the form of source code, the above
 *    	copyright notice, this use conditions, and the disclaimer shown
 *    	below must be retained in the source code without modification.
 *
 *	(2) When this software is redistributed in the forms usable for the
 *    	development of other software, such as in library form, the above
 *    	copyright notice, this use conditions, and the disclaimer shown
 *    	below must be shown without modification in the document provided
 *    	with the redistributed software, such as the user manual.
 *
 *	(3) When this software is redistributed in the forms unusable for the
 *    	development of other software, such as the case when the software
 *    	is embedded in a piece of equipment, either of the following two
 *   	 conditions must be satisfied:
 *
 *  	(a) The above copyright notice, this use conditions, and the
 *         	disclaimer shown below must be shown without modification in
 *     		the document provided with the redistributed software, such as
 *      	the user manual.
 *
 * 		(b) How the software is to be redistributed must be reported to the
 *     		TOPPERS Project according to the procedure described
 *     		separately.
 *
 *	(4) The above copyright holders and the TOPPERS Project are exempt
 *    	from responsibility for any type of damage directly or indirectly
 *   	caused from the use of this software and are indemnified by any
 *    	users or end users of this software from any and all causes of
 *    	action whatsoever.
 *
 *	THIS SOFTWARE IS PROVIDED "AS IS." THE ABOVE COPYRIGHT HOLDERS AND
 *	THE TOPPERS PROJECT DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES,
 *	INCLUDING, BUT NOT LIMITED TO, ITS APPLICABILITY TO A PARTICULAR
 *	PURPOSE. IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS AND THE
 *	TOPPERS PROJECT BE LIABLE FOR ANY TYPE OF DAMAGE DIRECTLY OR
 *	INDIRECTLY CAUSED FROM THE USE OF THIS SOFTWARE.
 * 
 *  @(#) $Id: interrupt.h 648 2007-12-15 09:35:35Z hiro $
 */

/*
 *	 interrupt management function
 */

#ifndef TOPPERS_INTERRUPT_H
#define TOPPERS_INTERRUPT_H

#ifndef OMIT_INITIALIZE_INTERRUPT

/*
 *  interrupt handler init block
 */
typedef struct interrupt_handler_initialization_block {
	INHNO		inhno;			/* interrupt handler no */
	ATR			inhatr;			/* interrupt handler attribute */
	FP			int_entry;		/* interrupt handler entry */
} INHINIB;

/*
 *  interrupt init block
 */
typedef struct interrupt_request_initialization_block {
	INTNO		intno;			/* interrupt no */
	ATR			intatr;			/* interrupt attribute */
	PRI			intpri;			/* interrupt priority */
} INTINIB;

/*
 * total number of interrupt handler (see kernel_cfg.c)
 */
extern const uint_t	tnum_inhno;

/*
 *  interrupt handler init block array (see kernel_cfg.c)
 */
extern const INHINIB	inhinib_table[];

/*
 * total number of interrupt handler (see kernel_cfg.c)
 */
extern const uint_t	tnum_intno;

/*
 * interrupt init block array (see kernel_cfg.c)
 */
extern const INTINIB	intinib_table[];

#endif /* OMIT_INITIALIZE_INTERRUPT */

/*
 *  interrupt management initialization
 */
extern void	initialize_interrupt(void);

#endif /* TOPPERS_INTERRUPT_H */
