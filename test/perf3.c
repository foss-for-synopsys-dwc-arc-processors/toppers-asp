/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2007-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: perf3.c 1575 2009-05-31 14:15:46Z ertl-hiro $
 */

/*
 *	kernel performance evaluation (3)
 *
 * 	measure the time of set_flg and its change when there are already
 * 	some tasks in the waiting queue of the event flag. (The number of
 * 	task will change)
 *
 */

#include <kernel.h>
#include <t_syslog.h>
#include <test_lib.h>
#include <histogram.h>
#include "kernel_cfg.h"
#include "perf3.h"

/*
 *  the loops of measurement and possible max time 
 */
#define NO_MEASURE	10000U			/* loops of measurement  */
#define MAX_TIME	1000U			/* possible max time */

/*
 *  the histogram area
 */
static uint_t	histarea1[MAX_TIME + 1];

/*
 *  the task list for measurement
 */
static ID task_list[20] = {
	TASK1, TASK2, TASK3, TASK4, TASK5,
	TASK6, TASK7, TASK8, TASK9, TASK10,
	TASK11, TASK12, TASK13, TASK14, TASK15,
	TASK16, TASK17, TASK18, TASK19, TASK20
};

/*
 *  task for measurement (middle priority)
 */
void task(intptr_t exinf)
{
	FLGPTN	flgptn;

	wai_flg(FLG1, 0x01U, TWF_ORW, &flgptn);
}

/*
 *  performance evaluation routine
 */
void
perf_eval(uint_t n)
{
	uint_t		i, j;

	init_hist(1, MAX_TIME, histarea1);

	for (i = 0; i < NO_MEASURE; i++) {
		ini_flg(FLG1);
		for (j = 0; j < n; j++) {
			act_tsk(task_list[j]);
		}
		chg_pri(TSK_SELF, MAIN_PRIORITY_LOW);
		/* let the task in the waiting queue of the event flag */
		chg_pri(TSK_SELF, TPRI_INI);

		begin_measure(1);
		set_flg(FLG1, 0x01U);
		end_measure(1);

		chg_pri(TSK_SELF, MAIN_PRIORITY_LOW);
		/* wait the task exits */
		chg_pri(TSK_SELF, TPRI_INI);
	}

	syslog_1(LOG_NOTICE, "Execution times of set_flg"
							" when %d tasks are released from waiting.", n);
	print_hist(1);
	syslog_flush();
}

/*
 *  main task (high priority)
 */
void main_task(intptr_t exinf)
{
	syslog_0(LOG_NOTICE, "Performance evaluation program (3)");
	syslog_flush();

	perf_eval(0);
	perf_eval(1);
	perf_eval(2);
	perf_eval(3);
	perf_eval(4);
	perf_eval(5);
	perf_eval(10);
	perf_eval(20);
	test_finish();
}
