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
 *  $Id: test_rstr1.c 1887 2010-08-06 06:15:19Z ertl-hiro $
 */

/* 
 *		restricted task test (1)
 *
 * [test purpose]
 *
 *  Test basic behaviors(except for ras_pri) of restricted task
 *
 * [test items]
 *
 *	(A) In restricted task, when you make a service call that bring the task state
 *		to waiting state , E_NOSPT error will return
 *		(A-1) slp_tsk
 *		(A-2) tslp_tsk
 *		(A-3) dly_tsk
 *      # although there are more, it just stop
 *	(B) As a restricted task，when you call chg_pri，wup_tsk，iwup_tsk，can_wup，
 *	    rel_wai，irel_wai，sus_tsk，rsm_tsk，E_NOSPT error will return
 *		(B-1) chg_pri
 *		(B-2) wup_tsk
 *		(B-3) iwup_tsk
 *		(B-4) can_wup
 *		(B-5) rel_wai
 *		(B-6) irel_wai
 *		(B-7) sus_tsk
 *		(B-8) rsm_tsk
 *  (C) When the task next task is restricted, call rot_rdq，irot_rdq,
 *  	E_NOSPT error will return
 *		(C-1) rot_rdq work properly
 *		(C-2) rot_rdq return E_NOSPT
 *		(C-3) irot_rdq work properly
 *		(C-4) irot_rdq return E_NOSPT
 *
 * [required resources]
 *
 *	TASK1: middle priority, the main task, start after create
 *	TASK2: high priorty, restricted task
 *	TASK3: middle priorty, restricted task
 *	TASK4: middle priorty, restricted task
 *	ALM1:  alarm handler
 *
 * [test sequence]
 *
 *	== TASK1(priority:middle)==
 *	1:	act_tsk(TASK2)
 *	== TASK2-1(priority:high)==
 *	2:	slp_tsk() -> E_NOSPT						... (A-1)
 *	3:	tslp_tsk(10) -> E_NOSPT						... (A-2)
 *	4:	dly_tsk(10) -> E_NOSPT						... (A-3)
 *  5:	ext_tsk()
 *	== TASK1(continue)==
 *	6:	chg_pri(TASK3, HIGH_PRIORITY) -> E_NOSPT	... (B-1)
 *	7:	wup_tsk(TASK3) -> E_NOSPT					... (B-2)
 *	8:	can_wup(TASK3) -> E_NOSPT					... (B-4)
 *	9:	rel_wai(TASK3) -> E_NOSPT					... (B-5)
 *	10:	sus_tsk(TASK3) -> E_NOSPT					... (B-7)
 *	11:	rsm_tsk(TASK3) -> E_NOSPT					... (B-8)
 *	12:	sta_alm(ALM1, 10)
 *	13:	slp_tsk()
 *	== ALM1 ==
 *	14:	iwup_tsk(TASK3) -> E_NOSPT					... (B-3)
 *	15:	irel_wai(TASK3) -> E_NOSPT					... (B-6)
 *	16:	iwup_tsk(TASK1)
 *	17:	iact_tsk(TASK3)
 *	18:	iact_tsk(TASK4)
 *	19:	irot_rdq(MID_PRIORITY)						... (C-3)
 *	20:	irot_rdq(MID_PRIORITY) -> E_NOSPT			... (C-4)
 *	21:	RETURN
 *	== TASK3-1(priority:middle)==
 *	22:	act_tsk(TASK2)
 *	== TASK2-2(priority:high)Second ==
 *	23:	rot_rdq(MID_PRIORITY) -> E_NOSPT			... (C-2)
 *	24:	ext_tsk()
 *	== TASK3-1(continue)==
 *	25:	ext_tsk()
 *	== TASK4-1(priority:middle)First==
 *	26:	ext_tsk()
 *	== TASK1(continue)==
 *	27:	act_tsk(TASK3)
 *	28:	act_tsk(TASK4)
 *	29:	act_tsk(TASK2)
 *	== TASK2-3(priority:high)Third ==
 *	30:	rot_rdq(MID_PRIORITY)						... (C-1)
 *	31:	ext_tsk()
 *	== TASK3-2(priority:middle)Second ==
 *	32:	ext_tsk()
 *	== TASK4-2(priority:middle)Second ==
 *	33:	ext_tsk()
 *	== TASK1(continue)==
 *	34: END
 */

#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_rstr1.h"

void
alarm1_handler(intptr_t exinf)
{
	ER		ercd;

	check_point(14);
	ercd = iwup_tsk(TASK3);
	check_ercd(ercd, E_NOSPT);

	check_point(15);
	ercd = irel_wai(TASK3);
	check_ercd(ercd, E_NOSPT);

	check_point(16);
	ercd = iwup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = iact_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = iact_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = irot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = irot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_NOSPT);

	check_point(21);
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

	check_point(6);
	ercd = chg_pri(TASK3, HIGH_PRIORITY);
	check_ercd(ercd, E_NOSPT);

	check_point(7);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_NOSPT);

	check_point(8);
	ercd = can_wup(TASK3);
	check_ercd(ercd, E_NOSPT);

	check_point(9);
	ercd = rel_wai(TASK3);
	check_ercd(ercd, E_NOSPT);

	check_point(10);
	ercd = sus_tsk(TASK3);
	check_ercd(ercd, E_NOSPT);

	check_point(11);
	ercd = rsm_tsk(TASK3);
	check_ercd(ercd, E_NOSPT);

	check_point(12);
	ercd = sta_alm(ALM1, 10);
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(27);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_finish(34);

	check_point(0);
}

static uint_t	task2_count = 0;

void
task2(intptr_t exinf)
{
	ER		ercd;

	switch (++task2_count) {
	case 1:
		check_point(2);
		ercd = slp_tsk();
		check_ercd(ercd, E_NOSPT);

		check_point(3);
		ercd = tslp_tsk(10);
		check_ercd(ercd, E_NOSPT);

		check_point(4);
		ercd = dly_tsk(10);
		check_ercd(ercd, E_NOSPT);

		check_point(5);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(23);
		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_NOSPT);

		check_point(24);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 3:
		check_point(30);
		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(31);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);
	}
	check_point(0);
}

static uint_t	task3_count = 0;

void
task3(intptr_t exinf)
{
	ER		ercd;

	switch (++task3_count) {
	case 1:
		check_point(22);
		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		check_point(25);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(32);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);
	}
	check_point(0);
}

static uint_t	task4_count = 0;

void
task4(intptr_t exinf)
{
	ER		ercd;

	switch (++task4_count) {
	case 1:
		check_point(26);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(33);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);
	}
	check_point(0);
}
