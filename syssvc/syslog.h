/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2008 by Embedded and Real-Time Systems Laboratory
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
 *		(a) The above copyright notice, this use conditions, and the
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
 *  $Id: syslog.h 1788 2010-05-16 21:06:02Z ertl-hiro $
 */

/*
 *	system log module
 */

#ifndef TOPPERS_SYSLOG_H
#define TOPPERS_SYSLOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  default definition of trace macros
 */
#include <t_syslog.h>

/*
 *  log buffer size
 */
#ifndef TCNT_SYSLOG_BUFFER
#define TCNT_SYSLOG_BUFFER	32		/*  log buffer size */
#endif /* TCNT_SYSLOG_BUFFER */

/*
 *  system log module initialize routine
 */
extern void	syslog_initialize(intptr_t exinf) throw();

/*
 *   output system log
 */
extern ER	syslog_wri_log(uint_t prio, const SYSLOG *p_syslog) throw();

/*
 *   read system log from log buffer
 */
extern ER_UINT	syslog_rea_log(SYSLOG *p_syslog) throw();

/*
 *  set the current priority of system log 
 */
extern ER	syslog_msk_log(uint_t logmask, uint_t lowmask) throw();

/*
 *  refer the status of log buffer
 */
extern ER	syslog_ref_log(T_SYSLOG_RLOG *pk_rlog) throw();

#ifdef __cplusplus
}
#endif

#endif /* TOPPERS_SYSLOG_H */
