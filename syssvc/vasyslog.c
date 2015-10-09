/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
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
 *  @(#) $Id: vasyslog.c 755 2008-03-07 17:42:37Z hiro $
 */

/*
 *	variable arguments system log library
 */

#include <t_stddef.h>
#include <t_syslog.h>
#include <stdarg.h>


/*
 * like printf
 */

void
syslog(uint_t prio, const char *format, ...)
{
	SYSLOG	syslog;
	va_list	ap;
	uint_t	i;
	char	c;
	bool_t	lflag;

	syslog.logtype = LOG_TYPE_COMMENT;
	syslog.loginfo[0] = (intptr_t) format;
	i = 1U;
	va_start(ap, format);

	while ((c = *format++) != '\0' && i < TMAX_LOGINFO) {
		if (c != '%') {
			continue;
		}

		lflag = false;
		c = *format++;
		while ('0' <= c && c <= '9') {
			c = *format++;
		}
		if (c == 'l') {
			lflag = true;
			c = *format++;
		}
		switch (c) {
		case 'd':
			syslog.loginfo[i++] = lflag ? (intptr_t) va_arg(ap, long_t)
										: (intptr_t) va_arg(ap, int_t);
			break;
		case 'u':
		case 'x':
		case 'X':
			syslog.loginfo[i++] = lflag ? (intptr_t) va_arg(ap, ulong_t)
										: (intptr_t) va_arg(ap, uint_t);
			break;
		case 'p':
			syslog.loginfo[i++] = (intptr_t) va_arg(ap, void *);
			break;
		case 'c':
			syslog.loginfo[i++] = (intptr_t) va_arg(ap, int);
			break;
		case 's':
			syslog.loginfo[i++] = (intptr_t) va_arg(ap, const char *);
			break;
		case '\0':
			format--;
			break;
		default:
			break;
		}
	}
	va_end(ap);
	(void) syslog_wri_log(prio, &syslog);
}
