/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: task_manage.c 2008 2010-12-31 12:41:42Z ertl-hiro $
 */

/*
 *	task management function
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"

/*
 *  default definition of trace macro
 */
#ifndef LOG_ACT_TSK_ENTER
#define LOG_ACT_TSK_ENTER(tskid)
#endif /* LOG_ACT_TSK_ENTER */

#ifndef LOG_ACT_TSK_LEAVE
#define LOG_ACT_TSK_LEAVE(ercd)
#endif /* LOG_ACT_TSK_LEAVE */

#ifndef LOG_IACT_TSK_ENTER
#define LOG_IACT_TSK_ENTER(tskid)
#endif /* LOG_IACT_TSK_ENTER */

#ifndef LOG_IACT_TSK_LEAVE
#define LOG_IACT_TSK_LEAVE(ercd)
#endif /* LOG_IACT_TSK_LEAVE */

#ifndef LOG_CAN_ACT_ENTER
#define LOG_CAN_ACT_ENTER(tskid)
#endif /* LOG_CAN_ACT_ENTER */

#ifndef LOG_CAN_ACT_LEAVE
#define LOG_CAN_ACT_LEAVE(ercd)
#endif /* LOG_CAN_ACT_LEAVE */

#ifndef LOG_EXT_TSK_ENTER
#define LOG_EXT_TSK_ENTER()
#endif /* LOG_EXT_TSK_ENTER */

#ifndef LOG_EXT_TSK_LEAVE
#define LOG_EXT_TSK_LEAVE(ercd)
#endif /* LOG_EXT_TSK_LEAVE */

#ifndef LOG_TER_TSK_ENTER
#define LOG_TER_TSK_ENTER(tskid)
#endif /* LOG_TER_TSK_ENTER */

#ifndef LOG_TER_TSK_LEAVE
#define LOG_TER_TSK_LEAVE(ercd)
#endif /* LOG_TER_TSK_LEAVE */

#ifndef LOG_CHG_PRI_ENTER
#define LOG_CHG_PRI_ENTER(tskid, tskpri)
#endif /* LOG_CHG_PRI_ENTER */

#ifndef LOG_CHG_PRI_LEAVE
#define LOG_CHG_PRI_LEAVE(ercd)
#endif /* LOG_CHG_PRI_LEAVE */

#ifndef LOG_GET_PRI_ENTER
#define LOG_GET_PRI_ENTER(tskid, p_tskpri)
#endif /* LOG_GET_PRI_ENTER */

#ifndef LOG_GET_PRI_LEAVE
#define LOG_GET_PRI_LEAVE(ercd, tskpri)
#endif /* LOG_GET_PRI_LEAVE */

#ifndef LOG_GET_INF_ENTER
#define LOG_GET_INF_ENTER(p_exinf)
#endif /* LOG_GET_INF_ENTER */

#ifndef LOG_GET_INF_LEAVE
#define LOG_GET_INF_LEAVE(ercd, exinf)
#endif /* LOG_GET_INF_LEAVE */

/*
 *  activate task, make task active
 */
#ifdef TOPPERS_act_tsk

