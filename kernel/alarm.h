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
 *  @(#) $Id: alarm.h 748 2008-03-07 17:18:06Z hiro $
 */

/*
 *	alarm handler function
 */

#ifndef TOPPERS_ALARM_H
#define TOPPERS_ALARM_H

#include "time_event.h"

/*
 *  alarm handler init block
 */
typedef struct alarm_handler_init_block {
	ATR			almatr;			/* alarm handler attribute */
	intptr_t	exinf;			/* alarm handler parameter */
	ALMHDR		almhdr;			/* alarm handler address */
} ALMINIB;

/*
 *  alarm handler control block
 */
typedef struct alarm_handler_control_block {
	const ALMINIB *p_alminib;	/* pointer to init block */
	bool_t		almsta;			/* alarm handler status	 */
	TMEVTB		tmevtb;			/* time event block		 */
} ALMCB;

/*
 *  max value of alarm handler id (see kernel_cfg.c)
 */
extern const ID	tmax_almid;

/*
 *  alarm handler init block array (see kernel_cfg.c)
 */
extern const ALMINIB	alminib_table[];

/*
 *  alarm handler control block array (see kernel_cfg.c)
 */
extern ALMCB	almcb_table[];

/*
 *  alarm handler initialization
 */
extern void	initialize_alarm(void);

/*
 *  alarm handler callback routine
 */
extern void	call_almhdr(ALMCB *p_almcb);

#endif /* TOPPERS_ALARM_H */
