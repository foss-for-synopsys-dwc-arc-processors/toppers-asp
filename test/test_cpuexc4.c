/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2007-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: test_cpuexc4.c 1577 2009-05-31 14:30:51Z ertl-hiro $
 */

/* 
 *		CPU exception test (3)
 *
 * [test purpose]
 *
 *  test the states when cpu exception happens in task context and
 *  interrupts are locked. test with ipm = TIPM_ENAALL, task dispatch
 *  disabled, task exception enabled.
 *
 * [test items]
 *
 * 	when cpu exception happens in task context and
 *  interrupts are locked.
 *
 *	(A) the cpu lock flag is not changed when cpu exception handler
 *	starts to run
 *		! for the cpu exception happended when interrups are locked, as
 *		the system stats cannot be read correctly, the test cannot be
 *		done
 *	(B) the ipm is not changed when cpu exception handler starts to run
 *		! for the cpu exception happended when interrups are locked, as
 *		the system stats cannot be read correctly, the test cannot be
 *		done
 *	(C) the dispatch disable flag is not changed when cpu exception 
 *		handler starts to run
 *		! for the cpu exception happended when interrups are locked, as
 *		the system stats cannot be read correctly, the test cannot be
 *		done
 *	(D) the task exception disable flag is not changed when cpu exception 
 *		handler starts to run
 *		! for the cpu exception happended when interrups are locked, as
 *		the system stats cannot be read correctly, the test cannot be
 *		done
 *	(E) recover the original cpu lock flag when cpu exception handler
 *	returns
 *		! only can test the situation when cpu exception handler can
 *		return.
 *	(F) recover the original ipm when cpu exception handler returns
 *		! only can test the situation when cpu exception handler can
 *		return.бе
 *	(G) recover the original dispatch disable flag when cpu exception
 *	handler returns
 *		! only can test the situation when cpu exception handler can
 *		return.
 *	(H) recover the original task exception disable flag when cpu 
 *	exception handler returns
 *		! only can test the situation when cpu exception handler can
 *		return.
 *	(I) xsns_xpn returns true
 *	(J) xsns_dpn returns true
 *
 * [required resources]
 *
 *	TASK1: TA_ACT, middle priority, with task exception routine
 *	CPUEXC1: TA_NULL
 *
 * [test sequence]
 *
 *	== TASK1(middle priority)==
 *	1:	check states
 *		dis_dsp()
 *		ena_tex()
 *	2:	check states
 *		SIL_LOC_INT()
 *		RAISE_CPU_EXCEPTION
 *	== CPUEXC1 ==
 *	3:	xsns_xpn() == true			... (I)
 *		xsns_dpn() == true			... (J)
 *	4:	finish if the cpu exception handler cannot return
 *	4:	return
 *	== TASK1(continue)==
 *	5:	SIL_UNL_LOC()
 *	6:	check states				... (E)(F)(G)(H)
 *	7:	test finish
 */

#include <sil.h>
#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_cpuexc.h"

void
task1(intptr_t exinf)
{
	ER		ercd;
	SIL_PRE_LOC;

	check_point(1);
	check_state(false, false, TIPM_ENAALL, false, false, true);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);
	ercd = ena_tex();
	check_ercd(ercd, E_OK);

	check_point(2);
	check_state(false, false, TIPM_ENAALL, true, true, false);
	SIL_LOC_INT();
	RAISE_CPU_EXCEPTION;

	check_point(5);
	SIL_UNL_INT();

	check_point(6);
	check_state(false, false, TIPM_ENAALL, true, true, false);

	check_finish(7);
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
	check_point(0);
}

void
cpuexc_handler(void *p_excinf)
{
	check_point(3);
	check_assert(xsns_xpn(p_excinf) == true);
	check_assert(xsns_dpn(p_excinf) == true);

#ifdef CANNOT_RETURN_CPUEXC
	check_finish(4);
#endif /* CANNOT_RETURN_CPUEXC */

	check_point(4);
}
