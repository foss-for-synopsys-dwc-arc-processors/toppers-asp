/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2007 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: trace_config.h 1230 2008-08-08 05:52:17Z ertl-hiro $
 */

/*
 *	trace log related configuration 
 *
 *  this headers file is only included in target_config.h and
 *  target_syssvc.h. Other files should not directly include
 *  this file
 */

#ifndef TOPPERS_TRACE_CONFIG_H
#define TOPPERS_TRACE_CONFIG_H

/*
 *  size of trace log buffer
 */
#ifndef TCNT_TRACE_BUFFER
#define TCNT_TRACE_BUFFER	1024
#endif /* TCNT_TRACE_BUFFER */

/*
 * 	how to get log time
 */
#ifndef TRACE_GET_TIM
#define TRACE_GET_TIM()		(current_time)
#endif /* TRACE_GET_TIM */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  data structure of trace log 
 *
 *  the same as sys log
 */
#include <t_syslog.h>
typedef	SYSLOG	TRACE;

/*
 *  trace log buffer and corresponding pointers
 */
extern TRACE	trace_buffer[];		/* trace log buffer  */
extern uint_t	trace_count;		/* the log count in the buffer  */
extern uint_t	trace_head;			/* the head log in the buffer  */
extern uint_t	trace_tail;			/* the tail log in the buffer */
extern uint_t	trace_lost;			/* the lost trace logs */

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  trace mode
 */
#define TRACE_STOP			UINT_C(0x00)	/* trace stop */
#define TRACE_RINGBUF		UINT_C(0x01)	/* ring buffer mode  */
#define TRACE_AUTOSTOP		UINT_C(0x02)	/* auto stop mode  */
#define TRACE_CLEAR			UINT_C(0x04)	/* clear trace log*/

#ifndef TOPPERS_MACRO_ONLY

/*
 *  initialize trace log function
 *
 *  this function is registered as the initialize routine. The following
 *  parameters decide corresponding operations
 *
 *  TRACE_STOP: initialize only, do not start trace.
 *  TRACE_RINGBUF: start trace in ring buffer mode.
 *  TRACE_AUTOSTOP: start trace in auto stop mode.
 */
extern void	trace_initialize(intptr_t exinf);

/*
 *  start trace
 *
 *  start/stop trace log. The following
 *  parameters decide corresponding operations
 *
 *  TRACE_STOP¡§stop trace¡¥
 *  TRACE_RINGBUF: start trace in ring buffer mode.
 *  TRACE_AUTOSTOP: start trace in ring buffer mode.
 *  TRACE_CLEAR; clear trace logs.
 */
extern ER	trace_sta_log(MODE mode);

/*
 *  write trace log
 */
extern ER	trace_wri_log(TRACE *p_trace);

/*
 *  read trace log
 */
extern ER	trace_rea_log(TRACE *p_trace);

/* 
 *  dump trace logs (trace_dump.c)
 *
 *  dump all trace logs. this functions is registered as termination
 *  routine. the parameter includes dump destination and corresponding
 *  output method. If target dependent low level output is used, the
 *  parameter is target_putc.
 */
extern void	trace_dump(intptr_t exinf);

/*
 *  library functions for trace log write
 */
extern void	trace_write_0(uint_t type);
extern void	trace_write_1(uint_t type, intptr_t arg1);
extern void	trace_write_2(uint_t type, intptr_t arg1, intptr_t arg2);
extern void	trace_write_3(uint_t type, intptr_t arg1, intptr_t arg2,
														intptr_t arg3);

/*
 *  macros for trace log write
 */

#define trace_0(type) \
				trace_write_0(type)

#define trace_1(type, arg1) \
				trace_write_1(type, (intptr_t)(arg1))

#define trace_2(type, arg1, arg2) \
				trace_write_2(type, (intptr_t)(arg1), (intptr_t)(arg2))

#define trace_3(type, arg1, arg2, arg3) \
				trace_write_3(type, (intptr_t)(arg1), (intptr_t)(arg2), \
						(intptr_t)(arg3))

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  corresponding trace log configurations
 */
#define LOG_TSKSTAT(p_tcb)		trace_2(LOG_TYPE_TSKSTAT, p_tcb, p_tcb->tstat)

#define LOG_DSP_LEAVE(p_tcb)	trace_1(LOG_TYPE_DSP|LOG_LEAVE, p_tcb)

#define LOG_SYSLOG_WRI_LOG_ENTER(prio, p_syslog) \
								trace_wri_log((TRACE *) p_syslog)

#endif /* TOPPERS_TRACE_CONFIG_H */
