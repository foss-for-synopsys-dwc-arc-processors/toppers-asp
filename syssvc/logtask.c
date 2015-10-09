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
 *  @(#) $Id: logtask.c 1111 2008-06-12 08:42:26Z ertl-hiro $
 */

/*
 *		system log task
 */

#include <kernel.h>
#include <t_syslog.h>
#include <log_output.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "logtask.h"

/*
 *  the output SIO port id of system log task
 */
static ID	logtask_portid;

/*
 *  output one character to serial interface
 */
static void
logtask_putc(char_t c)
{
	serial_wri_dat(logtask_portid, &c, 1);
}

/*
 *  wait for flushing the system logs.
 *
 *  the task that calls this function will wait for 
 *  the logtask to output the system logs untill the number of system
 *  logs is less than count
 */
ER
logtask_flush(uint_t count)
{
	T_SYSLOG_RLOG	rlog;
	T_SERIAL_RPOR	rpor;
	ER				ercd, rercd;

	if (sns_dpn()) {
		ercd = E_CTX;
	}
	else {
		for (;;) {
			if (syslog_ref_log(&rlog) < 0) {
				ercd = E_SYS;
				goto error_exit;
			}
			if (rlog.count <= count) {
				if (count == 0U) {
					/*
					 *  if the count is 0, check whether the serail output buffer is empty
					 */
					if (serial_ref_por(logtask_portid, &rpor) < 0) {
						ercd = E_SYS;
						goto error_exit;
					}
					if (rpor.wricnt == 0U) {
						ercd = E_OK;
						goto error_exit;
					}
				}
				else {
					ercd = E_OK;
					goto error_exit;
				}
			}

			/*
			 *  wait for LOGTASK_FLUSH_WAIT ms
			 */
			rercd = dly_tsk(LOGTASK_FLUSH_WAIT);
			if (rercd < 0) {
				ercd = (rercd == E_RLWAI) ? rercd : E_SYS;
				goto error_exit;
			}
		}
	}

  error_exit:
	return(ercd);
}

/*
 *  the task body of log task
 */
void
logtask_main(intptr_t exinf)
{
	SYSLOG	syslog;
	uint_t	lost;
	ER_UINT	rercd;

	logtask_portid = (ID) exinf;
	serial_opn_por(logtask_portid);
	syslog_msk_log(LOG_UPTO(LOG_NOTICE), LOG_UPTO(LOG_EMERG));
	syslog_1(LOG_NOTICE, "System logging task is started on port %d.",
													logtask_portid);
	for (;;) {
		lost = 0U;
		while ((rercd = syslog_rea_log(&syslog)) >= 0) {
			lost += (uint_t) rercd;
			if (syslog.logtype >= LOG_TYPE_COMMENT) {
				if (lost > 0U) {
					syslog_lostmsg(lost, logtask_putc);
					lost = 0U;
				}
				syslog_print(&syslog, logtask_putc);
				logtask_putc('\n');
			}
		}
		if (lost > 0U) {
			syslog_lostmsg(lost, logtask_putc);
		}
		dly_tsk(LOGTASK_INTERVAL);
	}
}

/*
 *  terminate routine of system log task
 *
 *  this function will be called when the kernel is terminated
 */
void
logtask_terminate(intptr_t exinf)
{
	char_t	c;
	SYSLOG	syslog;
	bool_t	msgflg = false;
	ER_UINT	rercd;

	/*
	 *  output all the data in the receive buffer of logtask_porid port
	 *  through the low level output function
	 */
	while (serial_get_chr(logtask_portid, &c)) {
		target_fput_log(c);
	}

	/*
	 *  output all the system logs in the log buffer through the low
	 *  level output function
	 */
	while ((rercd = syslog_rea_log(&syslog)) >= 0) {
		if (!msgflg) {
			/*
			 *  output a message to show that there are some system logs
			 *  remaining in the log buffer.
			 */
			syslog_printf("-- buffered messages --\n", NULL, target_fput_log);
			msgflg = true;
		}
		if (rercd > 0) {
			syslog_lostmsg((uint_t) rercd, target_fput_log);
		}
		if (syslog.logtype >= LOG_TYPE_COMMENT) {
			syslog_print(&syslog, target_fput_log);
			target_fput_log('\n');
		}
	}
}
