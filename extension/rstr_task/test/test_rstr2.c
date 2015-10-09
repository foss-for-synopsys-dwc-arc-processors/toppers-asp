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
 *  $Id: test_rstr2.c 1887 2010-08-06 06:15:19Z ertl-hiro $
 */

/* 
 *		restricted task test(2)
 *
 * [test Purpose]
 *
 *  ras_pri sevice call test¡¥
 *
 * [test Items]
 *
 *	(A) ras_pri is called from a restrictions task and a priority is raised.
 *		(A-1) When you call in the dispatch-enabled state
 *		(A-2) When the highest priority task is not a run state and
 *			 it calls by a dispatch prohibited state
 *	(B) when call ras_pri(TPRI_INI) ,restricted task priority will raise,
 * 		it return to the initial priority
 *		(B-1) when the new priority is higher than the startup priority,
 *		      then the check of whether task switch need will occurs
 *		(B-2) when the new priority is the same as the startup priority¡¤
 *		      then the check of whether task switch need won't occurs
 *		(B-3) when the new priority is the lower than the startup priority¡¤
 *		      then the check of whether task switch need won't occurs
 *	(C) if in a restricted task, ras_pri tends to change the priority to
 *		a lower priority than startup priority, it will check if the action is E_ILUSE
 *
 * [required resources]
 *
 *	TASK1: middle priority, the main task, start after create
 *	TASK2: middle priorty, restricted task
 *	TASK3: high priorty
 *
 * [test sequence]
 *
 *	== TASK1(priority: middle)==
 *	1:	act_tsk(TASK2)
 *	2:	slp_tsk()
 *	== TASK2(priority: middle)==
 *	3:	get_pri(TSK_SELF, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	4:	ras_pri(HIGH_PRIORITY)
 *	5:	get_pri(TSK_SELF, &tskpri)
 *		assert(tskpri == HIGH_PRIORITY)
 *	6:	act_tsk(TASK3)
 *	7:	ras_pri(LOW_PRIORITY) -> E_ILUSE			... (C)
 *	8:	ras_pri(TPRI_INI)							... (B-1)
 *	== TASK3(priority: high)==
 *	9:	get_pri(TASK2, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	10:	slp_tsk()
 *	== TASK2(continue)==
 *	11:	dis_dsp()
 *	12:	wup_tsk(TASK3)
 *	13:	ras_pri(HIGH_PRIORITY)						... (A-2)
 *	14:	ena_dsp()
 *	15:	ras_pri(TPRI_INI)							... (B-3)
 *	== TASK3(continue)==
 *	16:	chg_pri(TSK_SELF, LOW_PRIORITY)
 *	== TASK2(continue)==
 *	17:	ras_pri(HIGH_PRIORITY)
 *	18:	wup_tsk(TASK1)
 *	19:	ras_pri(TPRI_INI)							... (B-2)
 *	20:	ext_tsk()
 *	== TASK1(continue)==
 *	21: END
 */

#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_rstr1.h"

void
task1(intptr_t exinf)
{
	ER		ercd;

	check_point(1);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_finish(21);

	check_point(0);
}

void
task2(intptr_t exinf)
{
	ER		ercd;
	PRI		tskpri;

	check_point(3);
	ercd = get_pri(TSK_SELF, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(4);
	ercd = ras_pri(HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = get_pri(TSK_SELF, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == HIGH_PRIORITY);

	check_point(6);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = ras_pri(LOW_PRIORITY);
	check_ercd(ercd, E_ILUSE);

	check_point(8);
	ercd = ras_pri(TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = ras_pri(HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(15);
	ercd = ras_pri(TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = ras_pri(HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = ras_pri(TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_point(0);
}

void
task3(intptr_t exinf)
{
	ER		ercd;
	PRI		tskpri;

	check_point(9);
	ercd = get_pri(TASK2, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(10);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = chg_pri(TSK_SELF, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(0);
}
