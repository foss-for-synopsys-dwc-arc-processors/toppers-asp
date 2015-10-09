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
 *  @(#) $Id: test_task1.c 1577 2009-05-31 14:30:51Z ertl-hiro $
 */

/* 
 *		task management module test (1)
 *
 * [test purpose]
 *
 *  unit test of make_runnable and make_non_runnable.
 *
 * [test items]
 *
 *	(A) the running task makes the higher priority task runnable. 
 *		(A-1) task dispatch is allowed
 *		(A-2) task dispatch is not allowed
 *	(B) the running task makes the same priority task runnable. 
 *		! confirm the runnable task gets into the tail of priority queue
 *	(C) the running task makes the lower priority task runnable. 
 *		! confirm the runnable task gets into the tail of priority queue
 *	(D) make the unrunnable task runnable
 *		! confirm the task dispatch is allowed
 *	(E) when task dispatch is disabled, the running task makes the task
 *		runnable with higher priority than already ready to run task 
 *	(F) when task dispatch is disabled, the running task makes the task
 *		runnable with same priority as already ready to run task 
 *	(G) when task dispatch is disabled, the running task makes the task
 *		runnable with lower priority than already ready to run task 
 *	(H) the running task gets into runnable
 *		(H-1) without runnable task
 *		(H-2) without same priority runnable task
 *		(H-3) with same priority runnable task
 *	(I) make the runnable task unrunnable
 *		(I-1) without same priority runnable task
 *		(I-2) with same priority runnable task
 *	(J) when task dispatch is disabled, the running task makes the ready
 *		to run task unrunnable
 *		(J-1) without same priority runnable task
 *		(J-2) with same priority runnable task
 *	(K) when task dispatch is disabled, make the running task unrunable
 *		! when task dispatch is disabled, task dispatch cannot happen to
 *		make the running task unrunnable, so no such situation
 *
 * [required resources]
 *
 *	TASK1: middle priority, main task, TA_ACT
 *	TASK2: high priority
 *	TASK3: middle priority
 *	TASK4: middle priority
 *	TASK5: low priority
 *	TASK6: low priority
 *	ALM1:  alarm handler
 *
 * [test sequence]
 *
 *	== TASK1(priority: middle)==
 *	1:	act_tsk(TASK2)					... (A-1)
 *	== TASK2(priority: high)==
 *	2:	slp_tsk()						... (H-2)
 *	== TASK1(continue)==
 *	3:	act_tsk(TASK3)					... (B)
 *	4:	act_tsk(TASK4)					... (B)
 *	5:	slp_tsk()						... (H-3)
 *	== TASK3(priority: middle)==
 *	6:	slp_tsk()						... (H-3)
 *	== TASK4(priority: middle)==
 *	7:	wup_tsk(TASK1)
 *	8:	slp_tsk()						... (H-3)
 *	== TASK1(continue)==
 *	9:	act_tsk(TASK5)					... (C)
 *	10:	act_tsk(TASK6)					... (C)
 *	11:	sus_tsk(TASK6)					... (I-2)
 *	12:	sus_tsk(TASK5)					... (I-1)
 *	13:	rsm_tsk(TASK5)					... (C)
 *	14:	rsm_tsk(TASK6)					... (C)
 *	15:	slp_tsk()						... (H-2)
 *	== TASK5(priority: low)==
 *	16:	slp_tsk()						... (H-3)
 *	== TASK6(priority: low)==
 *	17:	dis_dsp()
 *	18:	wup_tsk(TASK1)					... (A-2)
 *	19:	wup_tsk(TASK2)					... (E)
 *	20:	sus_tsk(TASK2)					... (J-1)
 *	21:	wup_tsk(TASK3)					... (F)
 *	22:	wup_tsk(TASK4)					... (F)
 *	23:	sus_tsk(TASK1)					... (J-2)
 *	24:	ena_dsp()
 *	== TASK3(continue)==
 *	25:	slp_tsk()						... (H-3)
 *	== TASK4(continue)==
 *	26:	slp_tsk()						... (H-2)
 *	== TASK6(continue)==
 *	27:	dis_dsp()
 *	28:	rsm_tsk(TASK1)					... (A-2)
 *	29:	wup_tsk(TASK5)					... (G)
 *	30:	ena_dsp()
 *	== TASK1(continue)==
 *	31:	slp_tsk()						... (H-2)
 *	== TASK6(continue)==
 *	32:	slp_tsk()						... (H-3)
 *	== TASK5(continue)==
 *	33:	sta_alm(ALM1, 10)
 *	34:	slp_tsk()						... (H-1)
 *	== ALM1 ==
 *	35:	iget_tid(&tskid)
 *		assert(tskid == TSK_NONE)
 *	36:	iwup_tsk(TASK1)					... (D)
 *	37:	RETURN
 *	== TASK1(continue)==
 *	38: END
 */

#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_task1.h"

void
alarm1_handler(intptr_t exinf)
{
	ER		ercd;
	ID		tskid;

	check_point(35);
	ercd = iget_tid(&tskid);
	check_ercd(ercd, E_OK);
	check_assert(tskid == TSK_NONE);

	check_point(36);
	ercd = iwup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(37);
	return;

	check_point(0);
}

void
task1(intptr_t exinf)
{
	ER		ercd;

	check_point(1);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(10);
	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = sus_tsk(TASK6);
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = sus_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = rsm_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = rsm_tsk(TASK6);
	check_ercd(ercd, E_OK);

	check_point(15);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(31);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_finish(38);

	check_point(0);
}

void
task2(intptr_t exinf)
{
	ER		ercd;

	check_point(2);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(0);
}

void
task3(intptr_t exinf)
{
	ER		ercd;

	check_point(6);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(0);
}

void
task4(intptr_t exinf)
{
	ER		ercd;

	check_point(7);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(0);
}

void
task5(intptr_t exinf)
{
	ER		ercd;

	check_point(16);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = sta_alm(ALM1, 10);
	check_ercd(ercd, E_OK);

	check_point(34);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(0);
}

void
task6(intptr_t exinf)
{
	ER		ercd;

	check_point(17);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = sus_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = sus_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(24);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(27);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = rsm_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(32);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(0);
}
