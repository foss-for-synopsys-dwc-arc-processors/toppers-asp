/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: overrun.h 1717 2010-01-31 11:41:03Z ertl-hiro $
 */

/*
 *		overrun handler funtion
 */

#ifndef TOPPERS_OVERRUN_H
#define TOPPERS_OVERRUN_H

#ifdef TOPPERS_SUPPORT_OVRHDR
#include "target_timer.h"

/*
 *  overrun handler initization block
 */
typedef struct overrun_handler_initialization_block {
	ATR			ovratr;			/* overrun handler attribte */
	OVRHDR		ovrhdr;			/* overrun handler start address */
} OVRINIB;

/*
 *  overrun handler block area  (kernel_cfg.c)
 */
extern const OVRINIB	ovrinib;

/*
 *  the flag to show whether overrun timer is running
 */
extern bool_t	ovrtimer_flag;

/*
 *  overrun handler initialization function
 */
extern void	initialize_overrun(void);

/*
 *  start the overrun timer
 */
extern void	ovrtimer_start(void);

/*
 *  stop the overrun timer
 */
#ifndef OMIT_OVRTIMER_STOP

extern void	ovrtimer_stop(void);

#else /* OMIT_OVRTIMER_STOP */

Inline void
ovrtimer_stop(void)
{
	if (p_runtsk->leftotm > 0U) {
		p_runtsk->leftotm = target_ovrtimer_stop();
		ovrtimer_flag = false;
	}
}

#endif /* OMIT_OVRTIMER_STOP */

/*
 * call overrun handler
 */
extern void	call_ovrhdr(void);

#endif /* TOPPERS_SUPPORT_OVRHDR */
#endif /* TOPPERS_OVERRUN_H */
