/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: test_mutex8.c 1696 2010-01-01 16:01:25Z ertl-hiro $
 */

/*
 *		mutex test (8)
 *
 * [test purpose]
 *
 *  test the related processings for mutex with priority ceiling
 *  protocol when chg_pri is called. however, change_priority and
 *  mutex_calc_priority are not fully covered. 
 *
 * [test items]
 *
 *	(A) change task base priority (chg_pri)
 *		(A-1) the target task is runnable and does not lock the mutex,
 *		the precedence of target task is the lowest in the priority
 *		queue.
 *		(A-2) the target task is runnable and only locked the mutex with
 *		TA_TPRI, the precedence of target task is the lowest in the priority
 *		queue.
 *		(A-3) the target task is runnable and locked the mutex with
 *		priority ceiling protocol, the precedence of target task is not
 *		changed.
 *		(A-4) the target task is waiting in priority order and does not
 *		lock the mutex,  the precedence of target task is the lowest in
 *		the priority queue.
 *		(A-5) the target task is waiting and only locked the mutex with
 *		TA_TPRI, the precedence of target task is the lowest in the priority
 *		queue.
 *		(A-6) the target task is waiting and locked the mutex with
 *		priority ceiling protocol, the precedence of target task is not
 *		changed.
 *	(B) the E_ILUSE error of chg_pri
 *		(B-1) for the target task, if the changed base priority is higher
 *		 than the locked mutex's ceiling priority, return E_ILUSE error. 
 *		(B-2) for the target task, if the changed base priority is not 
 *		higher than the locked mutex's ceiling priority, do not return
 *		E_ILUSE error. 
 *		(B-3) for the target task, if the changed base priority is higher
 *		 than the waitedd mutex's ceiling priority, return E_ILUSE error.
 *		(B-4) for the target task, if the changed base priority is not 
 *		higher than the waitedd mutex's ceiling priority, do not return
 *		E_ILUSE error.
 *
 * [how to implement test items]
 *
 *	(A-1)
 *		TASK1 with middle priority is runnable, TASK5 with high priority
 *		changes TASK1's priority through chg_pri, check if the precedence
 *		of TASK1 is lower than the precedence of TASK4 with same priority
 *	(A-2)
 *		TASK1 with middle priority is runnable and locked the MTX3 
 *		(TA_TPRI), TASK5 with high priority changes TASK1's priority
 *		through chg_pri, check if the precedence of TASK1 is lower than
 *		the precedence of TASK4 with same priority
 *	(A-3)
 *		TASK1 with middle priority is runnable and locked the MTX3 
 *		with middle ceiling priority, TASK5 with high priority changes 
 *		TASK1's priority into low priority through chg_pri, check if the
 *		priority of TASK1 is not changed and the precedence of TASK1 is 
 *		not changed with 2 middle priority runnable tasks (TASK2, TASK3)
 *		prepared.
 *	(A-4)
 *		TASK1 with middle priority is waiting to lock MTX2 with high 
 *		ceiling priority, TASK5 with high priority changes TASK1's 
 *		priority into low priority through chg_pri, check if
 *		the precedence of TASK1 is lower than TASK4 (low priority) 
 *		waiting to lock MTX2 in the waiting queue.
 *	(A-5)
 *		TASK1 with middle priority locked the MTX3 (TA_TPRI) and is waiting
 *		to lock MTX2 with high ceiling priority, TASK5 with high priority
 *		changes TASK1's priority into low priority through chg_pri, check if
 *		the precedence of TASK1 is not changed in the waiting queue with 
 *		TASK4 (low priority) waiting to lock MTX2.
 *	(A-6)
 *		TASK1 with middle priority locked the MTX1 with middle ceiling 
 *		priority and is waiting to lock MTX2 with high ceiling priority,
 *		TASK5 with high priority changes TASK1's priority into low priority
 *		through chg_pri, check if the priority of TASK1 is not changed
 *		and the precedence of TASK1 is not changed in the waiting queue with 
 *		TASK2, TASK3 (low priority) waiting to lock MTX2.
 *	(B-1)
 *		TASK1 with middle priority locked the MTX1 with middle ceiling 
 *		priority and is runnable, TASK5 with high priority changes TASK1's
 *		priority into high priority through chg_pri, check if E_ILUSE is
 *		returned.
 *	(B-2) 
 *		check if E_ILUSE is not returned in (A-3);
 *	(B-3)
 *		TASK2 with middle priority is waiting to lock the MTX1 with middle
 *		ceiling priority, TASK5 with high priority changes TASK2's priority
 *		into high priority through chg_pri, check if E_ILUSE is returned.
 *	(B-4)
 *		check if E_ILUSE is not returned in (A-6);
 *
 * [required resources]
 *
 *  TASK1: middle priority, main task, 1st task to run
 *	TASK2: middle priority 
 *	TASK3: middle priority 
 *	TASK4: low priority
 *	TASK5: high priority
 *	MTX1: mutex (TA_CEILING, the ceiling priority is middle priority)
 *	MTX2: mutex (TA_CEILING, the ceiling priority is high priority)
 *	MTX3: mutex (TA_TPRI)
 *
 * [test sequence]
 *
 *	== TASK1(priority: low)==
 *		call(set_bit_func(bit_mutex))
 *	1:	act_tsk(TASK4)
 *	2:	act_tsk(TASK5)
 *	//		high: TASK5, middle: TASK1, low:TASK4
 *	== TASK5(priority: high)==
 *	3:	chg_pri(TASK1, LOW_PRIORITY)		... (A-1)
 *	//		high: TASK5, low:TASK4->TASK1
 *	4:	slp_tsk()
 *	== TASK4(priority: low)==
 *	5:	slp_tsk()
 *	== TASK1(continue)==
 *	6:	chg_pri(TSK_SELF, TPRI_INI)
 *
 *	7:	loc_mtx(MTX3)
 *	8:	wup_tsk(TASK4)
 *	9:	wup_tsk(TASK5)
 *	//		high: TASK5, middle: TASK1, low:TASK4, MTX3:TASK1
 *	== TASK5(continue)==
 *	10:	chg_pri(TASK1, LOW_PRIORITY)		... (A-2)
 *	//		high: TASK5, low:TASK4->TASK1, MTX3:TASK1
 *	11:	slp_tsk()
 *	== TASK4(continue)==
 *	12:	slp_tsk()
 *	== TASK1(continue)==
 *	13:	unl_mtx(MTX3)
 *		chg_pri(TSK_SELF, TPRI_INI)
 *
 *	14:	loc_mtx(MTX1)
 *	15:	act_tsk(TASK2)
 *	16:	dis_dsp()
 *	17:	rot_rdq(MID_PRIORITY)
 *	18:	act_tsk(TASK3)
 *	//		middle: TASK2->TASK1->TASK3, MTX1:TASK1
 *	19:	wup_tsk(TASK5)
 *	//		high: TASK5, middle: TASK2->TASK1->TASK3, MTX1:TASK1
 *	20:	ena_dsp()
 *	== TASK5(continue)==
 *	21:	chg_pri(TASK1, LOW_PRIORITY)		... (A-3)(B-2)
 *	//		high: TASK5, middle: TASK2->TASK1->TASK3, MTX1:TASK1
 *		get_pri(TASK1, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	22:	slp_tsk()
 *	//		middle: TASK2->TASK1->TASK3, MTX1:TASK1
 *	== TASK2(continue)==
 *	23:	slp_tsk()
 *	//		middle: TASK1->TASK3, MTX1:TASK1
 *	== TASK1(continue)==
 *	24:	unl_mtx(MTX1)
 *	//		middle: TASK3, low:TASK1
 *	== TASK3(continue)==
 *	25:	slp_tsk()
 *	== TASK1(continue)==
 *	26:	chg_pri(TSK_SELF, TPRI_INI)
 *
 *	27:	wup_tsk(TASK5)
 *	== TASK5(continue)==
 *	28:	loc_mtx(MTX2)
 *	29:	tslp_tsk(10) -> E_TMOUT
 *	== TASK1(continue)==
 *	30:	wup_tsk(TASK4)
 *	31:	loc_mtx(MTX2)
 *	== TASK4(continue)==
 *	32:	loc_mtx(MTX2)
 *	//		MTX2:TASK5->TASK1->TASK4
 *	//		Wait time-out
 *	//		high: TASK5, MTX2:TASK5->TASK1->TASK4
 *	== TASK5(continue)==
 *	33:	chg_pri(TASK1, LOW_PRIORITY)		... (A-4)
 *	//		high: TASK5, MTX2:TASK5->TASK4->TASK1
 *	34:	unl_mtx(MTX2)
 *	//		high: TASK5->TASK4, MTX2:TASK4->TASK1
 *	35:	slp_tsk()
 *	//		high: TASK4, MTX2:TASK4->TASK1
 *	== TASK4(continue)==
 *	36:	unl_mtx(MTX2)
 *	//		high: TASK1, low:TASK4, MTX2:TASK1
 *	== TASK1(continue)==
 *	37:	unl_mtx(MTX2)
 *	//		low:TASK1->TASK4
 *	38:	chg_pri(TSK_SELF, TPRI_INI)
 *
 *	39:	loc_mtx(MTX3)
 *	40:	wup_tsk(TASK5)
 *	//		high: TASK5, middle: TASK1, low:TASK4, MTX3:TASK1
 *	== TASK5(continue)==
 *	41:	loc_mtx(MTX2)
 *	42:	tslp_tsk(10) -> E_TMOUT
 *	== TASK1(continue)==
 *	43:	loc_mtx(MTX2)
 *	== TASK4(continue)==
 *	44:	loc_mtx(MTX2)
 *	//		MTX2:TASK5->TASK1->TASK4, MTX3:TASK1
 *	//		Wait time-out
 *	//		high: TASK5, MTX2:TASK5->TASK1->TASK4, MTX3:TASK1
 *	== TASK5(continue)==
 *	45:	chg_pri(TASK1, LOW_PRIORITY)		... (A-5)
 *	//		high: TASK5, MTX2:TASK5->TASK4->TASK1, MTX3:TASK1
 *	46:	unl_mtx(MTX2)
 *	//		high: TASK5->TASK4, MTX2:TASK4->TASK1, MTX3:TASK1
 *	47:	slp_tsk()
 *	//		high: TASK4, MTX2:TASK4->TASK1, MTX3:TASK1
 *	== TASK4(continue)==
 *	48:	unl_mtx(MTX2)
 *	//		high: TASK1, low:TASK4, MTX2:TASK1, MTX3:TASK1
 *	== TASK1(continue)==
 *	49:	unl_mtx(MTX2)
 *	//		low:TASK1->TASK4, MTX3:TASK1
 *	50:	unl_mtx(MTX3)
 *	//		low:TASK1->TASK4
 *	51:	ter_tsk(TASK4)
 *	52:	chg_pri(TSK_SELF, TPRI_INI)
 *
 *	53:	loc_mtx(MTX1)
 *	54:	wup_tsk(TASK5)
 *	//		high: TASK5, middle: TASK1, MTX1:TASK1
 *	== TASK5(continue)==
 *	55:	loc_mtx(MTX2)
 *	56:	tslp_tsk(10) -> E_TMOUT
 *	//		middle: TASK1, MTX1:TASK1, MTX2:TASK5
 *	== TASK1(continue)==
 *	57:	wup_tsk(TASK2)
 *	58:	rot_rdq(MID_PRIORITY)
 *	== TASK2(continue)==
 *	59:	loc_mtx(MTX2)
 *	//		middle: TASK1, MTX1:TASK1, MTX2:TASK5->TASK2
 *	== TASK1(continue)==
 *	60:	wup_tsk(TASK3)
 *	61:	loc_mtx(MTX2)
 *	//		middle: TASK3, MTX1:TASK1, MTX2:TASK5->TASK2->TASK1
 *	== TASK3(continue)==
 *	62:	loc_mtx(MTX2)
 *	//		MTX1:TASK1, MTX2:TASK5->TASK2->TASK1->TASK3
 *	//		Wait time-out
 *	//		high: TASK5, MTX1:TASK1, MTX2:TASK5->TASK2->TASK1->TASK3
 *	== TASK5(continue)==
 *	63:	chg_pri(TASK1, LOW_PRIORITY)		... (A-6)(B-4)
 *	//		high: TASK5, MTX1:TASK1, MTX2:TASK5->TASK2->TASK1->TASK3
 *		get_pri(TASK1, &tskpri)
 *		assert(tskpri == MID_PRIORITY)
 *	64:	unl_mtx(MTX2)
 *	//		high: TASK5->TASK2, MTX1:TASK1, MTX2:TASK2->TASK1->TASK3
 *	65:	slp_tsk()
 *	//		high: TASK2, MTX1:TASK1, MTX2:TASK2->TASK1->TASK3
 *	== TASK2(continue)==
 *	66:	unl_mtx(MTX2)
 *	//		high: TASK1, middle: TASK2, MTX1:TASK1, MTX2:TASK1->TASK3
 *	== TASK1(continue)==
 *	67:	unl_mtx(MTX2)
 *	//		high: TASK3, middle: TASK1->TASK2, MTX1:TASK1, MTX2:TASK3
 *	== TASK3(continue)==
 *	68:	unl_mtx(MTX2)
 *	//		middle: TASK3->TASK1->TASK2, MTX1:TASK1
 *	69:	ext_tsk() -> noreturn
 *	//		middle: TASK1->TASK2, MTX1:TASK1
 *	== TASK1(continue)==
 *	70:	chg_pri(TSK_SELF, TPRI_INI)
 *	//		middle: TASK1->TASK2, MTX1:TASK1
 *
 *	71:	rot_rdq(MID_PRIORITY)
 *	//		middle: TASK2->TASK1, MTX1:TASK1
 *	== TASK2(continue)==
 *	72:	loc_mtx(MTX1)
 *	//		middle: TASK1, MTX1:TASK1->TASK2
 *	== TASK1(continue)==
 *	73:	wup_tsk(TASK5)
 *	//		high: TASK5, middle: TASK1, MTX1:TASK1->TASK2
 *	== TASK5(continue)==
 *	74:	chg_pri(TASK1, HIGH_PRIORITY) -> E_ILUSE	... (B-1)
 *	75:	chg_pri(TASK2, HIGH_PRIORITY) -> E_ILUSE	... (B-3)
 *	76:	ext_tsk() -> noreturn
 *	//		middle: TASK1, MTX1:TASK1->TASK2
 *	== TASK1(continue)==
 *	77:	unl_mtx(MTX1)
 *	//		middle: TASK1->TASK2, MTX1:TASK2
 *	78:	ext_tsk() -> noreturn
 *	== TASK2(continue)==
 *	79:	unl_mtx(MTX1)
 *	80:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/logtask.h"
#include "kernel_cfg.h"
#include "test_lib.h"
#include "test_mutex8.h"

extern ER	bit_mutex(void);

void
task1(intptr_t exinf)
{
	ER		ercd;

	set_bit_func(bit_mutex);

	check_point(1);
	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = chg_pri(TSK_SELF, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = unl_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TSK_SELF, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(15);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(24);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = chg_pri(TSK_SELF, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(27);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(31);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(37);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(38);
	ercd = chg_pri(TSK_SELF, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(39);
	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(40);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(43);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(49);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(50);
	ercd = unl_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(51);
	ercd = ter_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(52);
	ercd = chg_pri(TSK_SELF, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(53);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(54);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(57);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(58);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(60);
	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(61);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(67);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(70);
	ercd = chg_pri(TSK_SELF, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(71);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(73);
	ercd = wup_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(77);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(78);
	ercd = ext_tsk();

	check_point(0);
}

void
task2(intptr_t exinf)
{
	ER		ercd;

	check_point(23);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(59);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(66);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(72);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(79);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_finish(80);

	check_point(0);
}

void
task3(intptr_t exinf)
{
	ER		ercd;

	check_point(25);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(62);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(68);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(69);
	ercd = ext_tsk();

	check_point(0);
}

void
task4(intptr_t exinf)
{
	ER		ercd;

	check_point(5);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(32);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(36);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(44);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(48);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(0);
}

void
task5(intptr_t exinf)
{
	ER		ercd;
	PRI		tskpri;

	check_point(3);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(10);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TASK1, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(22);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = tslp_tsk(10);
	check_ercd(ercd, E_TMOUT);

	check_point(33);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(34);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(35);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(41);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(42);
	ercd = tslp_tsk(10);
	check_ercd(ercd, E_TMOUT);

	check_point(45);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(46);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(47);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(55);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(56);
	ercd = tslp_tsk(10);
	check_ercd(ercd, E_TMOUT);

	check_point(63);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = get_pri(TASK1, &tskpri);
	check_ercd(ercd, E_OK);
	check_assert(tskpri == MID_PRIORITY);

	check_point(64);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(65);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(74);
	ercd = chg_pri(TASK1, HIGH_PRIORITY);
	check_ercd(ercd, E_ILUSE);

	check_point(75);
	ercd = chg_pri(TASK2, HIGH_PRIORITY);
	check_ercd(ercd, E_ILUSE);

	check_point(76);
	ercd = ext_tsk();

	check_point(0);
}
