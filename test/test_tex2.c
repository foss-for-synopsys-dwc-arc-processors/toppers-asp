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
 *  @(#) $Id: test_tex2.c 1577 2009-05-31 14:30:51Z ertl-hiro $
 */

/* 
 *	task exception test (2)
 *
 * [test purpose]
 *	
 * the test of raising task exception in the interrupt handler and cpu
 * exception handler
 *
 * [test items]
 *
 *	(A) error check of iras_tex
 *		(A-1) target task is in dormant
 *		(A-2) no task exception routine defined for the target task
 *	(B) normal processing of iras_tex in the interrupt handler
 *		(B-1) target task is running and task exception is enabled
 *		(B-2) target task is not running
 *		(B-3) target task is running but task exception is disabled
 *	(C)  normal processing of iras_tex in the cpu exception handler
 *		(C-1) target task is running and task exception is enabled
 *		(C-2) target task is not running
 *		(C-3) target task is running but task exception is disabled
 *				-> no implementation(it cannot be implemented in
 *				target independent part)
 *	(D) the task exception processing at the exit of interrupt handler
 *		(D-1) the task exception request will be generated for the
 *		target task after task dispatch 
 *	(E) the task exception processing at the exit of interrupt handler
 *		(E-1) the task exception request will be generated for the
 *		target task after task dispatch 
 *	(F) no running task in sns_tex
 *
 * [required resources]
 *
 *	TASK1: main task. the running target task of raising task exception 
 *	TASK2: the target task of raising task exception which is not running
 *	TASK3: no task exception routine defined
 *	TASK4: dormant
 *	ALM1:  alarm handler1
 *	ALM2:  alarm handler2
 *	ALM3:  alarm handler3
 *	CPUEXC1: CPU exception handler1
 *
 * [test sequency]
 *
 *	== TASK1(priority: 10)==
 *	1:	check init state
 *		ref_tex(TSK_SELF, &rtex)
 *	2:	sta_alm(ALM1, 1U)
 *		wait for alarm handler1
 *	== ALM1 ==
 *	3:	check init state
 *		iras_tex(TASK3, 0x0001)		... (A-2)
 *		iras_tex(TASK4, 0x0001)		... (A-1)
 *		iras_tex(TASK2, 0x0001)		... (B-2)
 *		iras_tex(TASK1, 0x0001)		... (B-3)
 *		return
 *	== TASK1(continue)==
 *	4:	ena_tex()
 *	== TASK1 task exception routine (1st time)==
 *	5:	check init state
 *	6:	dis_dsp() ... 4 states changed
 *		chg_ipm(TMAX_INTPRI)
 *		ena_tex()
 *		loc_cpu()
 *		return
 *	== TASK1(continue)==
 *	7:	check return state
 *		ref_tex(TSK_SELF, &rtex)
 *	8:	sta_alm(ALM2, 1U)
 *		wait for alarm handler2
 *	== ALM2 ==
 *	9:	check init state
 *		iras_tex(TASK1, 0x0002)		... (B-1)
 *		return						... (D-1)
 *	== TASK1 task exception routine(2nd time)==
 *	10:	check init state
 *		return
 *	== TASK1(continue)==
 *	11:	sus_tsk(TASK2)
 *		sus_tsk(TASK3)
 *	12:	sta_alm(ALM3, 10U)
 *	13:	dly_tsk(50U)
 *	== ALM3 ==
 *	14:	check init state
 *		[sns_tex()]					... (F)
 *		iget_tid(&tskid)
 *		iras_tex(TASK1, 0x0004)		... (B-2)
 *		return 
 *	== TASK1 task exception routine (3rd time)==
 *	15:	check init state
 *		return
 *	== TASK1(continue)==
 *	16:	rsm_tsk(TASK2)
 *		rsm_tsk(TASK3)
 *		dis_dsp()
 *	17:	RAISE_CPU_EXCEPTION
 *	== CPUEXC1 ==
 *	18:	check init state
 *		xsns_xpn(p_excinf)
 *		iras_tex(TASK3, 0x0010)		... (A-2)
 *		iras_tex(TASK4, 0x0010)		... (A-1)
 *		iras_tex(TASK2, 0x0010)		... (C-2)
 *		iras_tex(TASK1, 0x0010)		... (C-1)
 *		return						... (E-1)
 *	== TASK1 task exception routine (4th time)==
 *	19:	check init state
 *	20:	ext_tsk()
 *	== TASK2(priority: 10)==
 *	21:	check init state
 *	22:	ena_tex()
 *	== TASK2 task exception routine ==
 *	23:	check init state
 *		return
 *	== TASK2(continue)==
 *	24:	sus_tsk(TASK3)
 *	25:	test finish
 */

#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_tex2.h"

volatile bool_t	alm1_flag = false;
volatile bool_t	alm2_flag = false;
volatile bool_t	alm3_flag = false;

void
tex_task1(TEXPTN texptn, intptr_t exinf)
{
	ER		ercd;

	switch (texptn) {
	case 0x0001:
		check_point(5);
		check_state(false, false, TIPM_ENAALL, false, false, true);

		/*
		 *  dispatch disabled, ipm changed, task exception enabled
		 */
		check_point(6);
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
		check_state(false, false, TIPM_ENAALL, false, false, true);
		break;

	case 0x0004:
		check_point(15);
		check_state(false, false, TIPM_ENAALL, false, false, true);
		break;

	case 0x0010:
		check_point(19);
		check_state(false, false, TIPM_ENAALL, true, true, true);

		/*
		 *  task exits
		 */
		check_point(20);
		ercd = ext_tsk();
		check_point(0);
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
	 *  start alarm handler1
	 */
	check_point(2);
	ercd = sta_alm(ALM1, 1U);
	check_ercd(ercd, E_OK);

	/*
	 *  wait alarm handler to run
	 */
	while (!(alm1_flag));

	/*
	 *  enable task exception
	 */
	check_point(4);
	ercd = ena_tex();
	/* task exception routine runs here */
	check_ercd(ercd, E_OK);

	/*
	 *  check the state when task exception routine returns
	 */
	check_point(7);
	check_state(false, false, TIPM_ENAALL, false, false, false);
	ercd = ref_tex(TSK_SELF, &rtex);
	check_ercd(ercd, E_OK);
	check_assert((rtex.texstat & TTEX_ENA) != 0);
	check_assert(rtex.pndptn == 0);

	/*
	 *  start alarm handler2
	 */
	check_point(8);
	ercd = sta_alm(ALM2, 1U);
	check_ercd(ercd, E_OK);

	/*
	 *  wait alarm handler2 to run
	 */
	while (!(alm2_flag));

	/*
	 *  suspend TASK2 and TASK3
	 */
	check_point(11);
	ercd = sus_tsk(TASK2);
	check_ercd(ercd, E_OK);
	ercd = sus_tsk(TASK3);
	check_ercd(ercd, E_OK);

	/*
	 *  start alarm handler3
	 */
	check_point(12);
	ercd = sta_alm(ALM3, 10U);
	check_ercd(ercd, E_OK);

	/*
	 *  delay task
	 */
	check_point(13);
	ercd = dly_tsk(50U);
	/* alarm handler3 runs here */
	check_ercd(ercd, E_OK);

	/*
	 *  resume TASK2 and TASK3
	 */
	check_point(16);
	ercd = rsm_tsk(TASK2);
	check_ercd(ercd, E_OK);
	ercd = rsm_tsk(TASK3);
	check_ercd(ercd, E_OK);

	/*
	 *  disable dispatch
	 */
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	/*
	 *  raise cpu exception
	 */
	check_point(17);
	RAISE_CPU_EXCEPTION;

	check_point(0);
}

void
tex_task2(TEXPTN texptn, intptr_t exinf)
{
	switch (texptn) {
	case 0x0011:
		check_point(23);
		check_state(false, false, TIPM_ENAALL, false, false, true);
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
	check_point(21);
	check_state(false, false, TIPM_ENAALL, false, false, true);

	/*
	 *   enable task exception
	 */
	check_point(22);
	ercd = ena_tex();
	/* task exception routine runs here */
	check_ercd(ercd, E_OK);

	/*
	 *  suspend TASK3
	 */
	check_point(24);
	ercd = sus_tsk(TASK3);
	check_ercd(ercd, E_OK);

	/*
	 *  test exits
	 */
	check_finish(25);
}

void
task3(intptr_t exinf)
{
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

void
alarm1_handler(intptr_t exinf)
{
	ER		ercd;

	/*
	 *  check init state
	 */
	check_point(3);
	check_state_i(true, false, false, true, true);

	/*
	 *  error check of iras_tex
	 */
	ercd = iras_tex(TASK3, 0x0001);
	check_ercd(ercd, E_OBJ);
	ercd = iras_tex(TASK4, 0x0001);
	check_ercd(ercd, E_OBJ);

	/*
	 *  raise task exception
	 */
	ercd = iras_tex(TASK2, 0x0001);
	check_ercd(ercd, E_OK);
	ercd = iras_tex(TASK1, 0x0001);
	check_ercd(ercd, E_OK);

	alm1_flag = true;
}

void
alarm2_handler(intptr_t exinf)
{
	ER		ercd;

	/*
	 *  check init state
	 */
	check_point(9);
	check_state_i(true, false, false, true, false);

	/*
	 *  raise task exception
	 */
	ercd = iras_tex(TASK1, 0x0002);
	check_ercd(ercd, E_OK);

	alm2_flag = true;
}

void
alarm3_handler(intptr_t exinf)
{
	ER		ercd;
	ID		tskid;

	check_point(14);
	check_state_i(true, false, false, true, true);
	ercd = iget_tid(&tskid);
	check_ercd(ercd, E_OK);
	check_assert(tskid == TSK_NONE);

	/*
	 *  raise task exception
	 */
	ercd = iras_tex(TASK1, 0x0004);
	check_ercd(ercd, E_OK);

	alm3_flag = true;
}

void
cpuexc_handler(void *p_excinf)
{
	ER		ercd;

	/*
	 *  check init state
	 */
	check_point(18);
	check_state_i(true, false, true, true, false);
	check_assert(xsns_xpn(p_excinf) == false);

	/*
	 *  error check of iras_tex
	 */
	ercd = iras_tex(TASK3, 0x0010);
	check_ercd(ercd, E_OBJ);
	ercd = iras_tex(TASK4, 0x0010);
	check_ercd(ercd, E_OBJ);

	/*
	 * raise task exception 
	 */
	ercd = iras_tex(TASK2, 0x0010);
	check_ercd(ercd, E_OK);
	ercd = iras_tex(TASK1, 0x0010);
	check_ercd(ercd, E_OK);
}
