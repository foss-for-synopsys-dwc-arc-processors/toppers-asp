/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2007-2010 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  $Id: test_utm1.c 1860 2010-07-25 07:36:40Z ertl-hiro $
 */

/*
 *		get_utm test (1)
 */

#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_utm1.h"

#define	NO_LOOP		ULONG_C(100000)

SYSUTM	recent_sysutm;
char	*recent_sysutm_pos;
uint_t	cyclic_count;

void
cyclic_handler(intptr_t exinf)
{
	SYSUTM		sysutm, prev_sysutm;
	char		*prev_sysutm_pos;

	iloc_cpu();
	get_utm(&sysutm);
	prev_sysutm = recent_sysutm;
	prev_sysutm_pos = recent_sysutm_pos;
	recent_sysutm = sysutm;
	recent_sysutm_pos = "CYC";
	iunl_cpu();

	if (prev_sysutm > sysutm) {
		syslog(LOG_NOTICE,
				"system performance time goes back: %ld(%s) %ld(CYC)",
				((long_t) prev_sysutm), prev_sysutm_pos, ((ulong_t) sysutm));
	}
	cyclic_count += 1;
}

void
main_task(intptr_t exinf)
{
	SYSUTM		sysutm, prev_sysutm;
	char		*prev_sysutm_pos;
	ulong_t		i;

	cyclic_count = 0U;
	get_utm(&recent_sysutm);
	syslog(LOG_NOTICE, "system performance time test starts.");

	for (i = 0; i < NO_LOOP; i++) {
		loc_cpu();
		get_utm(&sysutm);
		prev_sysutm = recent_sysutm;
		prev_sysutm_pos = recent_sysutm_pos;
		recent_sysutm = sysutm;
		recent_sysutm_pos = "TSK";
		unl_cpu();

		if (prev_sysutm > sysutm) {
			syslog(LOG_NOTICE,
				"system performance time goes back: %ld(%s) %ld(TSK)",
				((long_t) prev_sysutm), prev_sysutm_pos, ((ulong_t) sysutm));
		}
	}

	syslog(LOG_NOTICE, "system performance time test finishes.");
	syslog(LOG_NOTICE, "number of cyclic handler execution: %d", cyclic_count);
	test_finish();
}
