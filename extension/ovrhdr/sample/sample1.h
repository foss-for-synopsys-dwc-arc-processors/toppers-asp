/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: sample1.h 1717 2010-01-31 11:41:03Z ertl-hiro $
 */

/*
 *		the head file of sample program (1)
 */

/*
 *  target-dependent definitions
 */
#include "target_test.h"

/*
 *  the definitions of task priority
 */

#define MAIN_PRIORITY	5		/* main task priority */
								/* higher than HIGH_PRIORITY */

#define HIGH_PRIORITY	9		/* target tasks priority */
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

/*
 *   the definitions of constants that may be target-dependent
 */

#ifndef TASK_PORTID
#define	TASK_PORTID		1			/* task port ID */
#endif /* TASK_PORTID */

#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* task stack size */
#endif /* STACK_SIZE */

#ifndef LOOP_REF
#define LOOP_REF		ULONG_C(1000000)	/* loop reference */
#endif /* LOOP_REF */

/*
 *   the declarations of functions
 */
#ifndef TOPPERS_MACRO_ONLY

extern void	task(intptr_t exinf);
extern void	main_task(intptr_t exinf);
extern void	tex_routine(TEXPTN texptn, intptr_t exinf);
#ifdef CPUEXC1
extern void	cpuexc_handler(void *p_excinf);
#endif /* CPUEXC1 */
extern void	cyclic_handler(intptr_t exinf);
extern void	alarm_handler(intptr_t exinf);
#ifdef TOPPERS_SUPPORT_OVRHDR
extern void	overrun_handler(ID tskid, intptr_t exinf);
#endif /* TOPPERS_SUPPORT_OVRHDR */

#endif /* TOPPERS_MACRO_ONLY */
