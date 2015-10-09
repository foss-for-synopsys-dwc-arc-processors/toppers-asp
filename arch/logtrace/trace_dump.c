/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2007,2008 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
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
 *  @(#) $Id: trace_dump.c 1230 2008-08-08 05:52:17Z ertl-hiro $
 */

/*
 *	 dump all trace logs
 */

#include "kernel_impl.h"
#include "task.h"
#include <log_output.h>

/* 
 *  get information about kernel
 */
static intptr_t
get_tskid(intptr_t info)
{
	TCB		*p_tcb;
	ID		tskid;

	p_tcb = (TCB *) info;
	if (p_tcb == NULL) {
		tskid = 0;
	}
	else {
		tskid = TSKID(p_tcb);
	}
	return((intptr_t) tskid);
}

static intptr_t
get_tskstat(intptr_t info)
{
	uint_t		tstat;
	const char	*tstatstr;

	tstat = (uint_t) info;
	switch (tstat & (TS_RUNNABLE|TS_WAITING|TS_SUSPENDED)) {
	case TS_DORMANT:
		tstatstr = "DORMANT";
		break;
	case TS_RUNNABLE:
		tstatstr = "RUNNABLE";
		break;
	case TS_WAITING:
		tstatstr = "WAITING";
		break;
	case TS_SUSPENDED:
		tstatstr = "SUSPENDED";
		break;
	case TS_WAITING|TS_SUSPENDED:
		tstatstr = "WAITING-SUSPENDED";
		break;
	default:
		tstatstr = "unknown state";
		break;
	}
	return((intptr_t) tstatstr);
}

/* 
 *  print trace logs
 */
static void
trace_print(TRACE *p_trace, void (*putc)(char_t))
{
	intptr_t	traceinfo[TMAX_LOGINFO + 1];
	const char	*tracemsg;
	int_t		i;

	traceinfo[0] = (intptr_t)(p_trace->logtim);
	syslog_printf("[%d] ", traceinfo, putc);

	switch (p_trace->logtype) {
	case LOG_TYPE_TSKSTAT:
		traceinfo[0] = get_tskid(p_trace->loginfo[0]);
		traceinfo[1] = get_tskstat(p_trace->loginfo[1]);
		tracemsg = "task %d becomes %s.";
		break;
	case LOG_TYPE_DSP|LOG_LEAVE:
		traceinfo[0] = get_tskid(p_trace->loginfo[0]);
		tracemsg = "dispatch to task %d.";
		break;
	case LOG_TYPE_COMMENT:
		for (i = 1; i < TMAX_LOGINFO; i++) {
			traceinfo[i-1] = p_trace->loginfo[i];
		}
		tracemsg = (const char *)(p_trace->loginfo[0]);
		break;
	case LOG_TYPE_ASSERT:
		traceinfo[0] = p_trace->loginfo[0];
		traceinfo[1] = p_trace->loginfo[1];
		traceinfo[2] = p_trace->loginfo[2];
		tracemsg = "%s:%u: Assertion `%s' failed.";
		break;
	default:
		traceinfo[0] = p_trace->logtype;
		tracemsg = "unknown trace log type: %d.";
		break;
	}
	syslog_printf(tracemsg, traceinfo, putc);
	(*putc)('\n');
}

/* 
 *  dump all trace logs
 */
void
trace_dump(intptr_t exinf)
{
	TRACE	trace;
	void	(*putc)(char_t);

	putc = (void (*)(char_t)) exinf;
	while (trace_rea_log(&trace) >= 0) {
		trace_print(&trace, putc);
	}
}
