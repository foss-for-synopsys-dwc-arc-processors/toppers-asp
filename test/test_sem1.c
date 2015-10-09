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
 *  @(#) $Id: test_sem1.c 1577 2009-05-31 14:30:51Z ertl-hiro $
 */

/* 
 *		semaphore test (1)
 *
 * [test purpose]
 *
 *  unit test of sig_sem, wai_sem and CRE_SEM. 
 *  But, the error test of CRE_SEM is excluded.
 *
 * [test items]
 *
 *	(A) static error test of sig_sem
 *		(A-1) called in non-task context
 *		(A-2) called when CPU is locked
 *		(A-3) semid is invalid (too small)
 *		(A-4) semid is invalid (too large)
 *	(B) release the waiting task through sig_sem
 *		(B-1) dispatch to the released task
 *		(B-2) no task dispatch when dispatch is not allowed
 *		(B-3) no task dispatch when the released task is suspended
 *		(B-4) no task dispatch when the released task's priority is
 *		lower
 *	(C) increase the semaphore through sig_sem
 *		(C-1) the semcnt from 0 to 1
 *		(C-2) the semcnt from 1 to 2
 *	(D) the E_QOVR error of sig_sem
 *		(D-1) when the max semcnt is 1
 *		(D-2) when the max semcnt is 2
 *	(E) static error test of wai_sem
 *		(E-1) called in non-task context
 *		(E-2) called when CPU is locked
 *		(E-3) called when task dispatch is disabled
 *		(E-4) called when the interrupt priority mask is not cleared
 *		(E-5) semid is invalid (too small)
 *		(E-6) semid is invalid (too large)
 *	(F) decrease the semaphore through wai_sem
 *		(F-1) the semcnt from 1 to 0
 *		(F-2) the semcnt from 2 to 1
 *	(G) task gets into the waiting state through wai_sem
 *		(G-1) for the attribute is TA_TNULL, without tasks waiting
 *		(G-2) for the attribute is TA_TNULL, with tasks waiting 
 *		(G-3) for the attribute is TA_TPRI, without tasks waiting 
 *		(G-4) for the attribute is TA_TPRI, with high priority 
 *		tasks waiting 
 *		(G-5)  for the attribute is TA_TPRI, with same priority
 *		tasks waiting
 *		(G-6) for the attribute is TA_TPRI, with lower priority
 *		tasks waiting 
 *	(H) forcibly release the waiting task
 *	(I) initialize the semaphore when it is waited by some tass
 *	(J) set the semcnt to the init value 
 *		(J-1) the init value of semcnt is 0
 *		(J-2) the init value of semcnt is 1
 *		(J-3) the init value of semcnt is 2
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
 *  SEM2:  TA_NULL, the init semcnt is 2, teh max semcnt is 2
 *  SEM3:  TA_TPRI, the init semcnt is 0, teh max semcnt is 1
 *
 * [test sequence]
 *
 *	== TASK1(priority: middle)==
 *  1:	ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 1)			... (J-2)
 *  	ref_sem(SEM2, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 2)			... (J-3)
 *  	ref_sem(SEM3, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 0)			... (J-1)
 *	2:	loc_cpu()
 *		sig_sem(SEM1) -> E_CTX				... (A-2)
 *		wai_sem(SEM1) -> E_CTX				... (E-2)
 *		unl_cpu()
 *		dis_dsp()
 *		wai_sem(SEM1) -> E_CTX				... (E-3)
 *		ena_dsp()
 *		chg_ipm(TMAX_INTPRI)
 *		wai_sem(SEM1) -> E_CTX				... (E-4)
 *		chg_ipm(TIPM_ENAALL)
 *		sig_sem(0) -> E_ID					... (A-3)
 *		wai_sem(0) -> E_ID					... (E-5)
 *		sig_sem(TNUM_SEMID+1) -> E_ID		... (A-4)
 *		wai_sem(TNUM_SEMID+1) -> E_ID		... (E-6)
 *	3:	act_tsk(TASK3)
 *	4:	slp_tsk()
 *	== TASK3(priority: low)==
 *	5:	wai_sem(SEM1)						... (F-1)
 *  6:	ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 0)
 *	7:	sta_alm(ALM1, 10)
 *	8:	wai_sem(SEM1)						... (G-1)
 *	== ALM1 ==
 *	9:	sig_sem(SEM1) -> E_CTX				... (A-1)
 *		wai_sem(SEM1) -> E_CTX				... (E-1)
 *	10:	iwup_tsk(TASK1)
 *	11:	RETURN
 *	== TASK1(continue)==
 *	12:	act_tsk(TASK2)
 *	== TASK2(priority:high)==
 *	13:	wai_sem(SEM1)						... (G-2)
 *	== TASK1(continue)==
 *  14:	ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TASK3)
 *		assert(rsem.semcnt == 0)
 *	15:	sig_sem(SEM1)						... (B-4)
 *	16:	sig_sem(SEM1)						... (B-1)
 *	== TASK2(continue)==
 *	17:	wai_sem(SEM1)						... (G-1)
 *	== TASK1(continue)==
 *	18: dis_dsp()
 *	19:	sig_sem(SEM1)						... (B-2)
 *	20:	ena_dsp()
 *	== TASK2(continue)==
 *	21:	wai_sem(SEM1)						... (G-1)
 *	== TASK1(continue)==
 *	22: sus_tsk(TASK2)
 *	23:	sig_sem(SEM1)						... (B-3)
 *	24: sig_sem(SEM1)						... (C-1)
 *	25: sig_sem(SEM1) -> E_QOVR				... (D-1)
 *  26:	ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 1)
 *	27:	rsm_tsk(TASK2)
 *	== TASK2(continue)==
 *	28:	wai_sem(SEM2)						... (F-2)
 *  29:	ref_sem(SEM2, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 1)
 *	30:	wai_sem(SEM2)						... (F-1)
 *	31: wai_sem(SEM2)						... (G-1)
 *	== TASK1(continue)==
 *	32:	sig_sem(SEM2)						... (B-1)
 *	== TASK2(continue)==
 *	33:	wai_sem(SEM3)						... (G-3)
 *	== TASK1(continue)==
 *	34:	sig_sem(SEM2)						... (C-1)
 *	35:	sig_sem(SEM2)						... (C-2)
 *  36:	ref_sem(SEM2, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 2)
 *	37:	sig_sem(SEM2) -> E_QOVR				... (D-2)
 *  38:	ref_sem(SEM2, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 2)
 *	39:	MISSING
 *	40:	tslp_tsk(10) -> E_TMOUT
 *	== TASK3(continue)==
 *	41:	wai_sem(SEM3)						... (G-4)
 *	== TASK1(continue)==
 *	42:	act_tsk(TASK4)
 *	43:	act_tsk(TASK5)
 *	44:	rot_rdq(TPRI_SELF)
 *	== TASK4(priority: middle)==
 *	45:	wai_sem(SEM3)						... (G-6)
 *	== TASK5(priority: middle)==
 *	46:	wai_sem(SEM3)						... (G-5)
 *	== TASK1(continue)==
 *	47:	sig_sem(SEM3)						... (B-1)
 *	== TASK2(continue)==
 *	48:	wai_sem(SEM1)
 *		wai_sem(SEM1) -> E_RLWAI
 *	== TASK1(continue)==
 *	49:	sig_sem(SEM3)						... (B-4)
 *	50:	tslp_tsk(10) -> E_TMOUT
 *	== TASK4(continue)==
 *	51:	ext_tsk() -> noreturn
 *	== TASK1(continue)==
 *	52:	sig_sem(SEM3)						... (B-4)
 *	53:	tslp_tsk(10) -> E_TMOUT
 *	== TASK5(continue)==
 *	54:	ext_tsk() -> noreturn
 *	== TASK1(continue)==
 *	55:	sig_sem(SEM3)						... (B-4)
 *	56:	tslp_tsk(10) -> E_TMOUT
 *	== TASK3(continue)==
 *	57:	ext_tsk() -> noreturn
 *	== TASK1(continue)==
 *	58: rel_wai(TASK2)						... (H)
 *	== TASK2(continue)==
 *	59:	wai_sem(SEM1) -> E_DLT
 *	== TASK1(continue)==
 *	60: ini_sem(SEM1)						... (I)
 *	== TASK2(continue)==
 *	61: ext_tsk() -> noreturn
 *	== TASK1(continue)==
 *	62: END
 */

#include <kernel.h>
#include <test_lib.h>
#include <t_syslog.h>
#include "kernel_cfg.h"
#include "test_sem1.h"

void
alarm1_handler(intptr_t exinf)
{
	ER		ercd;

	check_point(9);
	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_CTX);

	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_CTX);

	check_point(10);
	ercd = iwup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(11);
	return;

	check_point(0);
}

