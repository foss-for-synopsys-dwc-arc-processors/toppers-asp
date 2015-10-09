/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2006-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: test_sysstat1.c 2006 2010-12-31 04:20:56Z ertl-hiro $
 */

/* 
 *		system state test (1)
 *
 *  test sequence:
 *
 *	== TASK1(priority: 10)==
 *	1:	check init state
 *	2:	loc_cpu() ... change system state randomly
 *		chg_ipm(TMAX_INTPRI) -> E_CTX
 *	3:	unl_cpu()
 *	4:	chg_ipm(TMAX_INTPRI)
 *	5:	dis_dsp()
 *	6:	ena_tex()
 *	7:	chg_ipm(TIPM_ENAALL)
 *	8:	ena_dsp()
 *	9:	dis_tex()
 *	10:	dis_dsp() ... prepare to call task exception routine
 *		ena_tex()
 *	11:	ras_tex(TSK_SELF, 0x0001)
 *	== TASK1 task exception routine (just 1 time)==
 *	12:	check init state
 *	13:	ena_dsp() ... 3 states are changed,  check whether retruns
 *		chg_ipm(TMIN_INTPRI)
 *		ena_tex()
 *		return
 *	== TASK1(continue)==
 *	14:	check the return state
 *	15:	loc_cpu() ... lock cpu and exit
 *	16:	return (=task exits)
 *	== TASK2(priority: 10)	==
 *	17:	check init state
 *	18:	exit
 */

#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_sysstat1.h"

void
tex_task1(TEXPTN texptn, intptr_t exinf)
{
	ER		ercd;

	switch (texptn) {
	case 0x0001:
		check_point(12);
		check_state(false, false, TIPM_ENAALL, true, true, true);

		/*
		 *  enable dispatch, change ipm, enable task exception
		 */
		check_point(13);
		ercd = ena_dsp();
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMIN_INTPRI);
		check_ercd(ercd, E_OK);
		ercd = ena_tex();
		check_ercd(ercd, E_OK);
		check_state(false, false, TMIN_INTPRI, false, true, false);
		break;

	default:
		check_point(0);
		break;
	}
}

void
task1(intptr_t exinf)
{
	ER		ercd;

	/*
	 *  check init state
	 */
	check_point(1);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *  check the state when cpu is locked
	 */
	check_point(2);
	ercd = loc_cpu();
	check_ercd(ercd, E_OK);
	check_state(false, true, TIPM_ENAALL, false, true, true);

	/*
	 *  check the state when ipm is changed
	 */
	ercd = chg_ipm(TMAX_INTPRI);
	check_ercd(ercd, E_CTX);
	check_state(false, true, TIPM_ENAALL, false, true, true);

	/*
	 *  check the state when cpu is unlocked
	 */
	check_point(3);
	ercd = unl_cpu();
	check_ercd(ercd, E_OK);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *   check the state when ipm is changed
	 */
	check_point(4);
	ercd = chg_ipm(TMAX_INTPRI);
	check_ercd(ercd, E_OK);
	check_state(false, false, TMAX_INTPRI, false, true, true);

	/*
	 *   check the state when dispatch is disabled
	 */
	check_point(5);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);
	check_state(false, false, TMAX_INTPRI, true, true, true);

	/*
	 *   check the state when task exception is enabled
	 */
	check_point(6);
	ercd = ena_tex();
	check_ercd(ercd, E_OK);
	check_state(false, false, TMAX_INTPRI, true, true, false);

	/*
	 *   check the state when ipm is all cleared
	 */
	check_point(7);
	ercd = chg_ipm(TIPM_ENAALL);
	check_ercd(ercd, E_OK);
	check_state(false, false, TIPM_ENAALL, true, true, false);

	/*
	 *   check the state when dispatch is enabled
	 */
	check_point(8);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);
	check_state(false, false, TIPM_ENAALL, false, false, false);

	/*
	 *   check the state when task exception is disabled
	 */
	check_point(9);
	ercd = dis_tex();
	check_ercd(ercd, E_OK);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *  disable dispatch, enable task exception
	 */
	check_point(10);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);
	ercd = ena_tex();
	check_ercd(ercd, E_OK);
	check_state(false, false, TIPM_ENAALL, true, true, false);

	/*
	 *  raise task exception
	 */
	check_point(11);
	ercd = ras_tex(TSK_SELF, 0x0001);
	/* task exception routine is inserted here */
	check_ercd(ercd, E_OK);

	/*
	 *  check the state when task exception routine returns
	 */
	check_point(14);
	check_state(false, false, TIPM_ENAALL, true, true, false);

	/*
	 *  lock cpu
	 */
	check_point(15);
	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	/*
	 *  task exits
	 */
	check_point(16);
}

void
task2(intptr_t exinf)
{
	/*
	 *  check init state
	 */
	check_point(17);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	check_finish(18);
}
