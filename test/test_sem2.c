/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2008,2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: test_sem2.c 1577 2009-05-31 14:30:51Z ertl-hiro $
 */

/* 
 *		semaphore test (2)
 *
 * [test purpose]
 *
 *  test isig_sem and its difference with sig_sem.
 *  test pol_sem, twai_sem and their difference with
 *  wai_sem.
 *
 * [test items]
 *
 *	(A) static error test of isig_sem
 *		(A-1) called in task context
 *		(A-2) called when CPU is locked
 *	(B)  release the waiting task through sig_sem
 *		(B-1) dispatch to the released task in idle
 *		(B-2) dispatch to the released task from the running task
 *		(B-3) no task dispatch when dispatch is not allowed
 *		(B-4) no task dispatch when the released task is suspended
 *		(B-5) no task dispatch when the released task's priority is
 *		lower
 *	(C) static error test of pol_sem
 *		(C-1) called in non-task context
 *		(C-2) called when CPU is locked
 *		(C-3) called when task dispatch is disabled (not E_CTX error)
 *		(C-4) called when the interrupt priority mask 
 *		is not cleared (not E_CTX error)
 *	(D) the failure of pol_sem
 *	(E) static error test of pol_sem of twai_sem
 *		(E-1) called in non-task context
 *		(E-2) called when CPU is locked
 *		(E-3) called when task dispatch is disabled
 *		(E-4) called when the interrupt priority mask 
 *		is not cleared
 *		(E-5) tmout is invalid
 *	(F) when tmout=TMO_POL, twai_sem returns polling failure
 *	(G) when tmout=TMO_FEVR,the task call twai_sem will get into the
 *	waiting state forever.
 *	(H) when tmout is set, the task call twai_sem will get into the
 *	waiting state with timeout.
 *
 * [required resources]
 *
 *	TASK1: middle priority, TA_ACT
 *	TASK2: high priority
 *	TASK3: low priority
 *	TASK4: middle priority
 *	TASK5: middle priority
 *	ALM1:  alarm handler
 *  SEM1:  TA_NULL, the init semcnt is 1, teh max semcnt is 1
 *
 * [test sequence]
 *
 *	== TASK1(priority: middle)==
 *  1:	isig_sem(SEM1) -> E_CTX				... (A-1)
 *	2:	loc_cpu()
 *		pol_sem(SEM1) -> E_CTX				... (C-2)
 *		twai_sem(SEM1, TMO_POL) -> E_CTX	... (E-2)
 *		unl_cpu()
 *	3:	dis_dsp()
 *		pol_sem(SEM1)						... (C-3)
 *		twai_sem(SEM1, TMO_POL) -> E_CTX	... (E-3)
 *		ena_dsp()
 *	4:	chg_ipm(TMAX_INTPRI)
 *		pol_sem(SEM1) -> E_TMOUT			... (C-4)(D)
 *		twai_sem(SEM1, TMO_POL) -> E_CTX	... (E-4)
 *		chg_ipm(TIPM_ENAALL)
 *	5:	twai_sem(SEM1, -2) -> E_PAR			... (E-5)
 *		twai_sem(SEM1, TMO_POL) -> E_TMOUT	... (F)
 *	6:	sta_alm(ALM1, 10)
 *	7:	twai_sem(SEM1, TMO_FEVR)			... (G)
 *	== ALM1-1 ==
 *	8:	pol_sem(SEM1) -> E_CTX				... (C-1)
 *		twai_sem(SEM1, TMO_POL) -> E_CTX	... (E-1)
 *	9:	iloc_cpu()
 *		isig_sem(SEM1) -> E_CTX				... (A-2)
 *		iunl_cpu()
 *	10:	isig_sem(SEM1)						... (B-1)
 *		RETURN
 *	== TASK1(continue)==
 *	11:	act_tsk(TASK3)
 *	12:	wai_sem(SEM1)
 *	== TASK3(priority: low)==
 *	13:	sta_alm(ALM1, 10)
 *		call(wait_var())
 *	== ALM1-2 ==
 *	14:	isig_sem(SEM1)						... (B-2)
 *		call(signal_var())
 *		RETURN
 *	== TASK1(continue)==
 *	15:	wai_sem(SEM1)
 *	== TASK3(continue)==
 *	16:	sta_alm(ALM1, 10)
 *		dis_dsp()
 *		call(wait_var())
 *	== ALM1-3 ==
 *	17:	isig_sem(SEM1)						... (B-3)
 *		call(signal_var())
 *		RETURN
 *	== TASK3(continue)==
 *	18:	ena_dsp()
 *	== TASK1(continue)==
 *	19:	wai_sem(SEM1)
 *	== TASK3(continue)==
 *	20:	sta_alm(ALM1, 10)
 *		sus_tsk(TASK1)
 *		call(wait_var())
 *	== ALM1-4 ==
 *	21:	isig_sem(SEM1)						... (B-4)
 *		call(signal_var())
 *		RETURN
 *	== TASK3(continue)==
 *	22:	rsm_tsk(TASK1)
 *	== TASK1(continue)==
 *	23:	act_tsk(TASK2)
 *	== TASK2(priority: high)==
 *	24:	tslp_tsk(10) -> E_TMOUT
 *	== TASK1(continue)==
 *	25:	wai_sem(SEM1)
 *	== TASK3(continue)==
 *	26:	ext_tsk() -> no return
 *	== TASK2(continue)==
 *	27:	sta_alm(ALM1, 10)
 *		call(wait_var())
 *	== ALM1-5 ==
 *	28:	isig_sem(SEM1)						... (B-5)
 *		call(signal_var())
 *		RETURN
 *	== TASK2(continue)==
 *	29:	ext_tsk() -> no return
 *	== TASK1(continue)==
 *	30:	sta_alm(ALM1, 10)
 *	31:	twai_sem(SEM1, 100) -> E_RLWAI
 *	== ALM1-6 ==
 *	32:	irel_wai(TASK1)
 *		RETURN
 *	== TASK1(continue)==
 *	33:	sta_alm(ALM1, 100)
 *	34:	twai_sem(SEM1, 10) -> E_TMOUT		... (H)
 *	35:	stp_alm(ALM1)
 *	36:	END
 */