void
task1(intptr_t exinf)
{
	ER		ercd;
	T_RSEM	rsem;

	check_point(1);
	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);
	check_assert(rsem.wtskid == TSK_NONE);
	check_assert(rsem.semcnt == 1);

	ercd = ref_sem(SEM2, &rsem);
	check_ercd(ercd, E_OK);
	check_assert(rsem.wtskid == TSK_NONE);
	check_assert(rsem.semcnt == 2);

	ercd = ref_sem(SEM3, &rsem);
	check_ercd(ercd, E_OK);
	check_assert(rsem.wtskid == TSK_NONE);
	check_assert(rsem.semcnt == 0);

	check_point(2);
	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_CTX);

	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_CTX);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	ercd = chg_ipm(TMAX_INTPRI);
	check_ercd(ercd, E_OK);

	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_CTX);

	ercd = chg_ipm(TIPM_ENAALL);
	check_ercd(ercd, E_OK);

	ercd = sig_sem(0);
	check_ercd(ercd, E_ID);

	ercd = wai_sem(0);
	check_ercd(ercd, E_ID);

	ercd = sig_sem(TNUM_SEMID+1);
	check_ercd(ercd, E_ID);

	ercd = wai_sem(TNUM_SEMID+1);
	check_ercd(ercd, E_ID);

	check_point(3);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);
	check_assert(rsem.wtskid == TASK3);
	check_assert(rsem.semcnt == 0);

	check_point(15);
	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = sus_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(24);
	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_QOVR);

	check_point(26);
	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);
	check_assert(rsem.wtskid == TSK_NONE);
	check_assert(rsem.semcnt == 1);

	check_point(27);
	ercd = rsm_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(32);
	ercd = sig_sem(SEM2);
	check_ercd(ercd, E_OK);

	check_point(34);
	ercd = sig_sem(SEM2);
	check_ercd(ercd, E_OK);

	check_point(35);
	ercd = sig_sem(SEM2);
	check_ercd(ercd, E_OK);

	check_point(36);
	ercd = ref_sem(SEM2, &rsem);
	check_ercd(ercd, E_OK);
	check_assert(rsem.wtskid == TSK_NONE);
	check_assert(rsem.semcnt == 2);

	check_point(37);
	ercd = sig_sem(SEM2);
	check_ercd(ercd, E_QOVR);

	check_point(38);
	ercd = ref_sem(SEM2, &rsem);
	check_ercd(ercd, E_OK);
	check_assert(rsem.wtskid == TSK_NONE);
	check_assert(rsem.semcnt == 2);

	check_point(39);

	check_point(40);
	ercd = tslp_tsk(10);
	check_ercd(ercd, E_TMOUT);

	check_point(42);
	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(43);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(44);
	ercd = rot_rdq(TPRI_SELF);
	check_ercd(ercd, E_OK);

	check_point(47);
	ercd = sig_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(49);
	ercd = sig_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(50);
	ercd = tslp_tsk(10);
	check_ercd(ercd, E_TMOUT);

	check_point(52);
	ercd = sig_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(53);
	ercd = tslp_tsk(10);
	check_ercd(ercd, E_TMOUT);

	check_point(55);
	ercd = sig_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(56);
	ercd = tslp_tsk(10);
	check_ercd(ercd, E_TMOUT);

	check_point(58);
	ercd = rel_wai(TASK2);
	check_ercd(ercd, E_OK);

	check_point(60);
	ercd = ini_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_finish(62);

	check_point(0);
}

