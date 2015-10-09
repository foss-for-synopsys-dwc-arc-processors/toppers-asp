/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2008-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: test_mutex7.c 1696 2010-01-01 16:01:25Z ertl-hiro $
 */

/*
 *		test of the mutex(7)
 *
 * ¡Úthe purpose of the test¡Û
 *
 *  test the related processings for mutex with priority ceiling
 *  protocol when task is terminated
 *
 * [test items]
 *
 *	(A) task is terminated (ter_tsk)
 *		(A-1) unlock the single mutex locked by the terminated task.
 *		(A-2) unlock the single mutex locked by the terminated task,
 *		and another task waiting to lock the mutex will lock it with
 *		priority changed, and is released from waiting.
 *		(A-3) the task dispatch in (A-2).
 *		(A-4) unlock the multiple mutexes locked by the terminated task.
 *		(A-5) unlock the multiple mutexs locked by the terminated task,
 *		and other tasks waiting to lock these mutexes will lock them with
 *		priority changed, and are released from waiting. the earlier task
 *		locks the mutex, the higher the task precedence is.
 *		(A-6) the task dispatch in (A-5)
 *
 * [how to implement test items]
 *
 *	(A-1)
 *		TASK2 (low priority) locked the MTX1, then TASK1 terminates
 *		TASK2 (ter_tsk), check if the mutex is unlocked.
 *	(A-2)
 *		TASK2 (low priority) locked the MTX1 with middle ceiling
 *		priority, TASK3 (low priority) is waiting to lock the MTX1
 *		, then TASK1 (high priority) terminates TASK2 (ter_tsk), 
 *		check if TASK3 is released from waiting and its priority is
 *		raised to middle.
 *	(A-3)
 *		TASK2 (low priority) locked the MTX1 with middle ceiling
 *		priority, TASK3 (low priority) is waiting to lock the MTX1
 *		, then TASK1 (high priority) terminates TASK2 (ter_tsk), 
 *		check if TASK3 is released from waiting, its priority is
 *		raised to middle, and TASK3 is dispatched.
 *	(A-4)
 *		TASK2 (low priority) locked the MTX1 and the MTX2, then 
 *		TASK1 terminates TASK2 (ter_tsk), check if the 2 mutexes
 *		are unlocked.
 *	(A-5)
 *	 	TASK2 (low priority) locked the MTX1 and the MTX2 with middle
 *	 	ceiling priority, TASK3 and TASK4 (low priority) are waiting to
 *	 	lock these 2 mutexs, then TASK1 (high priority) terminates TASK1
 *	 	(ter_tsk), check if TASK3 and TASK4 are released from waiting
 *	 	with priority raised, and check if the precedence of TASK3 is
 *	 	higher than the precedence of TASK4, as TASK3 tries to lock the
 *	 	mutex (MTX1) earlier than TASK4.
 *	(A-6)
 *	 	TASK2 (low priority) locked the MTX1 and the MTX2 with middle
 *	 	ceiling priority, TASK3 and TASK4 (low priority) are waiting to
 *	 	lock these 2 mutexs, then TASK1 (high priority) terminates TASK1
 *	 	(ter_tsk), check if TASK3 and TASK4 are released from waiting
 *	 	with priority raised, and check if TASK3 is dispatched.
 *
 * [required resources]
 *
 * TASK1: high priority, main task, 1st task to run
 * TASK2: low priority 
 * TASK3: low priority 
 * TASK4: low priority
 * MTX1: mutex (TA_CEILING, the ceiling priority is middle priority)
 * MTX2: mutex (TA_CEILING, the ceiling priority is middle priority)
 *
 * [test sequence]
 *
 *	== TASK1(priority:high)==
 *		call(set_bit_func(bit_mutex))
 *	1:	act_tsk(TASK2)
 *	2:	slp_tsk()
 *	== TASK2-1(priority: low)==
 *	3:	loc_mtx(MTX1)
 *	4:	wup_tsk(TASK1)
 *	== TASK1(continue)==
 *	5:	ter_tsk(TASK2)			... (A-1)
 *	6:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *
 *	7:	act_tsk(TASK2)
 *		act_tsk(TASK3)
 *	8:	slp_tsk()
 *	== TASK2-2(priority: low)==
 *	9:	loc_mtx(MTX1)
 *	10:	tslp_tsk(10) -> E_TMOUT
 *	== TASK3-1(priority: low)==
 *	11:	loc_mtx(MTX1)
 *	== TASK2-2(continue)==
 *	12:	wup_tsk(TASK1)
 *	== TASK1(continue)==
 *	13:	ter_tsk(TASK2)			... (A-2)
 *	14:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		get_pri(TASK3, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	15:	ter_tsk(TASK3)
 *
 *	16:	act_tsk(TASK2)
 *		act_tsk(TASK3)
 *	17:	chg_pri(TSK_SELF, LOW_PRIORITY)
 *	== TASK2-3(priority: low)==
 *	18:	loc_mtx(MTX1)
 *	19:	slp_tsk()
 *	== TASK3-2(priority: low)==
 *	20:	loc_mtx(MTX1)
 *	== TASK1(continue)==
 *	21:	ter_tsk(TASK2)			... (A-3)
 *	== TASK3-2(continue)==
 *	22:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		get_pri(TASK3, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	23:	ext_tsk() -> noreturn
 *	== TASK1(continue)==
 *	24:	chg_pri(TSK_SELF, HIGH_PRIORITY)
 *
 *	25:	act_tsk(TASK2)
 *	26:	slp_tsk()
 *	== TASK2-4(priority: low)==
 *	27:	loc_mtx(MTX1)
 *		loc_mtx(MTX2)
 *	28:	wup_tsk(TASK1)
 *	== TASK1(continue)==
 *	29:	ter_tsk(TASK2)			... (A-4)
 *	30:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *	31:	MISSING
 *
 *	32:	act_tsk(TASK2)
 *		act_tsk(TASK3)
 *		act_tsk(TASK4)
 *	33:	slp_tsk()
 *	== TASK2-5(priority: low)==
 *	34:	loc_mtx(MTX1)
 *		loc_mtx(MTX2)
 *	35:	tslp_tsk(10) -> E_TMOUT
 *	== TASK3-3(priority: low)==
 *	36:	loc_mtx(MTX1)
 *	== TASK4-1(priority: low)==
 *	37:	loc_mtx(MTX2)
 *	== TASK2-5(continue)==
 *	38:	wup_tsk(TASK1)
 *	== TASK1(continue)==
 *	39:	ter_tsk(TASK2)			... (A-5)
 *	40:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TASK4)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		get_pri(TASK3, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *		get_pri(TASK4, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	41:	chg_pri(TSK_SELF, LOW_PRIORITY)
 *	== TASK3-3(continue)==
 *	42:	ext_tsk() -> noreturn
 *	== TASK4-1(continue)==
 *	43:	ext_tsk() -> noreturn
 *	== TASK1(continue)==
 *	44:	act_tsk(TASK2)
 *		act_tsk(TASK3)
 *		act_tsk(TASK4)
 *	45:	chg_pri(TSK_SELF, LOW_PRIORITY)
 *	== TASK2-6(priority: low)==
 *	46:	loc_mtx(MTX1)
 *		loc_mtx(MTX2)
 *	47:	slp_tsk()
 *	== TASK3-4(priority: low)==
 *	48:	loc_mtx(MTX1)
 *	== TASK4-2(priority: low)==
 *	49:	loc_mtx(MTX2)
 *	== TASK1(continue)==
 *	50:	ter_tsk(TASK2)			... (A-6)
 *	== TASK3-4(continue)==
 *	51:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TASK4)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		get_pri(TASK3, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *		get_pri(TASK4, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	52:	ext_tsk() -> noreturn
 *	== TASK4-2(continue)==
 *	53:	ext_tsk() -> noreturn
 *	== TASK1(continue)==
 *	54:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/logtask.h"
#include "kernel_cfg.h"
#include "test_lib.h"
#include "test_mutex7.h"

extern ER	bit_mutex(void);

void
task1(intptr_t exinf)
{
	ER		ercd;
	PRI		tskpri;
	T_RMTX	rmtx;

	set_bit_func(bit_mutex);

	check_point(1);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = ter_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TSK_NONE);
	check_assert(rmtx.wtskid == TSK_NONE);

	check_point(7);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = ter_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TASK3);
	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = get_pri(TASK3, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(15);
	ercd = ter_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = chg_pri(TSK_SELF, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = ter_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(24);
	ercd = chg_pri(TSK_SELF, HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = ter_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TSK_NONE);
	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = ref_mtx(MTX2, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TSK_NONE);
	check_assert(rmtx.wtskid == TSK_NONE);

	check_point(31);

	check_point(32);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(39);
	ercd = ter_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(40);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TASK3);
	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = ref_mtx(MTX2, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TASK4);
	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = get_pri(TASK3, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	ercd = get_pri(TASK4, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(41);
	ercd = chg_pri(TSK_SELF, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(44);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(45);
	ercd = chg_pri(TSK_SELF, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(50);
	ercd = ter_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_finish(54);

	check_point(0);
}

static uint_t	task2_count = 0;

void
task2(intptr_t exinf)
{
	ER		ercd;

	switch (++task2_count) {
	case 1:
		check_point(3);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(4);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(9);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(10);
		ercd = tslp_tsk(10);
		check_ercd(ercd, E_TMOUT);

		check_point(12);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(0);

	case 3:
		check_point(18);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(19);
		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 4:
		check_point(27);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(28);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(0);

	case 5:
		check_point(34);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(35);
		ercd = tslp_tsk(10);
		check_ercd(ercd, E_TMOUT);

		check_point(38);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(0);

	case 6:
		check_point(46);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(47);
		ercd = slp_tsk();
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
	PRI		tskpri;
	T_RMTX	rmtx;

	switch (++task3_count) {
	case 1:
		check_point(11);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(20);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(22);
		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);
		check_assert(rmtx.htskid == TASK3);
		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = get_pri(TASK3, &tskpri);
		check_ercd(ercd, E_OK);
		check_assert(tskpri == MID_PRIORITY);

		check_point(23);
		ercd = ext_tsk();

		check_point(0);

	case 3:
		check_point(36);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(42);
		ercd = ext_tsk();

		check_point(0);

	case 4:
		check_point(48);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(51);
		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);
		check_assert(rmtx.htskid == TASK3);
		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_mtx(MTX2, &rmtx);
		check_ercd(ercd, E_OK);
		check_assert(rmtx.htskid == TASK4);
		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = get_pri(TASK3, &tskpri);
		check_ercd(ercd, E_OK);
		check_assert(tskpri == MID_PRIORITY);

		ercd = get_pri(TASK4, &tskpri);
		check_ercd(ercd, E_OK);
		check_assert(tskpri == MID_PRIORITY);

		check_point(52);
		ercd = ext_tsk();

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
		check_point(37);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(43);
		ercd = ext_tsk();

		check_point(0);

	case 2:
		check_point(49);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(53);
		ercd = ext_tsk();

		check_point(0);
	}
	check_point(0);
}