#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_sem2.h"

static volatile bool_t	flagvar;

static void
wait_var(void)
{
	flagvar = false;
	while (!flagvar);
}

static void
signal_var(void)
{
	flagvar = true;
}

static uint_t	alarm1_count = 0;

void
alarm1_handler(intptr_t exinf)
{
	ER		ercd;

	switch (++alarm1_count) {
	case 1:
		check_point(8);
		ercd = pol_sem(SEM1);
		check_ercd(ercd, E_CTX);

		ercd = twai_sem(SEM1, TMO_POL);
		check_ercd(ercd, E_CTX);

		check_point(9);
		ercd = iloc_cpu();
		check_ercd(ercd, E_OK);

		ercd = isig_sem(SEM1);
		check_ercd(ercd, E_CTX);

		ercd = iunl_cpu();
		check_ercd(ercd, E_OK);

		check_point(10);
		ercd = isig_sem(SEM1);
		check_ercd(ercd, E_OK);

		return;

		check_point(0);

	case 2:
		check_point(14);
		ercd = isig_sem(SEM1);
		check_ercd(ercd, E_OK);

		signal_var();

		return;

		check_point(0);

	case 3:
		check_point(17);
		ercd = isig_sem(SEM1);
		check_ercd(ercd, E_OK);

		signal_var();

		return;

		check_point(0);

	case 4:
		check_point(21);
		ercd = isig_sem(SEM1);
		check_ercd(ercd, E_OK);

		signal_var();

		return;

		check_point(0);

	case 5:
		check_point(28);
		ercd = isig_sem(SEM1);
		check_ercd(ercd, E_OK);

		signal_var();

		return;

		check_point(0);

	case 6:
		check_point(32);
		ercd = irel_wai(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_point(0);
	}
	check_point(0);
}

void
task1(intptr_t exinf)
{
	ER		ercd;

	check_point(1);
	ercd = isig_sem(SEM1);
	check_ercd(ercd, E_CTX);

	check_point(2);
	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = pol_sem(SEM1);
	check_ercd(ercd, E_CTX);

	ercd = twai_sem(SEM1, TMO_POL);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = pol_sem(SEM1);
	check_ercd(ercd, E_OK);

	ercd = twai_sem(SEM1, TMO_POL);
	check_ercd(ercd, E_CTX);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = chg_ipm(TMAX_INTPRI);
	check_ercd(ercd, E_OK);

	ercd = pol_sem(SEM1);
	check_ercd(ercd, E_TMOUT);

	ercd = twai_sem(SEM1, TMO_POL);
	check_ercd(ercd, E_CTX);

	ercd = chg_ipm(TIPM_ENAALL);
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = twai_sem(SEM1, -2);
	check_ercd(ercd, E_PAR);

	ercd = twai_sem(SEM1, TMO_POL);
	check_ercd(ercd, E_TMOUT);

	check_point(6);
	ercd = sta_alm(ALM1, 10);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = twai_sem(SEM1, TMO_FEVR);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(15);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = sta_alm(ALM1, 10);
	check_ercd(ercd, E_OK);

	check_point(31);
	ercd = twai_sem(SEM1, 100);
	check_ercd(ercd, E_RLWAI);

	check_point(33);
	ercd = sta_alm(ALM1, 100);
	check_ercd(ercd, E_OK);

	check_point(34);
	ercd = twai_sem(SEM1, 10);
	check_ercd(ercd, E_TMOUT);

	check_point(35);
	ercd = stp_alm(ALM1);
	check_ercd(ercd, E_OK);

	check_finish(36);

	check_point(0);
}

void
task2(intptr_t exinf)
{
	ER		ercd;

	check_point(24);
	ercd = tslp_tsk(10);
	check_ercd(ercd, E_TMOUT);

	check_point(27);
	ercd = sta_alm(ALM1, 10);
	check_ercd(ercd, E_OK);

	wait_var();

	check_point(29);
	ercd = ext_tsk();

	check_point(0);
}

void
task3(intptr_t exinf)
{
	ER		ercd;

	check_point(13);
	ercd = sta_alm(ALM1, 10);
	check_ercd(ercd, E_OK);

	wait_var();

	check_point(16);
	ercd = sta_alm(ALM1, 10);
	check_ercd(ercd, E_OK);

	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	wait_var();

	check_point(18);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = sta_alm(ALM1, 10);
	check_ercd(ercd, E_OK);

	ercd = sus_tsk(TASK1);
	check_ercd(ercd, E_OK);

	wait_var();

	check_point(22);
	ercd = rsm_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = ext_tsk();

	check_point(0);
}
