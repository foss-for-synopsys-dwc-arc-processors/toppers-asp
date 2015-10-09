/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2007-2010 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: test_dlynse.c 1803 2010-06-20 08:55:06Z ertl-hiro $
 */

/*
 *		sil_dly_nse test
 */

#include <kernel.h>
#include <sil.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_dlynse.h"

/*
 *  to refer SIL_DLY_TIM1 and SIL_DLY_TIM2, include the header file for
 *  kernel implementation.
 */
#include "kernel/kernel_impl.h"

#define	NO_LOOP		ULONG_C(1000000)

SYSTIM	empty_time;

static void
test_empty(void)
{
	SYSTIM	stime, etime;
	volatile ulong_t	i;

	get_tim(&stime);
	for (i = 0; i < NO_LOOP; i++) {
	}
	get_tim(&etime);
	empty_time = etime - stime;
	syslog(LOG_NOTICE, "empty loop: %u", empty_time);
	syslog_flush();
}

static void
test_dly_nse(ulong_t dlytim)
{
	SYSTIM	stime, etime, delay_time;
	volatile ulong_t	i;

	get_tim(&stime);
	for (i = 0; i < NO_LOOP; i++) {
		sil_dly_nse(dlytim);
	}
	get_tim(&etime);
	delay_time = (etime - stime) - empty_time;
	syslog(LOG_NOTICE, "sil_dly_nse(%u): %u %s", (uint_t)(dlytim),
				(uint_t)(delay_time), delay_time > dlytim ? "OK" : "NG");
	syslog_flush();
}

void
main_task(intptr_t exinf)
{
	test_empty();

	syslog(LOG_NOTICE, "-- for fitting parameters --");
	test_dly_nse(0);
	test_dly_nse(SIL_DLY_TIM1);
	test_dly_nse(SIL_DLY_TIM1 + SIL_DLY_TIM2 * 1);
	test_dly_nse(SIL_DLY_TIM1 + SIL_DLY_TIM2 * 2);
	test_dly_nse(SIL_DLY_TIM1 + SIL_DLY_TIM2 * 3);
	test_dly_nse(SIL_DLY_TIM1 + SIL_DLY_TIM2 * 4);
	test_dly_nse(SIL_DLY_TIM1 + SIL_DLY_TIM2 * 5);
	test_dly_nse(SIL_DLY_TIM1 + SIL_DLY_TIM2 * 10);
	test_dly_nse(SIL_DLY_TIM1 + SIL_DLY_TIM2 * 20);
	test_dly_nse(SIL_DLY_TIM1 + SIL_DLY_TIM2 * 50);

	syslog(LOG_NOTICE, "-- for checking boundary conditions --");
	test_dly_nse(SIL_DLY_TIM1 + 1);
	test_dly_nse(SIL_DLY_TIM1 + SIL_DLY_TIM2 * 1 + 1);
	test_dly_nse(SIL_DLY_TIM1 + SIL_DLY_TIM2 * 2 + 1);

	test_finish();
}
