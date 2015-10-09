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
 *  @(#) $Id: test_mutex6.c 1696 2010-01-01 16:01:25Z ertl-hiro $
 */

/*
 *			mutex test (6)
 *
 * [test purpose]
 * 
 *  test the related processings for mutex with priority ceiling
 *  protocol when task exits and the task precedence is rotated.
 *
 * [test items]
 *
 *	(A) task exits (ext_tsk)
 *		(A-1) unlock the single mutex locked by the task exits.
 *		(A-2) unlock the single mutex locked by the task exits, and another
 *		task waiting to lock the mutex will lock it with priority
 *		changed, and is released from waiting.
 *		(A-3) unlock the multiple mutexes locked by the task exits.
 *		(A-4) unlock the multiple mutexs locked by the task exits, and other
 *		tasks waiting to lock these mutexes will lock them with priority
 *		changed, and are released from waiting. the earlier task locks the
 *		mutex, the higher the task precedence is.
 *	(B) rotate the task precedence (rot_rdq)
 *		(B-1) if the target is TPRI_SELF, rotate the precedence in the
 *		base priority queue.
 *		(B-2) if the target is TPRI_SELF, the precedence in the	current 
 *		priority queue is not rotated.
 *
 * [how to implement test items]
 *
 *	(A-1)
 *		TASK2 (low priority) locked the MTX1, then exits (ext_tsk),
 *		check if the mutex is unlocked.
 *	(A-2)
 *		TASK2 (low priority) locked the MTX1, TASK3 is waiting to 
 *		lock the MTX1, then TASK2 exits (ext_tsk), check if TASK3 is
 *		released from waiting.
 *	(A-3)
 *		TASK2 (low priority) locked the MTX1 and the MTX2, then exits
 *		(ext_tsk), check if the 2 mutexes are unlocked.
 *	(A-4)
 *	 	TASK2 (low priority) locked the MTX1 and the MTX2 with high
 *	 	ceiling priority, TASK3 and TASK4 (low priority) are waiting to
 *	 	lock these 2 mutexs, then TASK2 exits (ext_tsk), check if TASK3
 *	 	and TASK4 are released from waiting, and check if the precedence
 *	 	of TASK3 is higher than the precedence of TASK4, as TASK3 tries
 *	 	to lock the mutex (MTX1) earlier than TASK4.
 *	(B-1)
 *		the low priority tasks (TASK2, TASK3, TASK4) are runnable, and
 *		TASK2 locked the mutex with high ceiling priority, then
 *		rot_rdq(TPRI_SELF), check if the precedences of TASK3 and TASK4
 *		are changed. 
 *	(B-2)
 *		with the situation in (B-1), check if the precedences of high
 *		priority are not changed.
 *
 * [required resources]
 *
 * TASK1: low priority, main task, 1st task to run
 * TASK2: low priority 
 * TASK3: low priority 
 * TASK4: low priority
 * MTX1: mutex (TA_CEILING, the ceiling priority is high priority)
 * MTX2: mutex (TA_CEILING, the ceiling priority is high priority)
 *
 * [test sequence]
 *
 *	== TASK1(priority: low)==
 *		call(set_bit_func(bit_mutex))
 *	1:	act_tsk(TASK2)
 *	2:	rot_rdq(TPRI_SELF)
 *	//		low: TASK2¢ªTASK1
 *	== TASK2-1(priority: low)first time ==
 *	3:	loc_mtx(MTX1)
 *	//		high: TASK2, low: TASK1, MTX1: TASK2
 *	4:	ext_tsk() -> noreturn			... (A-1)
 *	//		low: TASK1
 *	== TASK1(continuation)==
 *	5:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *
 *	6:	act_tsk(TASK2)
 *		act_tsk(TASK3)
 *	7:	rot_rdq(TPRI_SELF)
 *	//		low: TASK2¢ªTASK3¢ªTASK1
 *	== TASK2-2(priority : low)second time ==
 *	8:	loc_mtx(MTX1)
 *	//		high: TASK2, low: TASK3¢ªTASK1, MTX1: TASK2
 *	9:	slp_tsk()
 *	//		low: TASK3¢ªTASK1, MTX1: TASK2
 *	== TASK3(priority: low)==
 *	10:	loc_mtx(MTX1)
 *	//		low: TASK1, MTX1: TASK2¢ªTASK3
 *	== TASK1(continuation)==
 *	11:	wup_tsk(TASK2)
 *	//		high: TASK2, low: TASK1, MTX1: TASK2¢ªTASK3
 *	== TASK2-2(continuation)==
 *	12:	ext_tsk() -> noreturn			... (A-2)
 *	//		high: TASK3, low: TASK1, MTX1: TASK3
 *	== TASK3(continuation)==
 *	13:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		get_pri(TPRI_SELF, &tskpri)
 *		assert(tskpri == HIGH_PRIORITY)
 *		unl_mtx(MTX1)
 *		slp_tsk()
 *	//		low: TASK1
 *
 *	== TASK1(continuation)==
 *	14:	act_tsk(TASK2)
 *	15:	rot_rdq(TPRI_SELF)
 *	//		low: TASK2¢ªTASK1
 *	== TASK2-3(priority: low)third time ==
 *	16:	loc_mtx(MTX1)
 *		loc_mtx(MTX2)
 *	//		high: TASK2, low: TASK1, MTX1: TASK2, MTX2: TASK2
 *	17:	ext_tsk() -> noreturn			... (A-3)
 *	//		low: TASK1
 *	== TASK1(continuation)==
 *	18:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *
 *	19:	act_tsk(TASK2)
 *		wup_tsk(TASK3)
 *		act_tsk(TASK4)
 *	20:	rot_rdq(TPRI_SELF)
 *	//		low: TASK2¢ªTASK3¢ªTASK4¢ªTASK1
 *	== TASK2-4(priority: low)fourth time ==
 *	21:	loc_mtx(MTX1)
 *		loc_mtx(MTX2)
 *	//		high: TASK2, low: TASK3¢ªTASK4¢ªTASK1, MTX1: TASK2, MTX2: TASK2
 *	22:	chg_pri(TASK1, HIGH_PRIORITY)
 *	//		high: TASK2¢ªTASK1, low: TASK3¢ªTASK4, MTX1: TASK2, MTX2: TASK2
 *	23:	rot_rdq(TPRI_SELF)				... (B-1)(B-2)
 *	//		high: TASK2¢ªTASK1, low: TASK4¢ªTASK3, MTX1: TASK2, MTX2: TASK2
 *	24:	slp_tsk()
 *	//		high: TASK1, low: TASK4¢ªTASK3, MTX1: TASK2, MTX2: TASK2
 *	== TASK1(continuation)==
 *	25:	chg_pri(TSK_SELF, TPRI_INI)
 *	//		low: TASK4¢ªTASK3¢ªTASK1, MTX1: TASK2, MTX2: TASK2
 *	== TASK4(priority: low)==
 *	26:	loc_mtx(MTX2)
 *	//		low: TASK3¢ªTASK1, MTX1: TASK2, MTX2: TASK2¢ªTASK4
 *	== TASK3(continuation)==
 *	27:	loc_mtx(MTX1)
 *	//		low: TASK1, MTX1: TASK2¢ªTASK3, MTX2: TASK2¢ªTASK4
 *	== TASK1(continuation)==
 *	28:	wup_tsk(TASK2)
 *	//		high: TASK2, low: TASK1, MTX1: TASK2¢ªTASK3, MTX2: TASK2¢ªTASK4
 *	== TASK2-4(continuation)==
 *	29:	ext_tsk() -> noreturn			... (A-4)
 *	//		high: TASK3¢ªTASK4, low: TASK1, MTX1: TASK3, MTX2: TASK4
 *	== TASK3(continuation)==
 *	30:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TASK4)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		get_pri(TPRI_SELF, &tskpri)
 *		assert(tskpri == HIGH_PRIORITY)
 *		ext_tsk() -> noreturn
 *	//		high: TASK4, low: TASK1, MTX2: TASK4
 *	== TASK4(continuation)==
 *	31:	get_pri(TPRI_SELF, &tskpri)
 *		assert(tskpri == HIGH_PRIORITY)
 *		ext_tsk() -> noreturn
 *	//		low: TASK1
 *	== TASK1(continuation)==
 *	32:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/logtask.h"
#include "kernel_cfg.h"
#include "test_lib.h"
#include "test_mutex6.h"

extern ER	bit_mutex(void);

void
task1(intptr_t exinf)
{
	ER		ercd;
	T_RMTX	rmtx;

	set_bit_func(bit_mutex);

	check_point(1);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = rot_rdq(TPRI_SELF);
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TSK_NONE);
	check_assert(rmtx.wtskid == TSK_NONE);

	check_point(6);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = rot_rdq(TPRI_SELF);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(15);
	ercd = rot_rdq(TPRI_SELF);
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TSK_NONE);
	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = ref_mtx(MTX2, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TSK_NONE);
	check_assert(rmtx.wtskid == TSK_NONE);

	check_point(19);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = rot_rdq(TPRI_SELF);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = chg_pri(TSK_SELF, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_finish(32);

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
		ercd = ext_tsk();

		check_point(0);

	case 2:
		check_point(8);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(9);
		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(12);
		ercd = ext_tsk();

		check_point(0);

	case 3:
		check_point(16);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(17);
		ercd = ext_tsk();

		check_point(0);

	case 4:
		check_point(21);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(22);
		ercd = chg_pri(TASK1, HIGH_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(23);
		ercd = rot_rdq(TPRI_SELF);
		check_ercd(ercd, E_OK);

		check_point(24);
		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(29);
		ercd = ext_tsk();

		check_point(0);
	}
	check_point(0);
}

void
task3(intptr_t exinf)
{
	ER		ercd;
	PRI		tskpri;
	T_RMTX	rmtx;

	check_point(10);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TASK3);
	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = get_pri(TPRI_SELF, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == HIGH_PRIORITY);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(27);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TASK3);
	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = ref_mtx(MTX2, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TASK4);
	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = get_pri(TPRI_SELF, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == HIGH_PRIORITY);

	ercd = ext_tsk();

	check_point(0);
}

void
task4(intptr_t exinf)
{
	ER		ercd;
	PRI		tskpri;

	check_point(26);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(31);
	ercd = get_pri(TPRI_SELF, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == HIGH_PRIORITY);

	ercd = ext_tsk();

	check_point(0);
}
