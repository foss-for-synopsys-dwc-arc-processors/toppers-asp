/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2006-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: perf4.c 1575 2009-05-31 14:15:46Z ertl-hiro $
 */

/*
 *		kernel performance evaluation (4)
 *
 * 	measure the time of act_tsk and iact_tsk and corresponding task
 * 	dispatch time. The following 3 time are measured
 *
 *  (1) the time of act_tsk that won't cause task dispatch. The running
 *  task will call act_tsk to activate the task with lower priority
 *  which will get into the runnable state for dormant state.
 *
 *  (2) the time of act_tsk that will cause task dispatch. The running
 *  task will call act_tsk to activate the task with higher priority
 *  which will get into the runnable state for dormant state, then a
 *  task dispatch will happen.
 *
 *  (3) the time of iact_tsk that will cause task dispatch. The
 *  interrupt handler will call iact_tsk to activate the task with
 *  higher priority which will get into the runnable state for dormant
 *  state. After the return of interrupt handler, a task dispatch will
 *  happen.
 *
 */

#include <kernel.h>
#include <t_syslog.h>
#include <test_lib.h>
#include <histogram.h>
#include "kernel_cfg.h"
#include "perf4.h"

/*
 *  the loops of measurement and possible max time 
 */
#define NO_MEASURE	10000U			/* loops of measurement  */
#define MAX_TIME	1000U			/* possible max time */

/*
 *  the histogram area
 */
static uint_t	histarea1[MAX_TIME + 1];
static uint_t	histarea2[MAX_TIME + 1];
static uint_t	histarea3[MAX_TIME + 1];

/*
 * task1 (high priority)
 */
void task1(intptr_t exinf)
{
	end_measure(2);
	ext_tsk();
}

/*
 * the count shared between task2 and main task
 */
volatile uint_t		task2_count;

/*
 *  task2 (high priority)
 */
void task2(intptr_t exinf)
{
	end_measure(3);
	task2_count++;
	ext_tsk();
}

/*
 *  task3 (low priority)
 */
void task3(intptr_t exinf)
{
	ext_tsk();
}

/*
 * task4 (the lowest priority)
 */
void task4(intptr_t exinf)
{
	while (true) {
		wup_tsk(MAIN_TASK);
	}
}

/*
 *  interrupt handler
 */
void cyclic_handler(intptr_t exinf)
{
	begin_measure(3);
	iact_tsk(TASK2);
}

/*
 *  main task (middle priority)
 */
void main_task(intptr_t exinf)
{
	uint_t	i;

	syslog_0(LOG_NOTICE, "Performance evaluation program (4)");
	init_hist(1, MAX_TIME, histarea1);
	init_hist(2, MAX_TIME, histarea2);
	init_hist(3, MAX_TIME, histarea3);
	syslog_flush();

	/*
	 *   measure the time of act_tsk that won't cause task dispatch
	 */
	for (i = 0; i < NO_MEASURE; i++) {
		begin_measure(1);
		act_tsk(TASK3);
		end_measure(1);
		slp_tsk();
	}

	/*
	 *   measure the time of act_tsk that will cause task dispatch
	 */
	for (i = 0; i < NO_MEASURE; i++) {
		begin_measure(2);
		act_tsk(TASK1);
	}

	/*
	 *  measure the time of iact_tsk that will cause task dispatch 
	 *  (the loops are 1/10 of NO_MEASURE)
	 */
	task2_count = 0;
	sta_cyc(CYC1);
	while (task2_count < NO_MEASURE / 10) ;
	stp_cyc(CYC1);

	syslog_0(LOG_NOTICE, "Execution times of act_tsk without task switch");
	print_hist(1);
	syslog_0(LOG_NOTICE, "Execution times of act_tsk with task switch");
	print_hist(2);
	syslog_0(LOG_NOTICE, "Execution times of iact_tsk with task switch");
	print_hist(3);
	test_finish();
}
