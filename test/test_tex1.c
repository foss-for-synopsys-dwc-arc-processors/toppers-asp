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
 *  @(#) $Id: test_tex1.c 2007 2010-12-31 05:48:15Z ertl-hiro $
 */

/* 
 *		task exception test (1)
 *
 * [test purpose]
 *
 *  test of raising task exception in task context
 *
 * [test items]
 *
 *	(A) error check of ras_tex
 *		(A-1) target task is in dormant
 *		(A-2) no task exception routine defined for the target task
 *	(B) error check dis_tex
 *		(B-1) no task exception routine defined for the calling task
 *	(C) error check ena_tex
 *		(C-1) no task exception routine defined for the calling task
 *	(D) error check of ref_tex
 *		(D-1) target task is in dormant
 *		(D-2) no task exception routine defined for the target task
 *	(E) normal processing of ras_tex
 *		(E-1) the target task is the calling task, task exception enabled, 
 *			  imp all cleared, call task exception routine immediately
 *		(E-2) the target task is not the calling task
 *		(E-3) the target task is the calling task, but task exception disabled
 *		(E-4) the target task is the calling task, but ipm not cleared
 *	(F) normal processing of ena_tex
 *		(F-1) task exception request exits, and ipm all cleared, call task 
 *		exception routine immediately
 *		(F-2) no task exception request
 *		(F-3) task exception request exits, but ipm not cleared
 *	(G) normal processing of chg_ipm
 *		(G-1) task exception request exits, and task exception enabled, call 
 *		task exception routine immediately
 *		(G-2) no task exception request
 *		(G-3) task exception request exits, but task exception disabled
 *	(H) call in task dispatcher
 *		(H-1) task exception enabled and task exception request exits
 *		for the dispatched task
 *	(I) continue call when task exception routine returns (continue call)
 *	(J) the state recover when task exception routine returns 
 *		(J-1) return to task
 *		(J-2) continue call of task exception routine
 *	(K) multiple call of task exception routine
 *	(L) task dispatch happens when task exception routine returns 
 *
 * [required resources]
 *
 *	TASK1: main task. the task exception request raised by itself
 *	TASK2: the task exception request raised by other tasks
 *	TASK3: no task exception routine defined
 *	TASK4: dormant 
 *
 * [test sequency]
 *
 *	== TASK1(priority: 10)==
 *	1:	check init state
 *		ref_tex(TSK_SELF, &rtex)
 *		ras_tex(TASK3, 0x0001) -> E_OBJ		... (A-2)
 *		ras_tex(TASK4, 0x0001) -> E_OBJ		... (A-1)
 *		ref_tex(TASK3, &rtex) -> E_OBJ		... (D-2)
 *		ref_tex(TASK4, &rtex) -> E_OBJ		... (D-1)
 *	2:	ena_tex()							... (F-2)
 *		ref_tex(TSK_SELF, &rtex)
 *	3:	ras_tex(TSK_SELF, 0x0001)			... (E-1)
 *	== TASK1 task exception routine (1st time)==
 *	4:	check init state
 *	5:	dis_dsp() ... 4 states changed
 *		chg_ipm(TMAX_INTPRI)
 *		ena_tex()
 *		loc_cpu()
 *		return
 *	== TASK1(continue)==
 *	6:	check return state					... (J-1)
 *	7:	dis_dsp() ... disable dispatch, disable task exception
 *		dis_tex()
 *	8:	ras_tex(TASK1, 0x0002)				... (E-3)
 *		ref_tex(TSK_SELF, &rtex)
 *	9:	ena_tex()							... (F-1)
 *	== TASK1 task exception routine (2nd time)==
 *	10:	check init state
 *	11:	ras_tex(TASK1, 0x0001)				... (E-3)
 *		ras_tex(TASK1, 0x0002)				... (E-3)
 *	12:	ena_dsp() ...  3 states changed
 *		chg_ipm(TMAX_INTPRI)
 *		loc_cpu()
 *		return								... (I)
 *	== TASK1 task exception routine (3rd time)==
 *	13:	check init state
 *	14:	ena_dsp() ... enable dispatch, enable task exception
 *		chg_ipm(TMAX_INTPRI)
 *		ena_tex()
 *		chg_ipm(TIPM_ENAALL)				... (G-2)
 *		chg_ipm(TMAX_INTPRI)
 *	15: ras_tex(TSK_SELF, 0x0004)			... (E-4)
 *		dis_tex()
 *		chg_ipm(TIPM_ENAALL)				... (G-3)
 *		chg_ipm(TMAX_INTPRI)
 *		ena_tex()							... (F-3)
 *	16:	chg_ipm(TIPM_ENAALL)				... (G-1)(K)
 *	== TASK1 task exception routine (4th time)==
 *	17:	check init state
 *	18:	dis_dsp() ... 3 states changed
 *		chg_ipm(TMAX_INTPRI)
 *		loc_cpu()
 *		return
 *	== TASK1 task exception routine (3rd time continue)==
 *	19:	check return state					... (J-2)
 *		return
 *	== TASK1(continue)==
 *	20:	check return state					... (J-1)
 *	21: ena_dsp()
 *		rot_rdq(TPRI_SELF)
 *	== TASK2(priority: 10)	==
 *	22:	check init state
 *	23:	ena_tex()
 *		rot_rdq(TPRI_SELF)
 *	== TASK3(priority: 10)	==
 *	24:	check init state
 *	25:	ena_tex() -> E_OBJ					... (C-1)
 *	26:	dis_tex() -> E_OBJ					... (B-1)
 *		ext_tsk()
 *	== TASK1(continue)==
 *	27: ras_tex(TASK2, 0x0001)				... (E-2)
 *		ref_tex(TASK2, &rtex)
 *	28:	rot_rdq(TPRI_SELF)					... (H-1)
 *	== TASK2 task exception routine (1st time)==
 *	29:	check init state
 *		return
 *	== TASK2(continue)==
 *	30: ras_tex(TSK_SELF, 0x0002)
 *	== TASK2 task exception routine (2nd time)==
 *	31:	check init state
 *	32:	dis_dsp()
 *		rot_rdq(TPRI_SELF)
 *	33:	return								... (L)
 *	== TASK1(continue)==
 *	34:	return (task exits)
 *	== TASK2(continue)==
 *	35:	test exits
 */

#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_tex1.h"

void
tex_task1(TEXPTN texptn, intptr_t exinf)
{
	ER		ercd;

	switch (texptn) {
	case 0x0001:
		check_point(4);
		check_state(false, false, TIPM_ENAALL, false, false, true);

		/*
		 *  dispatch disabled, ipm changed, task exception enabled
		 */
		check_point(5);
		ercd = dis_dsp();
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);
		ercd = ena_tex();
		check_ercd(ercd, E_OK);
		ercd = loc_cpu();
		check_ercd(ercd, E_OK);
		check_state(false, true, TMAX_INTPRI, true, true, false);
		break;

	case 0x0002:
		check_point(10);
		check_state(false, false, TIPM_ENAALL, true, true, true);

		/*
		 *  raise task exception request
		 */
		check_point(11);
		ercd = ras_tex(TSK_SELF, 0x0001);
		check_ercd(ercd, E_OK);
		ercd = ras_tex(TSK_SELF, 0x0002);
		check_ercd(ercd, E_OK);

		/*
		 *  dispatch enabled, ipm changed, cpu locked
		 */
		check_point(12);
		ercd = ena_dsp();
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);
		ercd = loc_cpu();
		check_ercd(ercd, E_OK);
		check_state(false, true, TMAX_INTPRI, false, true, true);
		break;

	case 0x0003:
		check_point(13);
		check_state(false, false, TIPM_ENAALL, true, true, true);

		/*
		 *  dispatch enabled, ipm changed, task exception enabled
		 */
		check_point(14);
		ercd = ena_dsp();
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);
		ercd = ena_tex();
		check_ercd(ercd, E_OK);
		check_state(false, false, TMAX_INTPRI, false, true, false);

		ercd = chg_ipm(TIPM_ENAALL);
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);

		/*
		 *  raise task exception request
		 */
		check_point(15);
		ercd = ras_tex(TSK_SELF, 0x0004);

		ercd = dis_tex();
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TIPM_ENAALL);
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);
		ercd = ena_tex();
		check_ercd(ercd, E_OK);

		check_point(16);
		ercd = chg_ipm(TIPM_ENAALL);
		/* task exception routine runs here */
		check_ercd(ercd, E_OK);

		/*
		 *  check the state when task exception routine returns
		 */
		check_point(19);
		check_state(false, false, TIPM_ENAALL, false, false, false);
		break;

	case 0x0004:
		check_point(17);
		check_state(false, false, TIPM_ENAALL, false, false, true);

		/*
		 *   dispatch disabled, ipm changed, cpu locked
		 */
		check_point(18);
		ercd = dis_dsp();
		check_ercd(ercd, E_OK);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);
		ercd = loc_cpu();
		check_ercd(ercd, E_OK);
		check_state(false, true, TMAX_INTPRI, true, true, true);
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
	T_RTEX	rtex;

	/*
	 *  check init state
	 */
	check_point(1);
	check_state(false, false, TIPM_ENAALL, false, false, true);
	ercd = ref_tex(TSK_SELF, &rtex);
	check_ercd(ercd, E_OK);
	check_assert((rtex.texstat & TTEX_DIS) != 0);
	check_assert(rtex.pndptn == 0);

	/*
	 *  error check of ras_tex
	 */
	ercd = ras_tex(TASK3, 0x0001);
	check_ercd(ercd, E_OBJ);
	ercd = ras_tex(TASK4, 0x0001);
	check_ercd(ercd, E_OBJ);

	/*
	 *  error check of ref_tex
	 */
	ercd = ref_tex(TASK3, &rtex);
	check_ercd(ercd, E_OBJ);
	ercd = ref_tex(TASK4, &rtex);
	check_ercd(ercd, E_OBJ);

	/*
	 *  enable task exception
	 */
	check_point(2);
	ercd = ena_tex();
	check_ercd(ercd, E_OK);
	check_state(false, false, TIPM_ENAALL, false, false, false);
	ercd = ref_tex(TSK_SELF, &rtex);
	check_ercd(ercd, E_OK);
	check_assert((rtex.texstat & TTEX_ENA) != 0);
	check_assert(rtex.pndptn == 0);

	/*
	 *  raise task exception request
	 */
	check_point(3);
	ercd = ras_tex(TSK_SELF, 0x0001);
	/* task exception routine runs here */
	check_ercd(ercd, E_OK);

	/*
	 *  check the state when task exception routine returns
	 */
	check_point(6);
	check_state(false, false, TIPM_ENAALL, false, false, false);

	/*
	 *  dispatch disabled, task exception disabled
	 */
	check_point(7);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);
	ercd = dis_tex();
	check_ercd(ercd, E_OK);
	check_state(false, false, TIPM_ENAALL, true, true, true);

	/*
	 *  raise task exception request
	 */
	check_point(8);
	ercd = ras_tex(TASK1, 0x0002);
	check_ercd(ercd, E_OK);
	ercd = ref_tex(TSK_SELF, &rtex);
	check_ercd(ercd, E_OK);
	check_assert((rtex.texstat & TTEX_DIS) != 0);
	check_assert(rtex.pndptn == 0x0002);

	/*
	 *  enable task exception
	 */
	check_point(9);
	ercd = ena_tex();
	/* task exception routine runs here */
	check_ercd(ercd, E_OK);

	/*
	 *  check the state when task exception routine returns
	 */
	check_point(20);
	check_state(false, false, TIPM_ENAALL, true, true, false);

	/*
	 *  switch task2
	 */
	check_point(21);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);
	ercd = rot_rdq(TPRI_SELF);
	/* other tasks run here */
	check_ercd(ercd, E_OK);

	/*
	 *  raise the task exception of task2
	 */
	check_point(27);
	ercd = ras_tex(TASK2, 0x0001);
	check_ercd(ercd, E_OK);
	ercd = ref_tex(TASK2, &rtex);
	check_ercd(ercd, E_OK);
	check_assert((rtex.texstat & TTEX_ENA) != 0);
	check_assert(rtex.pndptn == 0x0001);

	/*
	 *  switch to task2
	 */
	check_point(28);
	ercd = rot_rdq(TPRI_SELF);
	/* other tasks run here */
	check_ercd(ercd, E_OK);

	/*
	 *  task exits
	 */
	check_point(34);
}

