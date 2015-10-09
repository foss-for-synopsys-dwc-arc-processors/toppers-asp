/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2007-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: test_cpuexc3.c 2005 2010-12-31 04:20:08Z ertl-hiro $
 */

/* 
 *	CPU exception test (3)
 *
 * [test purpose]
 *
 *  test the states when cpu exception happens in non-task context, 
 *  interrupts are unlocked and cpu is unlocked. test with ipm = 
 *  TIPM_ENAALL, task dispatch disabled, task exception enabled.
 *
 * [test items]
 *
 * 	when cpu exception happens in non-task context, interrupts are 
 * 	unlocked and cpu is unlocked.
 *
 *	(A) the cpu lock flag is not changed when cpu exception handler
 *	starts to run
 *	(B) the ipm is not changed when cpu exception handler starts to run
 *		! as the ipm cannot be read in cpu exception handler, the test 
 *		cannot be done¡¥
 *	(C) the dispatch disable flag is not changed when cpu exception 
 *		handler starts to run
 *	(D) the task exception disable flag is not changed when cpu exception 
 *		handler starts to run
 *	(E)  recover the original cpu lock flag when cpu exception handler
 *	returns
 *		! only can test the situation when cpu exception handler can
 *		return.
 *	(F) recover the original ipm when cpu exception handler returns
 *		! only can test the situation when cpu exception handler can
 *		return.
 *	(G) the dispatch disable flag is not changed when cpu exception
 *	handler returns.
 *		! only can test the situation when cpu exception handler can
 *		return.
 *	(H) the task exception disable flag is not changed when cpu exception 
 *		handler returns.
 *		! only can test the situation when cpu exception handler can
 *		return.
 *	(I) xsns_xpn returns true
 *	(J) xsns_dpn returns true
 *
 * [required resources]
 *
 *	TASK1: TA_ACT, middle priority, with task exception routine
 *	CPUEXC1: TA_NULL
 *	ALM1: TA_NULL
 *
 * [test sequence]
 *
 *	== TASK1(middle priority)==
 *	1:	check states
 *		dis_dsp()
 *		ena_tex()
 *	2:	check states
 *		sta_alm(ALM1, 1U)
 *		wait for alarm handler1
 *	== ALM1 ==
 *	3:	check states
 *		RAISE_CPU_EXCEPTION
 *	== CPUEXC1 ==
 *	4:	check states				... (A)(C)(D)
 *		xsns_xpn() == true			... (I)
 *		xsns_dpn() == true			... (J)
 *	5:	finish if the cpu exception handler cannot return
 *	5:	return
 *	== ALM1(continue)==
 *	6:	check states				... (E)(G)(H)
 *		return
 *	== TASK1(continue)==
 *	7:	check states				... (F)
 *	8:	test finish
 */

#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_cpuexc.h"

volatile bool_t	alm1_flag = false;

void
task1(intptr_t exinf)
{
	ER		ercd;

	check_point(1);
	check_state(false, false, TIPM_ENAALL, false, false, true);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);
	ercd = ena_tex();
	check_ercd(ercd, E_OK);

	check_point(2);
	check_state(false, false, TIPM_ENAALL, true, true, false);
	ercd = sta_alm(ALM1, 1U);
	check_ercd(ercd, E_OK);

	while (!(alm1_flag));

	check_point(7);
	check_state(false, false, TIPM_ENAALL, true, true, false);

	check_finish(8);
}

void
tex_task1(TEXPTN texptn, intptr_t exinf)
{
	check_point(0);
}

void
task2(intptr_t exinf)
{
	check_point(0);
}

void
alarm1_handler(intptr_t exinf)
{
	check_point(3);
	check_state_i(true, false, true, true, false);
	RAISE_CPU_EXCEPTION;

	check_point(6);
	check_state_i(true, false, true, true, false);
	alm1_flag = true;
}

void
cpuexc_handler(void *p_excinf)
{
	check_point(4);
	check_state_i(true, false, true, true, false);
	check_assert(xsns_xpn(p_excinf) == true);
	check_assert(xsns_dpn(p_excinf) == true);

#ifdef CANNOT_RETURN_CPUEXC
	check_finish(5);
#endif /* CANNOT_RETURN_CPUEXC */

	check_point(5);
}
