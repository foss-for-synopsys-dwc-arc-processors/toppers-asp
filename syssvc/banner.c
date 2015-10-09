/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: banner.c 1691 2010-01-01 15:55:49Z ertl-hiro $
 */

/*
 *		the output of the kernel startup message
 */

#include <kernel.h>
#include <t_syslog.h>
#include "target_syssvc.h"

#ifndef TARGET_COPYRIGHT
#define TARGET_COPYRIGHT
#endif /* TARGET_COPYRIGHT */

static const char banner[] = "\n"
"TOPPERS/ASP Kernel Release %d.%X.%d for " TARGET_NAME
" (" __DATE__ ", " __TIME__ ")\n"
"Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory\n"
"                            Toyohashi Univ. of Technology, JAPAN\n"
"Copyright (C) 2004-2010 by Embedded and Real-Time Systems Laboratory\n"
"            Graduate School of Information Science, Nagoya Univ., JAPAN\n"
TARGET_COPYRIGHT;

void
print_banner(intptr_t exinf)
{
	syslog_3(LOG_NOTICE, banner,
				(TKERNEL_PRVER >> 12) & 0x0fU,
				(TKERNEL_PRVER >> 4) & 0xffU,
				TKERNEL_PRVER & 0x0fU);
}