void
tex_task2(TEXPTN texptn, intptr_t exinf)
{
	ER		ercd;

	switch (texptn) {
	case 0x0001:
		check_point(29);
		check_state(false, false, TIPM_ENAALL, false, false, true);
		break;

	case 0x0002:
		check_point(31);
		check_state(false, false, TIPM_ENAALL, false, false, true);

		/*
		 *  disable dispatch, raise dispatch request
		 */
		check_point(32);
		ercd = dis_dsp();
		check_ercd(ercd, E_OK);
		ercd = rot_rdq(TPRI_SELF);
		check_ercd(ercd, E_OK);

		/*
		 * task dispatch happens when task exception routine returns
		 */
		check_point(33);
		break;

	default:
		check_point(0);
		break;
	}
}

void
task2(intptr_t exinf)
{
	ER		ercd;

	/*
	 *  check init state
	 */
	check_point(22);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *  enable task exception
	 */
	check_point(23);
	ercd = ena_tex();
	check_ercd(ercd, E_OK);
	check_state(false, false, TIPM_ENAALL, false, false, false);

	/*
	 *  switch to task3
	 */
	ercd = rot_rdq(TPRI_SELF);
	/* other tasks run here */
	check_ercd(ercd, E_OK);

	/*
	 *  raise task exception request
	 */
	check_point(30);
	ercd = ras_tex(TSK_SELF, 0x0002);
	/* task exception routine runs here */
	check_ercd(ercd, E_OK);

	/*
	 *  test exits
	 */
	check_finish(35);
}

void
task3(intptr_t exinf)
{
	ER		ercd;

	/*
	 *  check init state
	 */
	check_point(24);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *  enable task exception
	 */
	check_point(25);
	ercd = ena_tex();
	check_ercd(ercd, E_OBJ);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *  disable task exception
	 */
	check_point(26);
	ercd = dis_tex();
	check_ercd(ercd, E_OBJ);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *  task exits
	 */
	ercd = ext_tsk();
	check_point(0);
}

void
task4(intptr_t exinf)
{
	check_point(0);
}

void
tex_task4(TEXPTN texptn, intptr_t exinf)
{
	check_point(0);
}