void
task2(intptr_t exinf)
{
	ER		ercd;
	T_RSEM	rsem;

	check_point(13);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = wai_sem(SEM2);
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = ref_sem(SEM2, &rsem);
	check_ercd(ercd, E_OK);
	check_assert(rsem.wtskid == TSK_NONE);
	check_assert(rsem.semcnt == 1);

	check_point(30);
	ercd = wai_sem(SEM2);
	check_ercd(ercd, E_OK);

	check_point(31);
	ercd = wai_sem(SEM2);
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = wai_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(48);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_RLWAI);

	check_point(59);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_DLT);

	check_point(61);
	ercd = ext_tsk();

	check_point(0);
}

void
task3(intptr_t exinf)
{
	ER		ercd;
	T_RSEM	rsem;

	check_point(5);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);
	check_assert(rsem.wtskid == TSK_NONE);
	check_assert(rsem.semcnt == 0);

	check_point(7);
	ercd = sta_alm(ALM1, 10);
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(41);
	ercd = wai_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(57);
	ercd = ext_tsk();

	check_point(0);
}

void
task4(intptr_t exinf)
{
	ER		ercd;

	check_point(45);
	ercd = wai_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(51);
	ercd = ext_tsk();

	check_point(0);
}

void
task5(intptr_t exinf)
{
	ER		ercd;

	check_point(46);
	ercd = wai_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(54);
	ercd = ext_tsk();

	check_point(0);
}
