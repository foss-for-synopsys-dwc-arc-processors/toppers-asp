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
 *  @(#) $Id: t_syslog.h 1176 2008-07-01 10:24:46Z ertl-hiro $
 */

/*
 *		definitions of system log output
 *
 *  system logging service is a service to output the system log
 *  information. Because it is used to output the log information from
 *  the kernel, there are no operations causing waiting.
 *
 *  The log information can be written to the log buffer in the kernel
 *  or be output through the low level output function please use the
 *  extended service call to choose.
 *
 *  If the log buffer is overflow, the old log information will be
 *  covered.
 *
 *  When this head file is included in assembly language source file,
 *  the macro TOPPERS_MACRO_ONLY should be defined. Then, the
 *  definitions that are not Marcos will be excluded.
 *
 *  t_stddef.h is required to be included before this head file.
 */

#ifndef TOPPERS_T_SYSLOG_H
#define TOPPERS_T_SYSLOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  defining of log information type
 */
#define LOG_TYPE_COMMENT	UINT_C(0x01)	/* comment */
#define LOG_TYPE_ASSERT		UINT_C(0x02)	/* assertion fail */

#define LOG_TYPE_INH		UINT_C(0x11)	/* interrupt handler */
#define LOG_TYPE_ISR		UINT_C(0x12)	/* interrupt service routine */
#define LOG_TYPE_CYC		UINT_C(0x13)	/* cyclic handler */
#define LOG_TYPE_ALM		UINT_C(0x14)	/* alarm Handler */
#define LOG_TYPE_OVR		UINT_C(0x15)	/* overrun handler */
#define LOG_TYPE_EXC		UINT_C(0x16)	/* CPU exception handler */
#define LOG_TYPE_TEX		UINT_C(0x17)	/* task exception handling routine */
#define LOG_TYPE_TSKSTAT	UINT_C(0x18)	/* task status changes */
#define LOG_TYPE_DSP		UINT_C(0x19)	/* dispatcher */
#define LOG_TYPE_SVC		UINT_C(0x1a)	/* service Call */

#define LOG_ENTER			UINT_C(0x00)	/* start / entrance */
#define LOG_LEAVE			UINT_C(0x80)	/* exit */

/*
 *  defining of log information importance
 */
#define LOG_EMERG			UINT_C(0)		/* error that causes shutdown */
#define LOG_ALERT			UINT_C(1)
#define LOG_CRIT			UINT_C(2)
#define LOG_ERROR			UINT_C(3)		/* system error */
#define LOG_WARNING			UINT_C(4)		/* warning message */
#define LOG_NOTICE			UINT_C(5)
#define LOG_INFO			UINT_C(6)
#define LOG_DEBUG			UINT_C(7)		/* debug messages */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  data structure of log information
 */

#define TMAX_LOGINFO	6

typedef struct {
	uint_t		logtype;				/* log information type */
	SYSTIM		logtim;					/* log time */
	intptr_t	loginfo[TMAX_LOGINFO];	/* other log information */
} SYSLOG;

/*
 *  macro to create the bitmap of log information importance
 */
#define LOG_MASK(prio)		(1U << (prio))
#define LOG_UPTO(prio)		((1U << ((prio) + 1)) - 1)

/*
 *  packet format for reference
 */
typedef struct t_syslog_rlog {
	uint_t	count;		/* the number of logs in the log buffer */
	uint_t	lost;		/* the number of lost logs */
	uint_t	logmask;	/* the importance of logs that should be recorded in the log buffer */
	uint_t	lowmask;	/* the importance of logs that should be output in low-level */
} T_SYSLOG_RLOG;

#ifndef TOPPERS_OMIT_SYSLOG

/*
 *  log information output library functions
 */

extern ER	syslog_wri_log(uint_t prio, const SYSLOG *p_syslog) throw();

Inline void
_syslog_0(uint_t prio, uint_t type)
{
	SYSLOG	syslog;

	syslog.logtype = type;
	(void) syslog_wri_log(prio, &syslog);
}

Inline void
_syslog_1(uint_t prio, uint_t type, intptr_t arg1)
{
	SYSLOG	syslog;

	syslog.logtype = type;
	syslog.loginfo[0] = arg1;
	(void) syslog_wri_log(prio, &syslog);
}

Inline void
_syslog_2(uint_t prio, uint_t type, intptr_t arg1, intptr_t arg2)
{
	SYSLOG	syslog;

	syslog.logtype = type;
	syslog.loginfo[0] = arg1;
	syslog.loginfo[1] = arg2;
	(void) syslog_wri_log(prio, &syslog);
}

Inline void
_syslog_3(uint_t prio, uint_t type, intptr_t arg1, intptr_t arg2,
														intptr_t arg3)
{
	SYSLOG	syslog;

	syslog.logtype = type;
	syslog.loginfo[0] = arg1;
	syslog.loginfo[1] = arg2;
	syslog.loginfo[2] = arg3;
	(void) syslog_wri_log(prio, &syslog);
}

Inline void
_syslog_4(uint_t prio, uint_t type, intptr_t arg1, intptr_t arg2,
										intptr_t arg3, intptr_t arg4)
{
	SYSLOG	syslog;

	syslog.logtype = type;
	syslog.loginfo[0] = arg1;
	syslog.loginfo[1] = arg2;
	syslog.loginfo[2] = arg3;
	syslog.loginfo[3] = arg4;
	(void) syslog_wri_log(prio, &syslog);
}

Inline void
_syslog_5(uint_t prio, uint_t type, intptr_t arg1, intptr_t arg2,
							intptr_t arg3, intptr_t arg4, intptr_t arg5)
{
	SYSLOG	syslog;

	syslog.logtype = type;
	syslog.loginfo[0] = arg1;
	syslog.loginfo[1] = arg2;
	syslog.loginfo[2] = arg3;
	syslog.loginfo[3] = arg4;
	syslog.loginfo[4] = arg5;
	(void) syslog_wri_log(prio, &syslog);
}

Inline void
_syslog_6(uint_t prio, uint_t type, intptr_t arg1, intptr_t arg2,
				intptr_t arg3, intptr_t arg4, intptr_t arg5, intptr_t arg6)
{
	SYSLOG	syslog;

	syslog.logtype = type;
	syslog.loginfo[0] = arg1;
	syslog.loginfo[1] = arg2;
	syslog.loginfo[2] = arg3;
	syslog.loginfo[3] = arg4;
	syslog.loginfo[4] = arg5;
	syslog.loginfo[5] = arg6;
	(void) syslog_wri_log(prio, &syslog);
}

/*
 *  output library function for log information (comment) (vasyslog.c)
 */
extern void	syslog(uint_t prio, const char *format, ...) throw();

#else /* TOPPERS_OMIT_SYSLOG */

/*
 *  omit the output of log information
 */

Inline void
_syslog_0(uint_t prio, uint_t type)
{
}

Inline void
_syslog_1(uint_t prio, uint_t type, intptr_t arg1)
{
}

Inline void
_syslog_2(uint_t prio, uint_t type, intptr_t arg1, intptr_t arg2)
{
}

Inline void
_syslog_3(uint_t prio, uint_t type, intptr_t arg1, intptr_t arg2,
														intptr_t arg3)
{
}

Inline void
_syslog_4(uint_t prio, uint_t type, intptr_t arg1, intptr_t arg2,
										intptr_t arg3, intptr_t arg4)
{
}

Inline void
_syslog_5(uint_t prio, uint_t type, intptr_t arg1, intptr_t arg2,
							intptr_t arg3, intptr_t arg4, intptr_t arg5)
{
}

Inline void
_syslog_6(uint_t prio, uint_t type, intptr_t arg1, intptr_t arg2,
				intptr_t arg3, intptr_t arg4, intptr_t arg5, intptr_t arg6)
{
}

Inline void
syslog(uint_t prio, const char *format, ...)
{
}

#endif /* TOPPERS_OMIT_SYSLOG */

/*
 *  macro to output the log information (comment)
 *
 *  output the log information whose importance is prio, format is
 *  specified in format and arguments are specified in arg1 ~ argn.
 *  the data type of arg1 ~ argn should the ones that can be cast into
 *  intptr_t 
 */

#define syslog_0(prio, format) \
				_syslog_1(prio, LOG_TYPE_COMMENT, (intptr_t) format)

#define syslog_1(prio, format, arg1) \
				_syslog_2(prio, LOG_TYPE_COMMENT, (intptr_t) format, \
														(intptr_t)(arg1))

#define syslog_2(prio, format, arg1, arg2) \
				_syslog_3(prio, LOG_TYPE_COMMENT, (intptr_t) format, \
										(intptr_t)(arg1), (intptr_t)(arg2))

#define syslog_3(prio, format, arg1, arg2, arg3) \
				_syslog_4(prio, LOG_TYPE_COMMENT, (intptr_t) format, \
						(intptr_t)(arg1), (intptr_t)(arg2), (intptr_t)(arg3))

#define syslog_4(prio, format, arg1, arg2, arg3, arg4) \
				_syslog_5(prio, LOG_TYPE_COMMENT, (intptr_t) format, \
						(intptr_t)(arg1), (intptr_t)(arg2), (intptr_t)(arg3), \
														(intptr_t)(arg4))

#define syslog_5(prio, format, arg1, arg2, arg3, arg4, arg5) \
				_syslog_6(prio, LOG_TYPE_COMMENT, (intptr_t) format, \
						(intptr_t)(arg1), (intptr_t)(arg2), (intptr_t)(arg3), \
										(intptr_t)(arg4), (intptr_t)(arg5))

/*
 *  macro to output the log information (assert fail)
 */
#ifndef TOPPERS_assert_fail
#define TOPPERS_assert_fail(exp, file, line) \
				_syslog_3(LOG_EMERG, LOG_TYPE_ASSERT, (intptr_t)(file), \
										(intptr_t)(line), (intptr_t)(exp))
#endif /* TOPPERS_assert_fail */

#endif /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* TOPPERS_T_SYSLOG_H */