ER
act_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_ACT_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	p_tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		if (make_active(p_tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (!(p_tcb->actque)) {
		p_tcb->actque = true;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	t_unlock_cpu();

  error_exit:
	LOG_ACT_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_act_tsk */

/*
 *  activate task, make task active
 *  (used in no-task context)
 */
#ifdef TOPPERS_iact_tsk

ER
iact_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_IACT_TSK_ENTER(tskid);
	CHECK_INTCTX_UNL();
	CHECK_TSKID(tskid);
	p_tcb = get_tcb(tskid);

	i_lock_cpu();
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		if (make_active(p_tcb)) {
			reqflg = true;
		}
		ercd = E_OK;
	}
	else if (!(p_tcb->actque)) {
		p_tcb->actque = true;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	i_unlock_cpu();

  error_exit:
	LOG_IACT_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iact_tsk */

/*
 *  cancel the activation request
 */
#ifdef TOPPERS_can_act

ER_UINT
can_act(ID tskid)
{
	TCB		*p_tcb;
	ER_UINT	ercd;

	LOG_CAN_ACT_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	p_tcb = get_tcb_self(tskid);

	t_lock_cpu();
	ercd = p_tcb->actque ? 1 : 0;
	p_tcb->actque = false;
	t_unlock_cpu();

  error_exit:
	LOG_CAN_ACT_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_can_act */

/*
 *  terminate task itself
 */
#ifdef TOPPERS_ext_tsk

ER
ext_tsk(void)
{
	ER		ercd;

	LOG_EXT_TSK_ENTER();
	CHECK_TSKCTX();

	if (t_sense_lock()) {
		/*
         * this function must be called in cpu lock state
		 */
	}
	else {
		t_lock_cpu();
	}
	if (disdsp) {
		/*
		 * dispatch must not be disabled, 
		 * or the kernel will fail
		 */
		disdsp = false;
	}
	if (!ipmflg) {
		/*
		 *  to make dispatch enable, ipmflg must be ture
		 */
		t_set_ipm(TIPM_ENAALL);
		ipmflg = true;
	}
	dspflg = true;

	(void) make_non_runnable(p_runtsk);
	make_dormant(p_runtsk);
	if (p_runtsk->actque) {
		p_runtsk->actque = false;
		(void) make_active(p_runtsk);
	}
	exit_and_dispatch();
	ercd = E_SYS;

  error_exit:
	LOG_EXT_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ext_tsk */

/*
 *  terminate task specified in tskid
 */
#ifdef TOPPERS_ter_tsk

ER
ter_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_TER_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID(tskid);
	p_tcb = get_tcb(tskid);
	CHECK_NONSELF(p_tcb);

	t_lock_cpu();
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
		if (TSTAT_RUNNABLE(p_tcb->tstat)) {
			/*
			 *  make the task unrunnable. Because the specified is
			 *  not running task, no need to make a dispatch
			 */
			(void) make_non_runnable(p_tcb);
		}
		else if (TSTAT_WAITING(p_tcb->tstat)) {
			wait_dequeue_wobj(p_tcb);
			wait_dequeue_tmevtb(p_tcb);
		}
		make_dormant(p_tcb);
		if (p_tcb->actque) {
			p_tcb->actque = false;
			if (make_active(p_tcb)) {
				dispatch();
			}
		}
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_TER_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ter_tsk */

/*
 *  change task priority
 */
#ifdef TOPPERS_chg_pri

ER
chg_pri(ID tskid, PRI tskpri)
{
	TCB		*p_tcb;
	uint_t	newpri;
	ER		ercd;

	LOG_CHG_PRI_ENTER(tskid, tskpri);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	CHECK_TPRI_INI(tskpri);
	p_tcb = get_tcb_self(tskid);
	newpri = (tskpri == TPRI_INI) ? p_tcb->p_tinib->ipriority
										: INT_PRIORITY(tskpri);

	t_lock_cpu();
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
		if (change_priority(p_tcb, newpri)) {
			dispatch();
		}
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_CHG_PRI_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_chg_pri */

/*
 *  refer task priority
 */
#ifdef TOPPERS_get_pri

ER
get_pri(ID tskid, PRI *p_tskpri)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_GET_PRI_ENTER(tskid, p_tskpri);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	p_tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
		*p_tskpri = EXT_TSKPRI(p_tcb->priority);
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_GET_PRI_LEAVE(ercd, *p_tskpri);
	return(ercd);
}

#endif /* TOPPERS_get_pri */

/*
 *  get parameter of task routine
 */
#ifdef TOPPERS_get_inf

ER
get_inf(intptr_t *p_exinf)
{
	ER		ercd;

	LOG_GET_INF_ENTER(p_exinf);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	*p_exinf = p_runtsk->p_tinib->exinf;
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_GET_INF_LEAVE(ercd, *p_exinf);
	return(ercd);
}

#endif /* TOPPERS_get_inf */
