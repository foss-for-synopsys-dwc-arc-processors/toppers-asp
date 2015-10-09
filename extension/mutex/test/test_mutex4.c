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
 *  @(#) $Id: test_mutex4.c 1696 2010-01-01 16:01:25Z ertl-hiro $
 */

/*
 *		mutex test (4)
 *
 * [test purpose]
 *
 *  test the change of task priority when loc_mtx
 *  and unl_mtx are called for the mutex with priority ceiling protocol.
 *  change_priority and mutex_calc_priority are not fully covered.
 *
 * [test items]
 *
 *	(A) the change of task priority when loc_mtx is called
 *		(A-1) if the ceiling priority of the locked mutex is higher than
 *		the current priority of calling task, raise the priority to ceiling 
 *		priority. the precedence of the calling task is the highest in the
 *		priority queue. 
 *		(A-2) if the ceiling priority of the locked mutex is same as
 *		the current priority of calling task, the task priority is not
 *		changed. the precedence of the calling task in the priority
 *		queue is also not changed.
 *		(A-3) if the ceiling priority of the locked mutex is lower than
 *		the current priority of calling task, the task priority is not
 *		changed. the precedence of the calling task in the priority
 *		queue is also not changed.
 *	(B) the change of task priority when unl_mtx is called
 *		(B-1) if the ceiling priority of unlocked mutex is the same as
 *		the current priority of calling task, for the situation that
 *		priority should be lowerd, lower the task priority.the precedence 
 *		of the calling task is the highest in the priority queue. 
 *		(B-2) if the ceiling priority of unlocked mutex is the same as
 *		the current priority of calling task, for the situation that
 *		priority should not be changed, the priority is not changed.
 *		the precedence of the calling task in the priority queue is also
 *		 not changed.
 *		(B-3)  if the ceiling priority of unlocked mutex is lower than 
 *		the current priority of calling task, the priority is not changed.
 *		the precedence of the calling task in the priority queue is also
 *		not changed.
 *	(C) the change of task priority (the task locked the mutex) when 
 *		unl_mtx is called
 *		(C-1) if the ceiling priority of the locked mutex is higher than
 *		the current priority of new task that locked the mutex, raise the
 *		priority to ceiling priority. the precedence of the calling task 
 *		is the lowest in the priority queue. 
 *		(C-2) if the ceiling priority of the locked mutex is the same as
 *		the current priority of new task that locked the mutex, the task
 *		priority is not changed. the precedence of the calling task 
 *		is the lowest in the priority queue. 
 *		(C-3) if the ceiling priority of the locked mutex is lower than
 *		the current priority of new task that locked the mutex, the task
 *		priority is not changed. the precedence of the calling task 
 *		is the lowest in the priority queue. 
 *	(D) the E_ILUSE error of loc_mtx
 *		(D-1) if the base priority of calling task is higher than the
 *		ceiling priority of the target mutex, return E_ILUSE error. 
 *		(D-2) if the base priority of calling task is lower than or the
 *		same as the ceiling priority of the target mutex, but the current
 *		priority is higher than the ceiling priority, do not return 
 *		E_ILUSE error. 
 *
 * [how to implement test items]
 *
 *	(A-1)
 *		TASK1 (middle priority) locked the MTX2 with high ceiling priority,
 *		check if the priority of TASK1 is raised, and check if the
 *		precedence of TASK1 is higher than the precedence of TASK4 with
 *		high priority. the checks are done with task dispatch disabled.
 *	(A-2)
 *	 	TASK1 (middle priority) locked the MTX1 with middle ceiling priority,
 *		check if the priority of TASK1 is not changed, and check the
 *		precedence of TASK1 is not changed with 2 middle priority tasks (TASK2, 
 *		TASK3) prepared. the checks are done with task dispatch disabled.
 *	(A-3)
 *		TASK1 (middle priority) locked the MTX2 with high ceiling priority, 
 *		and the priority is raised. Then TASK1 locked the MTX1 with middle
 *		ceiling priority, check if the priority of TASK1 is not changed, 
 *		and check the precedence of TASK1 is not changed with 2 high 
 *		priority tasks (TASK4, TASK5) prepared. the checks are done with
 *		task dispatch disabled.
 *	(B-1)
 * 		TASK1 (middle priority) only locked the MTX2 with high ceiling 
 * 		priority, then unlock MTX2, check if the priority is lowerd
 * 		back to middle, and check the precedence of TASK1 is higher than
 * 		the precedence of TASK2 with middle priority. the checks are done
 * 		with task dispatch disabled.
 *	(B-2)
*		TASK1 (middle priority) only locked the MTX1 with middle ceiling 
 * 		priority, then unlock MTX1, check if the priority of TASK1 is not
 * 		changed, and check the precedence of TASK1 is not changed with 2
 * 		middle priority tasks (TASK2, TASK3) prepared. the checks are done
 * 		with task dispatch disabled.
 *	(B-3)
 *	  	TASK1 (middle priority) locked the MTX2 with high ceiling priority
 *	  	and the MTX1 with middle ceiling priority, then unlock MTX1, check
 *	  	if the priority of TASK1 is not changed, and check the precedence
 *	  	of TASK1 is not changed with 2 high priority tasks (TASK4, TASK5)
 *	  	prepared. the checks are done with task dispatch disabled. 
 *	(C-1)
 *		TASK1 (middle priority) locked the MTX2 and MTX3 with high ceiling
 *		priority, TASK2 (middle priority) is waiting for MTX2. then,
 *		task1 unlock MTX2, check if the priority of TASK2 is raised, and
 *		check the precedence of TASK2 is lower than the precedence of TASK1 
 *	(C-2)
 *		TASK2 (middle priority) locked the MTX1 with middle ceiling priority,
 *		TASK1 (middle priority) is waiting for MTX1. the TASK2 unlock
 *		MTX1, check if the priority of TASK1 is not changed, and check the 
 *		precedence of TASK1 is lower than the precedence of TASK3
 *		(middle priority)
 *	(C-3)
 *		TASK1 (middle priority) locked the MTX1 with middle ceiling priority
 *	  	and the MTX3 with high ceiling priority, TASK2 (middle priority) 
 *	  	locked MTX2 and is waiting for MTX1, then TASK1 unlock MTX1,
 *	  	check if the priority of TASK2 is not changed, and check the 
 *		precedence of TASK2 is lower than the precedence of TASK1
 *	(D-1)
 *		TASK4 (high priority) tries to lock MTX1 with middle ceiling
 *		priority, check E_ILUSE error is returned
 *	(D-2)
 *		TASK1 (middle priority) locked MTX2 with high ceiling priority,
 *		tries to lock MTX1 with middle ceiling priority, check E_ILUSE 
 *		error is not returned
 *
 * [required resources]
 *
 *	TASK1: middle priority, main task, 1st task to run
 *	TASK2: middle priority 
 *	TASK3: middle priority 
 *	TASK4: high priority
 *	TASK5: high priority
 *	MTX1: mutex (TA_CEILING, the ceiling priority is middle priority)
 *	MTX2: mutex (TA_CEILING, the ceiling priority is high priority)
 *	MTX3: mutex (TA_CEILING, the ceiling priority is high priority)
 *
 * [test sequence]
 *
 *	== TASK1(priority: middle)==
 *		call(set_bit_func(bit_mutex))
 *	1:	dis_dsp()
 *	2:	act_tsk(TASK2)
 *		act_tsk(TASK4)
 *	//		high: TASK4, middle: TASK1->TASK2
 *	3:	ploc_mtx(MTX2)								... (A-1)
 *	//		high: TASK1->TASK4, middle: TASK2, MTX2：TASK1
 *		get_pri(TSK_SELF, &tskpri)
 *		assert(tskpri == HIGH_PRIORITY)
 *	4:	ena_dsp()
 *	5:	unl_mtx(MTX2)								... (B-1)
 *	//		high: TASK4, middle: TASK1->TASK2
 *	== TASK4(priority: high)==
 *	6:	loc_mtx(MTX1) -> E_ILUSE					... (D-1)
 *	7:	slp_tsk()
 *	//		middle: TASK1->TASK2
 *	== TASK1(continue)==
 *	8:	get_pri(TSK_SELF, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *
 *	9:	dis_dsp()
 *	10:	rot_rdq(MID_PRIORITY)
 *	//		middle: TASK2->TASK1
 *	11:	act_tsk(TASK3)
 *	//		middle: TASK2->TASK1->TASK3
 *	12:	ploc_mtx(MTX1)								... (A-2)
 *	//		middle: TASK2->TASK1->TASK3, MTX1：TASK1
 *		get_pri(TSK_SELF, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	13:	ena_dsp()
 *	== TASK2(priority: middle)==
 *	14:	slp_tsk()
 *	//		middle: TASK1->TASK3, MTX1：TASK1
 *	== TASK1(continue)==
 *	15:	slp_tsk()
 *	//		middle: TASK3, MTX1：TASK1
 *	== TASK3(priority: middle)==
 *	16:	wup_tsk(TASK1)
 *	//		middle: TASK3->TASK1, MTX1：TASK1
 *	17:	slp_tsk()
 *	//		middle: TASK1, MTX1：TASK1
 *
 *	== TASK1(continue)==
 *	18:	wup_tsk(TASK2)
 *	//		middle: TASK1->TASK2, MTX1：TASK1
 *	19:	dis_dsp()
 *	20:	rot_rdq(MID_PRIORITY)
 *	//		middle: TASK2->TASK1, MTX1：TASK1
 *	21:	wup_tsk(TASK3)
 *	//		middle: TASK2->TASK1->TASK3, MTX1：TASK1
 *	22:	unl_mtx(MTX1)								... (B-2)
 *	//		middle: TASK2->TASK1->TASK3
 *		get_pri(TSK_SELF, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	23:	ena_dsp()
 *	== TASK2(priority: middle)==
 *	24:	slp_tsk()
 *	//		middle: TASK1->TASK3
 *	== TASK1(continue)==
 *	25:	slp_tsk()
 *	//		middle: TASK3
 *	== TASK3(priority: middle)==
 *	26:	wup_tsk(TASK1)
 *	//		middle: TASK3->TASK1
 *	27:	slp_tsk()
 *	//		middle: TASK1
 *
 *	== TASK1(continue)==
 *	28:	dis_dsp()
 *	29:	wup_tsk(TASK4)
 *	//		high: TASK4, middle: TASK1
 *	30:	ploc_mtx(MTX2)
 *	//		high: TASK1->TASK4, MTX2：TASK1
 *	31:	rot_rdq(HIGH_PRIORITY)
 *	//		high: TASK4->TASK1, MTX2：TASK1
 *	32:	act_tsk(TASK5)
 *	//		high: TASK4->TASK1->TASK5, MTX2：TASK1
 *	33:	ploc_mtx(MTX1)								... (A-3)(D-2)
 *	//		high: TASK4->TASK1->TASK5, MTX2：TASK1, MTX1：TASK1
 *		get_pri(TSK_SELF, &tskpri)
 *		assert(tskpri == HIGH_PRIORITY)
 *	34:	ena_dsp()
 *	== TASK4(continue)==
 *	35:	slp_tsk()
 *	//		high: TASK1->TASK5, MTX2：TASK1, MTX1：TASK1
 *	== TASK1(continue)==
 *	36:	slp_tsk()
 *	//		high: TASK5, MTX2：TASK1, MTX1：TASK1
 *	== TASK5(priority: high)==
 *	37:	wup_tsk(TASK1)
 *	//		high: TASK5->TASK1, MTX2：TASK1, MTX1：TASK1
 *	38:	slp_tsk()
 *	//		high: TASK1, MTX2：TASK1, MTX1：TASK1
 *
 *	== TASK1(continue)==
 *	39:	dis_dsp()
 *	40:	wup_tsk(TASK4)
 *	//		high: TASK1->TASK4, MTX2：TASK1, MTX1：TASK1
 *	41:	rot_rdq(HIGH_PRIORITY)
 *	//		high: TASK4->TASK1, MTX2：TASK1, MTX1：TASK1
 *	42:	wup_tsk(TASK5)
 *	//		high: TASK4->TASK1->TASK5, MTX2：TASK1, MTX1：TASK1
 *	43:	unl_mtx(MTX1)								... (B-3)
 *	//		high: TASK4->TASK1->TASK5, MTX2：TASK1
 *		get_pri(TSK_SELF, &tskpri)
 *		assert(tskpri == HIGH_PRIORITY)
 *	44:	ena_dsp()
 *	== TASK4(continue)==
 *	45:	slp_tsk()
 *	//		high: TASK1->TASK5, MTX2：TASK1
 *	== TASK1(continue)==
 *	46:	slp_tsk()
 *	//		high: TASK5, MTX2：TASK1
 *	== TASK5(continue)==
 *	47:	wup_tsk(TASK1)
 *	//		high: TASK5->TASK1, MTX2：TASK1
 *	48:	slp_tsk()
 *	//		high: TASK1, MTX2：TASK1
 *
 *	== TASK1(continue)==
 *	49:	wup_tsk(TASK2)
 *		wup_tsk(TASK3)
 *		slp_tsk()
 *	//		middle: TASK2->TASK3, MTX2：TASK1
 *	== TASK2(continue)==
 *	50:	loc_mtx(MTX2)
 *	//		middle: TASK3, MTX2：TASK1->TASK2
 *	== TASK3(continue)==
 *	51:	wup_tsk(TASK1)
 *	//		high: TASK1, middle: TASK3, MTX2：TASK1->TASK2
 *	== TASK1(continue)==
 *	52:	loc_mtx(MTX3)
 *	//		high: TASK1, middle: TASK3, MTX2：TASK1->TASK2, MTX3：TASK1
 *	53:	unl_mtx(MTX2)								... (C-1)
 *	//		high: TASK1->TASK2, middle: TASK3, MTX2：TASK2, MTX3：TASK1
 *		get_pri(TASK2, &tskpri)
 *		assert(tskpri == HIGH_PRIORITY)
 *	54:	loc_mtx(MTX1)
 *	//		high: TASK1->TASK2, middle: TASK3, MTX1：TASK1, MTX2：TASK2, MTX3：TASK1
 *	55:	slp_tsk()
 *	//		high: TASK2, middle: TASK3, MTX1：TASK1, MTX2：TASK2, MTX3：TASK1
 *
 *	== TASK2(continue)==
 *	56:	wup_tsk(TASK1)
 *	//		high: TASK2->TASK1, middle: TASK3, MTX1：TASK1, MTX2：TASK2, MTX3：TASK1
 *	57:	loc_mtx(MTX1)
 *	//		high: TASK1, middle: TASK3, MTX1：TASK1->TASK2, MTX2：TASK2, MTX3：TASK1
 *	== TASK1(continue)==
 *	58:	unl_mtx(MTX1)								... (C-3)
 *	//		high: TASK1->TASK2, middle: TASK3, MTX1：TASK2, MTX2：TASK2, MTX3：TASK1
 *		get_pri(TASK2, &tskpri)
 *		assert(tskpri == HIGH_PRIORITY)
 *	59:	unl_mtx(MTX3)
 *	//		high: TASK2, middle: TASK1->TASK3, MTX1：TASK2, MTX2：TASK2
 *	== TASK2(continue)==
 *	60:	unl_mtx(MTX2)
 *	//		middle: TASK2->TASK1->TASK3, MTX1：TASK2
 *
 *	61:	rot_rdq(MID_PRIORITY)
 *	//		middle: TASK1->TASK3->TASK2, MTX1：TASK2
 *	== TASK1(continue)==
 *	62:	loc_mtx(MTX1)
 *	//		middle: TASK3->TASK2, MTX1：TASK2->TASK1
 *	== TASK3(continue)==
 *	63:	rot_rdq(MID_PRIORITY)
 *	//		middle: TASK2->TASK3, MTX1：TASK2->TASK1
 *	== TASK2(continue)==
 *	64:	unl_mtx(MTX1)								... (C-2)
 *	//		middle: TASK2->TASK3->TASK1, MTX1：TASK1
 *		get_pri(TASK1, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	== TASK2(continue)==
 *	65:	ext_tsk() -> noreturn
 *	//		middle: TASK3->TASK1, MTX1：TASK1
 *	== TASK3(continue)==
 *	66:	ext_tsk() -> noreturn
 *	//		middle: TASK1, MTX1：TASK1
 *	== TASK1(continue)==
 *	67:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/logtask.h"
#include "kernel_cfg.h"
#include "test_lib.h"
#include "test_mutex4.h"

extern ER	bit_mutex(void);

void
task1(intptr_t exinf)
{
	ER		ercd;
	PRI		tskpri;

	set_bit_func(bit_mutex);

	check_point(1);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = ploc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TSK_SELF, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == HIGH_PRIORITY);

	check_point(4);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = get_pri(TSK_SELF, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(9);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(10);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = ploc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TSK_SELF, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(13);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(15);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TSK_SELF, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(23);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = ploc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(31);
	ercd = rot_rdq(HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(32);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = ploc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TSK_SELF, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == HIGH_PRIORITY);

	check_point(34);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(36);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(39);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(40);
	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(41);
	ercd = rot_rdq(HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(42);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(43);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TSK_SELF, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == HIGH_PRIORITY);

	check_point(44);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(46);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(49);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(52);
	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(53);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TASK2, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == HIGH_PRIORITY);

	check_point(54);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(55);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(58);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TASK2, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == HIGH_PRIORITY);

	check_point(59);
	ercd = unl_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(62);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_finish(67);

	check_point(0);
}

void
task2(intptr_t exinf)
{
	ER		ercd;
	PRI		tskpri;

	check_point(14);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(24);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(50);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(56);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(57);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(60);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(61);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(64);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TASK1, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(65);
	ercd = ext_tsk();

	check_point(0);
}

void
task3(intptr_t exinf)
{
	ER		ercd;

	check_point(16);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(27);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(51);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(63);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(66);
	ercd = ext_tsk();

	check_point(0);
}

void
task4(intptr_t exinf)
{
	ER		ercd;

	check_point(6);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_ILUSE);

	check_point(7);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(35);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(45);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(0);
}

void
task5(intptr_t exinf)
{
	ER		ercd;

	check_point(37);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(38);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(47);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(48);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(0);
}
