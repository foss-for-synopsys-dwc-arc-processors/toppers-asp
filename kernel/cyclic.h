/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005,2006 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: cyclic.h 748 2008-03-07 17:18:06Z hiro $
 */

/*
 *	cyclic handler function
 */

#ifndef TOPPERS_CYCLIC_H
#define TOPPERS_CYCLIC_H

#include "time_event.h"

/*
 *  cyclic handler init block
 *
 *  sta_cyc   1st run         2nd run
 *    +--------*-----------------*----------
 *       phase       period
 */
typedef struct cyclic_handler_initialization_block {
	ATR			cycatr;			/* cyclic handler attribute */
	intptr_t	exinf;			/* cyclic handler parameter */
	CYCHDR		cychdr;			/* cyclic handler address */
	RELTIM		cyctim;			/* cyclic handler period  */
	RELTIM		cycphs;			/* cyclic handler phase	  */
} CYCINIB;

/*
 *  cyclic handler control block
 */
typedef struct cyclic_handler_control_block {
	const CYCINIB *p_cycinib;	/* pointer to init block */
	bool_t		cycsta;			/* cyclic handler status */
	EVTTIM		evttim;			/* next run time */
	TMEVTB		tmevtb;			/* time event block */
} CYCCB;

/*
 *  max value of cyclic handler id (see kernel_cfg.c)
 */
extern const ID	tmax_cycid;

/*
 *  cyclic handler init block array (see kernel_cfg.c)
 */
extern const CYCINIB	cycinib_table[];

/*
 *  cyclic handler control block array (see kernel_cfg.c)
 */
extern CYCCB	cyccb_table[];

/*
 *  cyclic handler initialization
 */
extern void	initialize_cyclic(void);

/*
 *  cyclic handler callback routine
 */
extern void	call_cychdr(CYCCB *p_cyccb);

#endif /* TOPPERS_CYCLIC_H */
