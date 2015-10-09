/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2010 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: syslog.c 1788 2010-05-16 21:06:02Z ertl-hiro $
 */

/*
 *		system log module
 */

#include <sil.h>
#undef TOPPERS_OMIT_SYSLOG
#include <t_syslog.h>
#include <log_output.h>
#include "target_syssvc.h"
#include "syslog.h"

/*
 *  default definition of trace macros
 */
#ifndef LOG_SYSLOG_WRI_LOG_ENTER
#define LOG_SYSLOG_WRI_LOG_ENTER(prio, p_syslog)
#endif /* LOG_SYSLOG_WRI_LOG_ENTER */

#ifndef LOG_SYSLOG_WRI_LOG_LEAVE
#define LOG_SYSLOG_WRI_LOG_LEAVE(ercd)
#endif /* LOG_SYSLOG_WRI_LOG_LEAVE */

#ifndef LOG_SYSLOG_REA_LOG_ENTER
#define LOG_SYSLOG_REA_LOG_ENTER(p_syslog)
#endif /* LOG_SYSLOG_REA_LOG_ENTER */

#ifndef LOG_SYSLOG_REA_LOG_LEAVE
#define LOG_SYSLOG_REA_LOG_LEAVE(ercd, p_syslog)
#endif /* LOG_SYSLOG_REA_LOG_LEAVE */

#ifndef LOG_SYSLOG_MSK_LOG_ENTER
#define LOG_SYSLOG_MSK_LOG_ENTER(logmask, lowmask)
#endif /* LOG_SYSLOG_MSK_LOG_ENTER */

#ifndef LOG_SYSLOG_MSK_LOG_LEAVE
#define LOG_SYSLOG_MSK_LOG_LEAVE(ercd)
#endif /* LOG_SYSLOG_MSK_LOG_LEAVE */

#ifndef LOG_SYSLOG_REF_LOG_ENTER
#define LOG_SYSLOG_REF_LOG_ENTER(pk_rlog)
#endif /* LOG_SYSLOG_REF_LOG_ENTER */

#ifndef LOG_SYSLOG_REF_LOG_LEAVE
#define LOG_SYSLOG_REF_LOG_LEAVE(pk_rlog)
#endif /* LOG_SYSLOG_REF_LOG_LEAVE */

/*
 *  current system time
 *
 *  refer internal kernel variable to record the time when system log
 *  is logged.
 */
extern ulong_t	_kernel_current_time;

/*
 *  system log buffer and related access pointers 
 */
static SYSLOG	syslog_buffer[TCNT_SYSLOG_BUFFER];	/* log buffer */
static uint_t	syslog_count;			/* the number of logs in the log buffer */
static uint_t	syslog_head;			/* head position in log buffer */
static uint_t	syslog_tail;			/* tail position in log buffer */
static uint_t	syslog_lost;			/* the number of lost logs */

/*
 * the priority of system log (bitmap)
 */
static uint_t	syslog_logmask;			/* system log priority mask in log buffer */
static uint_t	syslog_lowmask_not;		/* system log priority mask in low-level output */

/*
 *  system log module initialize routine
 */
void
syslog_initialize(intptr_t exinf)
{
	syslog_count = 0U;
	syslog_head = syslog_tail = 0U;
	syslog_lost = 0U;

	syslog_logmask = LOG_UPTO(LOG_NOTICE);
	syslog_lowmask_not = ~0U;
}

/*
 *  output system log
 *
 *  this function is implemented to work regardless of the cpu lock state or context
 */
ER
syslog_wri_log(uint_t prio, const SYSLOG *p_syslog)
{
	SIL_PRE_LOC;

	LOG_SYSLOG_WRI_LOG_ENTER(prio, p_syslog);
	SIL_LOC_INT();

	/*
	 *  record the log time
	 */
	((SYSLOG *) p_syslog)->logtim = _kernel_current_time;

	/*
	 *  write to log buffer
	 */
	if ((syslog_logmask & LOG_MASK(prio)) != 0U) {
		syslog_buffer[syslog_tail] = *p_syslog;
		syslog_tail++;
		if (syslog_tail >= TCNT_SYSLOG_BUFFER) {
			syslog_tail = 0U;
		}
		if (syslog_count < TCNT_SYSLOG_BUFFER) {
			syslog_count++;
		}
		else {
			syslog_head = syslog_tail;
			syslog_lost++;
		}
	}

	/*
	 *  low-level output
	 */
	if (((~syslog_lowmask_not) & LOG_MASK(prio)) != 0U) {
		syslog_print(p_syslog, target_fput_log);
		(*target_fput_log)('\n');
	}

	SIL_UNL_INT();
	LOG_SYSLOG_WRI_LOG_LEAVE(E_OK);
	return(E_OK);
}

/*
 *  read system log from log buffer
 *  this function is implemented to work regardless of the cpu lock state or context.
 */
ER_UINT
syslog_rea_log(SYSLOG *p_syslog)
{
	ER_UINT	ercd;
	SIL_PRE_LOC;

	LOG_SYSLOG_REA_LOG_ENTER(p_syslog);
	SIL_LOC_INT();

	/*
	 *  read from log buffer
	 */
	if (syslog_count > 0U) {
		*p_syslog = syslog_buffer[syslog_head];
		syslog_count--;
		syslog_head++;
		if (syslog_head >= TCNT_SYSLOG_BUFFER) {
			syslog_head = 0U;
		}
		ercd = (ER_UINT) syslog_lost;
		syslog_lost = 0U;
	}
	else {
		ercd = E_OBJ;
	}

	SIL_UNL_INT();
	LOG_SYSLOG_REA_LOG_LEAVE(ercd, p_syslog);
	return(ercd);
}

/*
 *  set the current priority of system log 
 */
ER
syslog_msk_log(uint_t logmask, uint_t lowmask)
{
	LOG_SYSLOG_MSK_LOG_ENTER(logmask, lowmask);
	syslog_logmask = logmask;
	syslog_lowmask_not = ~lowmask;
	LOG_SYSLOG_MSK_LOG_LEAVE(E_OK);
	return(E_OK);
}

/*
 *  refer the status of log buffer
 */
ER_UINT
syslog_ref_log(T_SYSLOG_RLOG *pk_rlog)
{
	SIL_PRE_LOC;

	LOG_SYSLOG_REF_LOG_ENTER(pk_rlog);
	SIL_LOC_INT();

	pk_rlog->count = syslog_count;
	pk_rlog->lost = syslog_lost;
	pk_rlog->logmask = syslog_logmask;
	pk_rlog->lowmask = ~syslog_lowmask_not;

	SIL_UNL_INT();
	LOG_SYSLOG_REF_LOG_LEAVE(pk_rlog);
	return(E_OK);
}
