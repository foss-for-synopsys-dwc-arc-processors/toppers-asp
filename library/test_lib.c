/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2005-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: test_lib.c 1573 2009-05-31 13:47:28Z ertl-hiro $
 */

/*
 *		library for test programs
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include <log_output.h>
#include "syssvc/syslog.h"
#include "target_syssvc.h"
#include "test_lib.h"

/*
 *	checkpoint
 */
static uint_t	check_count = 0u;

/*
 *	self-diagnostic function
 */
static BIT_FUNC	check_bit_func = NULL;

/*
 *	set self-diagnostic function
 */
void
set_bit_func(BIT_FUNC bit_func)
{
	check_bit_func = bit_func;
}

/*
 *  flush the system logs
 */
void
syslog_flush(void)
{
	SYSLOG	syslog;
	ER_UINT	rercd;

	/*
	 *  output the log information in the log buffer through the low
	 *  level output function
	 */
	while ((rercd = syslog_rea_log(&syslog)) >= 0) {
		if (rercd > 0) {
			syslog_lostmsg((uint_t) rercd, target_fput_log);
		}
		if (syslog.logtype >= LOG_TYPE_COMMENT) {
			syslog_print(&syslog, target_fput_log);
			target_fput_log('\n');
		}
	}
}

/*
 *	finish the test program
 */
void
test_finish(void)
{
	SIL_PRE_LOC;

	SIL_LOC_INT();
	syslog_flush();
	ext_ker();
	
	/* should not come here */
	SIL_UNL_INT();
}

/*
 *	checkpoint
 */
void
check_point(uint_t count)
{
	bool_t	errorflag = false;
	ER		rercd;
	SIL_PRE_LOC;

	/*
	 *  lock interrupt
	 */
	SIL_LOC_INT();

	/*
	 * check sequence
	 */
	if (++check_count == count) {
		syslog_1(LOG_NOTICE, "Check point %d passed.", count);
	}
	else {
		syslog_1(LOG_ERROR, "## Unexpected check point %d.", count);
		errorflag = true;
	}

	/*
	 *  checking the internal state of kernel
	 */
	if (check_bit_func != NULL) {
		rercd = (*check_bit_func)();
		if (rercd < 0) {
			syslog_2(LOG_ERROR, "## Internal inconsistency detected (%s, %d).",
								itron_strerror(rercd), SERCD(rercd));
			errorflag = true;
		}
	}

	/*
	 *  if an error is detected, finish the test program.
	 */
	if (errorflag) {
		test_finish();
	}

	/*
	 *  unlock interrupt
	 */
	SIL_UNL_INT();
}

/*
 *	check finish
 */
void
check_finish(uint_t count)
{
	check_point(count);
	syslog_0(LOG_NOTICE, "All check points passed.");
	test_finish();
}

/*
 *	the error handling of condition check
 */
void
_check_assert(const char *expr, const char *file, int_t line)
{
	syslog_3(LOG_ERROR, "## Assertion `%s' failed at %s:%u.",
								expr, file, line);
	test_finish();
}

/*
 *	the error handling of error code check
 */
void
_check_ercd(ER ercd, const char *file, int_t line)
{
	syslog_3(LOG_ERROR, "## Unexpected error %s detected at %s:%u.",
								itron_strerror(ercd), file, line);
	test_finish();
}
