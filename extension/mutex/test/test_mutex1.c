/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2007,2008 by Embedded and Real-Time Systems Laboratory
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
 *      (a) The above copyright notice, this use conditions, and the
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
 *  @(#) $Id: test_mutex1.c 1696 2010-01-01 16:01:25Z ertl-hiro $
 */

/*
 *		mutex test (1)
 *
 * [test purpose]
 *
 *  test the functions of lock and unlock the mutex in FIFO order
 *
 * [test items]
 *
 *	(A) lock mutex (loc_mtx)
 *		(A-1) if the mutex is not locked, it can be locked immediately
 *		(A-2) if try to lock the mutex in multiple times, return E_ILUSE error
 *		(A-3) if the mutex is locked,  the task should be waiting in FIFO order
 *	(B) unlock the mutex (unl_mtx)
 *		(B-1) if try to unlock the mutex that is locked by other tasks, 
 *		return E_ILUSE error
 *		(B-2) if there are no tasks waiting,  unlocked the mutx only
 *		(B-3) transfer the mutex to the waitng tasks
 *		(B-4) transfer the mutex to the waitng tasks, make a task
 *		dispatch
 *	(C) lock the mutex without waiting (ploc_mtx)
 *		(C-1) if the mutex is locked, return E_TMOUT error 
 *	(D) lock the mutex with timeout (tloc_mtx)
 *		(D-1) if the mutex is locked, the task gets into the waiting state
 *		with timeout
 *	(E) appendix
 *	 	(E-1) when the task exits, unlock the mutex
 *
 * [required resources]
 *
 *	TASK1: low priority, main task, 1st task to run
 *	TASK2: middle priority
 *	TASK3: high priority
 *	MTX1: mutex (TA_NULL)
 *
 * [test sequence]
 *
 *	== TASK1(priority : low)==
 *		call(set_bit_func(bit_mutex))
 *	1:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		loc_mtx(MTX1)					... (A-1)
 *	2:	loc_mtx(MTX1) -> E_ILUSE		... (A-2)
 *	3:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		act_tsk(TASK2)
 *	== TASK2(priority : middle)==
 *	4:	ploc_mtx(MTX1) -> E_TMOUT		... (C-1)
 *		loc_mtx(MTX1)					... (A-3)
 *	== TASK1(continue)==
 *	5:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TASK2)
 *		act_tsk(TASK3)
 *	== TASK3(priority : high)==
 *	6:	unl_mtx(MTX1) -> E_ILUSE		... (B-1)
 *	7:	loc_mtx(MTX1)					... (A-3)
 *	== TASK1(continue)==
 *	8:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TASK2)
 *		dis_dsp()
 *		unl_mtx(MTX1)					... (B-3)
 *	9:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK2)
 *		assert(rmtx.wtskid == TASK3)
 *		ena_dsp()
 *	== TASK2(continue)==
 *	10:	unl_mtx(MTX1)					... (B-4)
 *	== TASK3(continue)==
 *	11:	ext_tsk() -> noreturn			... (E-1)
 *	== TASK2(continue)==
 *	12:	loc_mtx(MTX1)
 *	13:	slp_tsk()
 *	== TASK1(continue)==
 *	14:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK2)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		tloc_mtx(MTX1, 10) -> E_TMOUT	... (D-1)
 *	15:	wup_tsk(TASK2)
 *	== TASK2(continue)==
 *	16:	unl_mtx(MTX1)					... (B-2)
 *	17:	ext_tsk() -> noreturn
 *	== TASK1(continue)==
 *	18:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/logtask.h"
#include "kernel_cfg.h"
#include "test_lib.h"
#include "test_mutex.h"

extern ER	bit_mutex(void);

void
task1(intptr_t exinf)
{
	ER		ercd;
	T_RMTX	rmtx;

	set_bit_func(bit_mutex);

	check_point(1);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TSK_NONE);
	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_ILUSE);

	check_point(3);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TASK1);
	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TASK1);
	check_assert(rmtx.wtskid == TASK2);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TASK1);
	check_assert(rmtx.wtskid == TASK2);

	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TASK2);
	check_assert(rmtx.wtskid == TASK3);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);
	check_assert(rmtx.htskid == TASK2);
	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = tloc_mtx(MTX1, 10);
	check_ercd(ercd, E_TMOUT);

	check_point(15);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_finish(18);

	check_point(0);
}

void
task2(intptr_t exinf)
{
	ER		ercd;

	check_point(4);
	ercd = ploc_mtx(MTX1);
	check_ercd(ercd, E_TMOUT);

	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(10);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = ext_tsk();

	check_point(0);
}

void
task3(intptr_t exinf)
{
	ER		ercd;

	check_point(6);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_ILUSE);

	check_point(7);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = ext_tsk();

	check_point(0);
}
