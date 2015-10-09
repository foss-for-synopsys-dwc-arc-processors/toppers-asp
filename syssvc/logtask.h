/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2008 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: logtask.h 774 2008-03-08 06:35:57Z hiro $
 */

/*
 *		system log task
 */

#ifndef TOPPERS_LOGTASK_H
#define TOPPERS_LOGTASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "target_syssvc.h"

/*
 * default definitions for system log task
 */
#ifndef LOGTASK_PRIORITY
#define LOGTASK_PRIORITY	3		/* initial task priority */
#endif /* LOGTASK_PRIORITY */

#ifndef LOGTASK_STACK_SIZE
#define LOGTASK_STACK_SIZE	1024	/* the size of task stack for system log task */
#endif /* LOGTASK_STACK_SIZE */

#ifndef LOGTASK_PORTID
#define LOGTASK_PORTID		1		/* serial port id for system log task */
#endif /* LOGTASK_PORTID */

#ifndef LOGTASK_INTERVAL
#define LOGTASK_INTERVAL	10U		/* output interval of system log task (ms) */
#endif /* LOGTASK_INTERVAL */

#ifndef LOGTASK_FLUSH_WAIT
#define LOGTASK_FLUSH_WAIT	1U		/* wait time for flushing the system logs (ms) */
#endif /* LOGTASK_FLUSH_WAIT */


/*
 *  wait for flushing the system logs.
 *
 *  the task that calls this function will wait for 
 *  the logtask to output the system logs untill the number of system
 *  logs is less than count
 *
 *  when count is zero, the calling task will wait untill the output
 *  buffer of system log port is empty 
 */
extern ER	logtask_flush(uint_t count) throw();

/*
 *  the task body of log task
 */
extern void	logtask_main(intptr_t exinf) throw();

/*
 *  terminate routine of system log task
 *
 *  this function will be called when the kernel is terminated
 */
extern void	logtask_terminate(intptr_t exinf) throw();

#ifdef __cplusplus
}
#endif

#endif /* TOPPERS_LOGTASK_H */
