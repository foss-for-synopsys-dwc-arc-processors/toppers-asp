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
 *  @(#) $Id: test_mutex5.c 1696 2010-01-01 16:01:25Z ertl-hiro $
 */

/*
 *		mutex test (5)
 *
 * [test purpose]
 *
 *  test the re-initialize functions of the mutex with priority ceiling
 *  protocol. change_priority and mutex_calc_priority are not fully covered.
 *
 * [test items]
 *
 *	(A) the priority change of task (runnable) that locked the mutex 
 *		when ini_mtx is called
 *		(A-1) if the ceiling priority of re-initialized mutex is the same
 *		as the current priority of task (runnable) that locked the mutex,
 *		for the situation that priority should be lowerd, lower the task
 *		priority. the precedence of the task is the highest in the 
 *		priority queue.  
 *		(A-2) if the ceiling priority of re-initialized mutex is the same
 *		as the current priority of task (runnable) that locked the mutex,
 *		for the situation that priority should not be changed, the task
 *		priority is not changed. the precedence of the task in the priority
 *		queue is also not changed.
 *		(A-3) if the ceiling priority of re-initialized mutex is lower than
 *		the current priority of task (runnable) that locked the mutex,
 *	    the task priority is not changed. the precedence of the task in the
 *	    priority queue is also not changed.
 *		(A-4) the task dispatch in (A-1)
 *	(B) the priority change of task (waiting) that locked the mutex 
 *		when ini_mtx is called
 *		(B-1) if the ceiling priority of re-initialized mutex is the same
 *		as the current priority of task (waiting) that locked the mutex,
 *		for the situation that priority should be lowerd, lower the task
 *		priority. the precedence of the task is the lowest in the 
 *		priority queue.  
 *		(B-2) if the ceiling priority of re-initialized mutex is the same
 *		as the current priority of task (waiting) that locked the mutex,
 *		for the situation that priority should not be changed, the task
 *		priority is not changed. the precedence of the task in the priority
 *		queue is also not changed.
 *		(B-3) if the ceiling priority of re-initialized mutex is lower than
 *		the current priority of task (waiting) that locked the mutex,
 *	    the task priority is not changed. the precedence of the task in the
 *	    priority queue is also not changed.
 *	(C) the state change of task waiting to lock the mutex when ini_mtx is
 *	    called  
 *		(C-1) the waiting state of task waiting to lock the mutex is
 *		released.
 *		(C-2) the task dispatch in (C-1)
 *
 * [how to implement test items]
 *
 *	(A-1)
 *		TASK1 (low priority) locked the MTX1 with middle ceiling priority,
 *		then TASK5 (high priority) re-initializes the MTX1, check if the
 *		priority of TASK1 is lowered, and check the precedence of TASK1 
 *		is higher than the precedence of TASK2 (runnable) with low priority.
 *	(A-2)
 *		TASK1 (low priority) locked the MTX1 and MTX2 with middle ceiling 
 *		priority, then TASK5 (high priority) re-initializes the MTX1, 
 *		check if the priority of TASK1 is not changed, and check the
 *		precedence of TASK1 is not changed with 2 middle priority tasks 
 *		(TASK3, TASK4) prepared.
 *	(A-3)
 *	 	TASK1 (low priority) locked the MTX2 with middle ceiling priority
 *	  	and the MTX3 with low ceiling priority, then TASK5 (high priority)
 *	  	re-initializes the MTX3, check if the priority of TASK1 is not 
 *	  	changed, and check the precedence of TASK1 is not changed with 2
 *	  	middle priority tasks (TASK3, TASK4) prepared.
 *	(A-4)
 *		TASK1 (low priority) locked the MTX2 with middle ceiling priority,
 *		then TASK1 (middle priority) re-initializes the MTX2, check if the 
 *		priority of TASK1 is lowered, TASK4 (middle priority) is dispatched,
 *		and check the precedence of TASK1 is higher than the precedence of
 *		TASK2 (runnable) with low priority.
 *	(B-1)
 *		TASK1 (low priority) locked the MTX1 with middle ceiling priority and
 *		is waiting to lock MTX4, then TASK5 (high priority) re-initializes 
 *		the MTX1,  check if the priority of TASK1 is lowered,and check the
 *		precedence of TASK1 is lower than the precedence of TASK2 (waiting
 * 		to lock MTX4) with low priority。
 *	(B-2)
 *		TASK1 (low priority) locked the MTX1 and MTX2 with middle ceiling 
 *		priority and is waiting to lock MTX4, then TASK5 (high priority) 
 *		re-initializes the MTX1, check if the priority of TASK1 is not 
 *	  	changed, and check the precedence of TASK1 is not changed with 2
 *	  	middle priority tasks (TASK3, TASK4) waiting to lock MTX4 prepared.
 *	(B-3)
 *		TASK1 (low priority) locked the MTX2 and MTX3 with middle ceiling 
 *		priority and is waiting to lock MTX4, then TASK5 (high priority) 
 *		re-initializes the MTX3, check if the priority of TASK1 is not 
 *	  	changed, and check the precedence of TASK1 is not changed with 2
 *	  	middle priority tasks (TASK3, TASK4) waiting to lock MTX4 prepared.
 *	(C-1)
 *		TASK1 (low priority) locked the MTX1 with middle ceiling priority,
 *		TASK3 (middle priority) is waiting to lock MTX1, then TASK1 (high 
 * 		priority) re-initializes the MTX1， check TASK3 is released from
 * 		waiting and check the precedence of TASK3 is lower than TASK4 (
 *		middle priority, runnable).
 *	(C-2)
 *		TASK1 (low priority) locked the MTX1 with middle ceiling priority,
 *		TASK3 (middle priority) is waiting to lock MTX1, then TASK1
 *		sleeps, TASK2 (low priority) re-initializes MTX1, TASK3 is
 *		released from waiting, check if TASK3 is dispatched.
 *
 * [required resources]
 *
 * TASK1: low priority, main task, 1st task to run
 * TASK2: low priority 
 * TASK3: middle priority 
 * TASK4: middle priority
 * TASK5: high priority
 * MTX1: mutex (TA_CEILING, the ceiling priority is middle priority)
 * MTX2: mutex (TA_CEILING, the ceiling priority is middle priority)
 * MTX3: mutex (TA_CEILING, the ceiling priority is low priority)
 * MTX4: mutex (TA_CEILING, the ceiling priority is high priority)
 * 
 * [test sequence]
 *
 *	== TASK1(priority: low)==
 *		call(set_bit_func(bit_mutex))
 *	1:	loc_mtx(MTX1)
 *	2:	act_tsk(TASK2)
 *	3:	act_tsk(TASK5)
 *	//		high: TASK5, middle: TASK1, low: TASK2, MTX1: TASK1
 *	== TASK5(priority: high)==
 *	4:	ini_mtx(MTX1)			... (A-1)
 *	//		high: TASK5, low: TASK1->TASK2
 *		get_pri(TASK1, &tskpri)
 *		assert(tskpri == LOW_PRIORITY)
 *	5:	slp_tsk()
 *	//		low: TASK1->TASK2
 *
 *	== TASK1(continue)==
 *	6:	sus_tsk(TASK2)
 *	7:	loc_mtx(MTX1)
 *		loc_mtx(MTX2)
 *	8:	act_tsk(TASK3)
 *	//		middle: TASK1->TASK3, MTX1: TASK1, MTX2: TASK1
 *	9:	dis_dsp()
 *	10:	rot_rdq(MID_PRIORITY)
 *	//		middle: TASK3->TASK1, MTX1: TASK1, MTX2: TASK1
 *	11:	act_tsk(TASK4)
 *	//		middle: TASK3->TASK1->TASK4, MTX1: TASK1, MTX2: TASK1
 *	12:	wup_tsk(TASK5)
 *	//		high: TASK5, middle: TASK3->TASK1->TASK4, MTX1: TASK1, MTX2: TASK1
 *	13:	ena_dsp()
 *	== TASK5(continue)==
 *	14:	ini_mtx(MTX1)			... (A-2)
 *	//		high: TASK5, middle: TASK3->TASK1->TASK4, MTX2: TASK1
 *		get_pri(TASK1, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	15:	slp_tsk()
 *	//		middle: TASK3->TASK1->TASK4, MTX2: TASK1
 *	== TASK3(priority: 中)==
 *	16:	slp_tsk()
 *	//		middle: TASK1->TASK4, MTX2: TASK1
 *	== TASK1(continueき)==
 *	17:	slp_tsk()
 *	//		middle: TASK4, MTX2: TASK1
 *	== TASK4(priority: 中)==
 *	18:	wup_tsk(TASK1)
 *	//		middle: TASK4->TASK1, MTX2: TASK1
 *	19:	slp_tsk()
 *	//		middle: TASK1, MTX2: TASK1
 *
 *	== TASK1(continue)==
 *	20:	loc_mtx(MTX3)
 *	//		middle: TASK1, MTX2: TASK1, MTX3: TASK1
 *	21:	wup_tsk(TASK3)
 *	//		middle: TASK1->TASK3, MTX2: TASK1, MTX3: TASK1
 *	22:	dis_dsp()
 *	23:	rot_rdq(MID_PRIORITY)
 *	//		middle: TASK3->TASK1, MTX2: TASK1, MTX3: TASK1
 *	24:	wup_tsk(TASK4)
 *	//		middle: TASK3->TASK1->TASK4, MTX2: TASK1, MTX3: TASK1
 *	25:	wup_tsk(TASK5)
 *	//		high: TASK5, middle: TASK3->TASK1->TASK4, MTX2: TASK1, MTX3: TASK1
 *	26:	ena_dsp()
 *	== TASK5(continue)==
 *	27:	ini_mtx(MTX3)			... (A-3)
 *	//		high: TASK5, middle: TASK3->TASK1->TASK4, MTX2: TASK1
 *		get_pri(TASK1, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	28:	slp_tsk()
 *	//		middle: TASK3->TASK1->TASK4, MTX2: TASK1
 *	== TASK3(continue)==
 *	29:	slp_tsk()
 *	//		middle: TASK1->TASK4, MTX2: TASK1
 *
 *	== TASK1(continue)==
 *	30:	slp_tsk()
 *	//		middle: TASK4, MTX2: TASK1
 *	== TASK4(continue)==
 *	31:	wup_tsk(TASK1)
 *	//		middle: TASK4->TASK1, MTX2: TASK1
 *	32:	rot_rdq(MID_PRIORITY)
 *	//		middle: TASK1->TASK4, MTX2: TASK1
 *	== TASK1(continue)==
 *	33:	rsm_tsk(TASK2)
 *	//		middle: TASK1->TASK4, low: TASK2, MTX2: TASK1
 *	34:	ini_mtx(MTX2)			... (A-4)
 *	//		middle: TASK4, low: TASK1->TASK2
 *	== TASK4(continue)==
 *	35:	get_pri(TASK1, &tskpri)
 *		assert(tskpri == LOW_PRIORITY)
 *	36:	slp_tsk()
 *	//		low: TASK1->TASK2
 *
 *	== TASK1(continue)==
 *	37:	wup_tsk(TASK5)
 *	== TASK5(continue)==
 *	38:	loc_mtx(MTX4)
 *	39:	tslp_tsk(10) -> E_TMOUT
 *	//		low: TASK1->TASK2, MTX4: TASK5
 *	== TASK1(continue)==
 *	40:	loc_mtx(MTX1)
 *	//		middle: TASK1, low: TASK2, MTX1: TASK1, MTX4: TASK5
 *	41:	loc_mtx(MTX4)
 *	//		low: TASK2, MTX1: TASK1, MTX4: TASK5->TASK1
 *	== TASK2(priority: low)==
 *	42:	loc_mtx(MTX4)
 *	//		MTX1: TASK1, MTX4: TASK5->TASK1->TASK2
 *	//		Wait time-out
 *	//		high: TASK5, MTX1: TASK1, MTX4: TASK5->TASK1->TASK2
 *	== TASK5(continue)==
 *	43:	ini_mtx(MTX1)			... (B-1)
 *	//		high: TASK5, MTX4: TASK5->TASK2->TASK1
 *		get_pri(TASK1, &tskpri)
 *		assert(tskpri == LOW_PRIORITY)
 *	44:	unl_mtx(MTX4)
 *	//		high: TASK5->TASK2, MTX4: TASK2->TASK1
 *	45:	slp_tsk()
 *	//		high: TASK2, MTX4: TASK2->TASK1
 *	== TASK2(continue)==
 *	46:	unl_mtx(MTX4)
 *	//		high: TASK1, low: TASK2, MTX4: TASK1
 *	== TASK1(continue)==
 *	47:	unl_mtx(MTX4)
 *	//		low: TASK1->TASK2
 *
 *	48:	wup_tsk(TASK5)
 *	== TASK5(continue)==
 *	49:	loc_mtx(MTX4)
 *	50:	slp_tsk()
 *	//		low: TASK1->TASK2, MTX4: TASK5
 *	== TASK1(continue)==
 *	51:	wup_tsk(TASK3)
 *	//		middle: TASK3, low: TASK1->TASK2, MTX4: TASK5
 *	== TASK3(continue)==
 *	52:	loc_mtx(MTX4)
 *	//		low: TASK1->TASK2, MTX4: TASK5->TASK3
 *	== TASK1(continue)==
 *	53:	loc_mtx(MTX1)
 *		loc_mtx(MTX2)
 *	//		middle: TASK1, low: TASK2, MTX1: TASK1, MTX2: TASK1, MTX4: TASK5->TASK3
 *	54:	loc_mtx(MTX4)
 *	//		low: TASK2, MTX1: TASK1, MTX2: TASK1, MTX4: TASK5->TASK3->TASK1
 *	== TASK2(continue)==
 *	55:	wup_tsk(TASK4)
 *	== TASK4(continue)==
 *	56:	loc_mtx(MTX4)
 *	//		low: TASK2, MTX1: TASK1, MTX2: TASK1, 
 *	//								MTX4: TASK5->TASK3->TASK1->TASK4
 *	== TASK2(continue)==
 *	57:	wup_tsk(TASK5)
 *	//		high: TASK5, low: TASK2, MTX1: TASK1, MTX2: TASK1, 
 *	//								MTX4: TASK5->TASK3->TASK1->TASK4
 *	== TASK5(continue)==
 *	58:	ini_mtx(MTX1)			... (B-2)
 *	//		high: TASK5, low: TASK2, MTX2: TASK1, MTX4: TASK5->TASK3->TASK1->TASK4
 *		get_pri(TASK1, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	59:	unl_mtx(MTX4)
 *	//		high: TASK5->TASK3, low: TASK2, MTX2: TASK1, MTX4: TASK3->TASK1->TASK4
 *	60:	slp_tsk()
 *	//		high: TASK3, low: TASK2, MTX2: TASK1, MTX4: TASK3->TASK1->TASK4
 *	== TASK3(continue)==
 *	61:	unl_mtx(MTX4)
 *	//		high: TASK1, middle: TASK3, low: TASK2, MTX2: TASK1, MTX4: TASK1->TASK4
 *	== TASK1(continue)==
 *	62:	unl_mtx(MTX4)
 *	//		high: TASK4, middle: TASK1->TASK3, low: TASK2, MTX2: TASK1, MTX4: TASK4
 *	== TASK4(continue)==
 *	63:	unl_mtx(MTX4)
 *	//		middle: TASK4->TASK1->TASK3, low: TASK2, MTX2: TASK1
 *	64:	slp_tsk()
 *	//		middle: TASK1->TASK3, low: TASK2, MTX2: TASK1
 *	== TASK1(continue)==
 *	65:	slp_tsk()
 *	//		middle: TASK3, low: TASK2, MTX2: TASK1
 *	== TASK3(continue)==
 *	66:	slp_tsk()
 *	//		low: TASK2, MTX2: TASK1
 *
 *	== TASK2(continue)==
 *	67: wup_tsk(TASK5)
 *	== TASK5(continue)==
 *	68:	loc_mtx(MTX4)
 *	69:	slp_tsk()
 *	//		low: TASK2, MTX4: TASK5
 *	== TASK2(continue)==
 *	70:	wup_tsk(TASK3)
 *	== TASK3(continue)==
 *	71:	loc_mtx(MTX4)
 *	//		low: TASK2, MTX4: TASK5->TASK3
 *	== TASK2(continue)==
 *	72:	wup_tsk(TASK1)
 *	//		middle: TASK1, low: TASK2, MTX4: TASK5
 *	== TASK1(continue)==
 *	73:	loc_mtx(MTX3)
 *	//		middle: TASK1, low: TASK2, MTX2: TASK1, MTX3: TASK1, MTX4: TASK5->TASK3
 *	74:	loc_mtx(MTX4)
 *	//		low: TASK2, MTX2: TASK1, MTX3: TASK1, MTX4: TASK5->TASK3->TASK1
 *	== TASK2(continue)==
 *	75:	wup_tsk(TASK4)
 *	== TASK4(continue)==
 *	76:	loc_mtx(MTX4)
 *	//		low: TASK2, MTX2: TASK1, MTX3: TASK1, 
 *	//								MTX4: TASK5->TASK3->TASK1->TASK4
 *	== TASK2(continue)==
 *	77:	wup_tsk(TASK5)
 *	//		high: TASK5, low: TASK2, MTX2: TASK1, MTX3: TASK1, 
 *	//								MTX4: TASK5->TASK3->TASK1->TASK4
 *	== TASK5(continue)==
 *	78:	ini_mtx(MTX3)			... (B-3)
 *	//		high: TASK5, low: TASK2, MTX2: TASK1, MTX4: TASK5->TASK3->TASK1->TASK4
 *		get_pri(TASK1, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	79:	unl_mtx(MTX4)
 *	//		high: TASK5->TASK3, low: TASK2, MTX2: TASK1, MTX4: TASK3->TASK1->TASK4
 *	80:	slp_tsk()
 *	//		high: TASK3, low: TASK2, MTX2: TASK1, MTX4: TASK3->TASK1->TASK4
 *	== TASK3(continue)==
 *	81:	unl_mtx(MTX4)
 *	//		high: TASK1, middle: TASK3, low: TASK2, MTX2: TASK1, MTX4: TASK1->TASK4
 *	== TASK1(continue)==
 *	82:	unl_mtx(MTX4)
 *	//		high: TASK4, middle: TASK1->TASK3, low: TASK2, MTX2: TASK1, MTX4: TASK4
 *	== TASK4(continue)==
 *	83:	unl_mtx(MTX4)
 *	//		middle: TASK4->TASK1->TASK3, low: TASK2, MTX2: TASK1
 *	84:	slp_tsk()
 *	//		middle: TASK1->TASK3, low: TASK2, MTX2: TASK1
 *	== TASK1(continue)==
 *	85:	unl_mtx(MTX2)
 *	//		middle: TASK3, low: TASK1->TASK2
 *	== TASK3(continue)==
 *	86:	slp_tsk()
 *	//		low: TASK1->TASK2
 *
 *	== TASK1(continue)==
 *	87:	sus_tsk(TASK2)
 *	//		low: TASK1
 *	88:	loc_mtx(MTX1)
 *	//		middle: TASK1, MTX1: TASK1
 *	89:	wup_tsk(TASK3)
 *	//		middle: TASK1->TASK3, MTX1: TASK1
 *	90:	rot_rdq(MID_PRIORITY)
 *	//		middle: TASK3->TASK1, MTX1: TASK1
 *	== TASK3(continue)==
 *	91:	loc_mtx(MTX1) -> E_DLT
 *	//		middle: TASK1, MTX1: TASK1->TASK3
 *	== TASK1(continue)==
 *	92:	wup_tsk(TASK5)
 *	//		high: TASK5, middle: TASK1, MTX1: TASK1->TASK3
 *	== TASK5(continue)==
 *	93:	wup_tsk(TASK4)
 *	//		high: TASK5, middle: TASK1->TASK4, MTX1: TASK1->TASK3
 *	94:	ini_mtx(MTX1)			... (C-1)
 *	//		high: TASK5, middle: TASK4->TASK3, low: TASK1
 *	95:	ext_tsk() -> noreturn
 *	//		middle: TASK4->TASK3, low: TASK1
 *	== TASK4(continue)==
 *	96:	ext_tsk() -> noreturn
 *	//		middle: TASK3, low: TASK1
 *	== TASK3(continue)==
 *	97:	slp_tsk()
 *	//		low: TASK1
 *
 *	== TASK1(continue)==
 *	98:	loc_mtx(MTX1)
 *	//		middle: TASK1, MTX1: TASK1
 *	99:	wup_tsk(TASK3)
 *	//		middle: TASK1->TASK3, MTX1: TASK1
 * 100:	rot_rdq(MID_PRIORITY)
 *	//		middle: TASK3->TASK1, MTX1: TASK1
 *	== TASK3(continue)==
 * 101:	loc_mtx(MTX1) -> E_DLT
 *	//		middle: TASK1, MTX1: TASK1->TASK3
 *	== TASK1(continue)==
 * 102:	rsm_tsk(TASK2)
 *	//		middle: TASK1, low: TASK2, MTX1: TASK1->TASK3
 * 103:	slp_tsk()
 *	//		low: TASK2, MTX1: TASK1->TASK3
 *	== TASK2(continue)==
 * 104:	ini_mtx(MTX1)			... (C-2)
 *	//		middle: TASK3, low: TASK2
 *	== TASK3(continue)==
 * 105:	ext_tsk() -> noreturn
 *	//		low: TASK2
 *	== TASK2(continue)==
 * 106:	ter_tsk(TASK1)
 * 107:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/logtask.h"
#include "kernel_cfg.h"
#include "test_lib.h"
#include "test_mutex5.h"

extern ER	bit_mutex(void);

void
task1(intptr_t exinf)
{
	ER		ercd;

	set_bit_func(bit_mutex);

	check_point(1);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = sus_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(10);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(24);
	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = rsm_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(34);
	ercd = ini_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(37);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(40);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(41);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(47);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(48);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(51);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(53);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(54);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(62);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(65);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(73);
	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(74);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(82);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(85);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(87);
	ercd = sus_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(88);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(89);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(90);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(92);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(98);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(99);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(100);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(102);
	ercd = rsm_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(103);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(0);
}

void
task2(intptr_t exinf)
{
	ER		ercd;

	check_point(42);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(46);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(55);
	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(57);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(67);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(70);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(72);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(75);
	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(77);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(104);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(106);
	ercd = ter_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_finish(107);

	check_point(0);
}

void
task3(intptr_t exinf)
{
	ER		ercd;

	check_point(16);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(52);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(61);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(66);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(71);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(81);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(86);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(91);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_DLT);

	check_point(97);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(101);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_DLT);

	check_point(105);
	ercd = ext_tsk();

	check_point(0);
}

void
task4(intptr_t exinf)
{
	ER		ercd;
	PRI		tskpri;

	check_point(18);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(31);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(32);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(35);
	ercd = get_pri(TASK1, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == LOW_PRIORITY);

	check_point(36);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(56);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(63);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(64);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(76);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(83);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(84);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(96);
	ercd = ext_tsk();

	check_point(0);
}

void
task5(intptr_t exinf)
{
	ER		ercd;
	PRI		tskpri;

	check_point(4);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TASK1, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == LOW_PRIORITY);

	check_point(5);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TASK1, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(15);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(27);
	ercd = ini_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TASK1, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(28);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(38);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(39);
	ercd = tslp_tsk(10);
	check_ercd(ercd, E_TMOUT);

	check_point(43);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TASK1, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == LOW_PRIORITY);

	check_point(44);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(45);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(49);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(50);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(58);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TASK1, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(59);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(60);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(68);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(69);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(78);
	ercd = ini_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TASK1, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(79);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(80);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(93);
	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(94);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(95);
	ercd = ext_tsk();

	check_point(0);
}
