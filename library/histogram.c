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
 *  @(#) $Id: histogram.c 1574 2009-05-31 13:57:37Z ertl-hiro $
 */

/*
 *		execution time histogram module
 */

#include <kernel.h>
#include <t_syslog.h>
#include <test_lib.h>
#include <histogram.h>
#include "target_test.h"

/*
 *  the number of execution time histogram
 */
#ifndef TNUM_HIST
#define TNUM_HIST		10
#endif /* TNUM_HIST */

/*
 *  macro which can be changed in target-dependent part
 */
#ifndef HISTTIM						/* the data type of time for measuring execution time */
#define HISTTIM			SYSUTM
#endif /* HISTTIM */

#ifndef HIST_GET_TIM				/* get the current time */
#ifndef TOPPERS_SUPPORT_GET_UTM
#error get_utm is not supported.
#endif /* TOPPERS_SUPPORT_GET_UTM */
#define HIST_GET_TIM(p_time)	((void) get_utm(p_time))
#endif /* HIST_GET_TIM */

#ifndef HIST_CONV_TIM				/* convert to the execution time from the time difference */
#define HIST_CONV_TIM(time)		((uint_t)(time))
#endif /* HIST_CONV_TIM */

#ifndef HIST_BM_HOOK				/* hook before the execution time measuring */
#define HIST_BM_HOOK()			((void) 0)
#endif

/*
 *  control block of execution time histogram
 */
typedef struct histogram_control_block {
	HISTTIM		begin_time;			/* start time of measurement*/
	uint_t		maxval;				/* the maximum execution time */
	uint_t		*histarea;			/* memory space of histogram */
	uint_t		over;				/* the times of exceeding the maximum execution time */
	uint_t		under;				/* the times of possible time reversion */
} HISTCB;

/*
 * memory area of control block of execution time histogram
 */
HISTCB	histcb_table[TNUM_HIST];

/*
 *  the max and min id of execution time histograms
 */
#define TMIN_HISTID		1
#define TMAX_HISTID		(TMIN_HISTID + TNUM_HIST - 1)

/*
 *  initialize the execution time histograms
 */
void
init_hist(ID histid, uint_t maxval, uint_t histarea[])
{
	HISTCB	*p_histcb;
	uint_t	i;

	assert(TMIN_HISTID <= histid && histid <= TMAX_HISTID);
	p_histcb = &(histcb_table[histid - TMIN_HISTID]);

	for (i = 0; i <= maxval; i++) {
		histarea[i] = 0U;
	}
	p_histcb->maxval = maxval;
	p_histcb->histarea = histarea;
	p_histcb->over = 0U;
	p_histcb->under = 0U;
}

/*
 *  start the execution time measurement
 */
void
begin_measure(ID histid)
{
	HISTCB	*p_histcb;

	assert(TMIN_HISTID <= histid && histid <= TMAX_HISTID);
	p_histcb = &(histcb_table[histid - TMIN_HISTID]);

	HIST_BM_HOOK();
	HIST_GET_TIM(&(p_histcb->begin_time));
}

/*
 *  end the execution time measurement
 */
void
end_measure(ID histid)
{
	HISTCB	*p_histcb;
	HISTTIM	end_time;
	uint_t	val;

	HIST_GET_TIM(&end_time);

	assert(TMIN_HISTID <= histid && histid <= TMAX_HISTID);
	p_histcb = &(histcb_table[histid - TMIN_HISTID]);

	val = HIST_CONV_TIM(end_time - p_histcb->begin_time);
	if (val <= p_histcb->maxval) {
		p_histcb->histarea[val]++;
	}
	else if (val <= ((uint_t) INT_MAX)) {
		p_histcb->over++;
	}
	else {
		p_histcb->under++;
	}
}

/*
 *  print execution time histogram
 */
void
print_hist(ID histid)
{
	HISTCB	*p_histcb;
	uint_t	i;

	assert(TMIN_HISTID <= histid && histid <= TMAX_HISTID);
	p_histcb = &(histcb_table[histid - TMIN_HISTID]);

	for (i = 0; i <= p_histcb->maxval; i++) {
		if (p_histcb->histarea[i] > 0) {
			syslog_2(LOG_NOTICE, "%d : %d", i, p_histcb->histarea[i]);
			syslog_flush();
		}
	}
	if (p_histcb->over > 0) {
		syslog_2(LOG_NOTICE, "> %d : %d", p_histcb->maxval, p_histcb->over);
	}
	if (p_histcb->under > 0) {
		syslog_1(LOG_NOTICE, "> INT_MAX : %d", p_histcb->under);
	}
	syslog_flush();
}
