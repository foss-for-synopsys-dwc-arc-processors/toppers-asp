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
 *  @(#) $Id: perf2.c 1575 2009-05-31 14:15:46Z ertl-hiro $
 */

/*
 *		kernel performance evaluation (2)
 *
 * 	measure the time of snd_pdq and its change when there are already
 * 	some items in the priority dataqueue (the number of items will
 * 	change)
 */

#include <kernel.h>
#include <t_syslog.h>
#include <test_lib.h>
#include <histogram.h>
#include "kernel_cfg.h"
#include "perf2.h"

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
 *  performance evaluation routine
 */
void
perf_eval(uint_t n)
{
	uint_t		i;
	intptr_t	data;
	PRI			pri;

	ini_pdq(PDQ1);
	init_hist(1, MAX_TIME, histarea1);

	for (i = 0; i < n; i++) {
		data = i;
		snd_pdq(PDQ1, data, 1);
	}

	for (i = 0; i < NO_MEASURE; i++) {
		data = i;
		begin_measure(1);
		snd_pdq(PDQ1, data, 2);
		end_measure(1);
		rcv_pdq(PDQ1, &data, &pri);
	}

	syslog_1(LOG_NOTICE, "Execution times of snd_pdq"
								" when %d data are queued.", n);
	print_hist(1);
	syslog_flush();
}

/*
 *  main task (low priority)
 */
void main_task(intptr_t exinf)
{
	syslog_0(LOG_NOTICE, "Performance evaluation program (2)");
	syslog_flush();

	perf_eval(0);
	perf_eval(10);
	perf_eval(20);
	perf_eval(30);
	perf_eval(40);
	perf_eval(50);
	perf_eval(100);
	perf_eval(200);
	perf_eval(300);
	test_finish();
}
