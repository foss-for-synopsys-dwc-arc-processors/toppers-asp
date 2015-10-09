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
 *  @(#) $Id: sys_manage.c 2008 2010-12-31 12:41:42Z ertl-hiro $
 */

/*
 *	system management function
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"

/*
 *   default definition of trace macro
 */
#ifndef LOG_ROT_RDQ_ENTER
#define LOG_ROT_RDQ_ENTER(tskpri)
#endif /* LOG_ROT_RDQ_ENTER */

#ifndef LOG_ROT_RDQ_LEAVE
#define LOG_ROT_RDQ_LEAVE(ercd)
#endif /* LOG_ROT_RDQ_LEAVE */

#ifndef LOG_IROT_RDQ_ENTER
#define LOG_IROT_RDQ_ENTER(tskpri)
#endif /* LOG_IROT_RDQ_ENTER */

#ifndef LOG_IROT_RDQ_LEAVE
#define LOG_IROT_RDQ_LEAVE(ercd)
#endif /* LOG_IROT_RDQ_LEAVE */

#ifndef LOG_GET_TID_ENTER
#define LOG_GET_TID_ENTER(p_tskid)
#endif /* LOG_GET_TID_ENTER */

#ifndef LOG_GET_TID_LEAVE
#define LOG_GET_TID_LEAVE(ercd, tskid)
#endif /* LOG_GET_TID_LEAVE */

#ifndef LOG_IGET_TID_ENTER
#define LOG_IGET_TID_ENTER(p_tskid)
#endif /* LOG_IGET_TID_ENTER */

#ifndef LOG_IGET_TID_LEAVE
#define LOG_IGET_TID_LEAVE(ercd, tskid)
#endif /* LOG_IGET_TID_LEAVE */

#ifndef LOG_LOC_CPU_ENTER
#define LOG_LOC_CPU_ENTER()
#endif /* LOG_LOC_CPU_ENTER */

#ifndef LOG_LOC_CPU_LEAVE
#define LOG_LOC_CPU_LEAVE(ercd)
#endif /* LOG_LOC_CPU_LEAVE */

#ifndef LOG_ILOC_CPU_ENTER
#define LOG_ILOC_CPU_ENTER()
#endif /* LOG_ILOC_CPU_ENTER */

#ifndef LOG_ILOC_CPU_LEAVE
#define LOG_ILOC_CPU_LEAVE(ercd)
#endif /* LOG_ILOC_CPU_LEAVE */

#ifndef LOG_UNL_CPU_ENTER
#define LOG_UNL_CPU_ENTER()
#endif /* LOG_UNL_CPU_ENTER */

#ifndef LOG_UNL_CPU_LEAVE
#define LOG_UNL_CPU_LEAVE(ercd)
#endif /* LOG_UNL_CPU_LEAVE */

#ifndef LOG_IUNL_CPU_ENTER
#define LOG_IUNL_CPU_ENTER()
#endif /* LOG_IUNL_CPU_ENTER */

#ifndef LOG_IUNL_CPU_LEAVE
#define LOG_IUNL_CPU_LEAVE(ercd)
#endif /* LOG_IUNL_CPU_LEAVE */

#ifndef LOG_DIS_DSP_ENTER
#define LOG_DIS_DSP_ENTER()
#endif /* LOG_DIS_DSP_ENTER */

#ifndef LOG_DIS_DSP_LEAVE
#define LOG_DIS_DSP_LEAVE(ercd)
#endif /* LOG_DIS_DSP_LEAVE */

#ifndef LOG_ENA_DSP_ENTER
#define LOG_ENA_DSP_ENTER()
#endif /* LOG_ENA_DSP_ENTER */

#ifndef LOG_ENA_DSP_LEAVE
#define LOG_ENA_DSP_LEAVE(ercd)
#endif /* LOG_ENA_DSP_LEAVE */

#ifndef LOG_SNS_CTX_ENTER
#define LOG_SNS_CTX_ENTER()
#endif /* LOG_SNS_CTX_ENTER */

#ifndef LOG_SNS_CTX_LEAVE
#define LOG_SNS_CTX_LEAVE(state)
#endif /* LOG_SNS_CTX_LEAVE */

#ifndef LOG_SNS_LOC_ENTER
#define LOG_SNS_LOC_ENTER()
#endif /* LOG_SNS_LOC_ENTER */

#ifndef LOG_SNS_LOC_LEAVE
#define LOG_SNS_LOC_LEAVE(state)
#endif /* LOG_SNS_LOC_LEAVE */

#ifndef LOG_SNS_DSP_ENTER
#define LOG_SNS_DSP_ENTER()
#endif /* LOG_SNS_DSP_ENTER */

#ifndef LOG_SNS_DSP_LEAVE
#define LOG_SNS_DSP_LEAVE(state)
#endif /* LOG_SNS_DSP_LEAVE */

#ifndef LOG_SNS_DPN_ENTER
#define LOG_SNS_DPN_ENTER()
#endif /* LOG_SNS_DPN_ENTER */

#ifndef LOG_SNS_DPN_LEAVE
#define LOG_SNS_DPN_LEAVE(state)
#endif /* LOG_SNS_DPN_LEAVE */

#ifndef LOG_SNS_KER_ENTER
#define LOG_SNS_KER_ENTER()
#endif /* LOG_SNS_KER_ENTER */

#ifndef LOG_SNS_KER_LEAVE
#define LOG_SNS_KER_LEAVE(state)
#endif /* LOG_SNS_KER_LEAVE */

/*
 *  rotate task precedence for tasks in the
 *  same priority  
 */
#ifdef TOPPERS_rot_rdq

ER
rot_rdq(PRI tskpri)
{
	uint_t	pri;
	ER		ercd;

	LOG_ROT_RDQ_ENTER(tskpri);
	CHECK_TSKCTX_UNL();
	CHECK_TPRI_SELF(tskpri);

	t_lock_cpu();
	pri = (tskpri == TPRI_SELF) ? p_runtsk->priority : INT_PRIORITY(tskpri);
	if (rotate_ready_queue(pri)) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_ROT_RDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_rot_rdq */

/*
 *  rotate task precedence for tasks in the
 *  same priority(used in non-task context)
 */
#ifdef TOPPERS_irot_rdq

ER
irot_rdq(PRI tskpri)
{
	ER		ercd;

	LOG_IROT_RDQ_ENTER(tskpri);
	CHECK_INTCTX_UNL();
	CHECK_TPRI(tskpri);

	i_lock_cpu();
	if (rotate_ready_queue(INT_PRIORITY(tskpri))) {
		reqflg = true;
	}
	ercd = E_OK;
	i_unlock_cpu();

  error_exit:
	LOG_IROT_RDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_irot_rdq */

/*
 *  get the task id of running task
 */
#ifdef TOPPERS_get_tid

ER
get_tid(ID *p_tskid)
{
	ER		ercd;

	LOG_GET_TID_ENTER(p_tskid);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	*p_tskid = TSKID(p_runtsk);
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_GET_TID_LEAVE(ercd, *p_tskid);
	return(ercd);
}

#endif /* TOPPERS_get_tid */

/*
 *    get the task id of running task(used in non-task context)
 */
#ifdef TOPPERS_iget_tid

ER
iget_tid(ID *p_tskid)
{
	ER		ercd;

	LOG_IGET_TID_ENTER(p_tskid);
	CHECK_INTCTX_UNL();

	i_lock_cpu();
	*p_tskid = (p_runtsk == NULL) ? TSK_NONE : TSKID(p_runtsk);
	ercd = E_OK;
	i_unlock_cpu();

  error_exit:
	LOG_IGET_TID_LEAVE(ercd, *p_tskid);
	return(ercd);
}

#endif /* TOPPERS_iget_tid */

/*
 *  lock cpu, then no interrupts can happen
 */
#ifdef TOPPERS_loc_cpu

ER
loc_cpu(void)
{
	ER		ercd;

	LOG_LOC_CPU_ENTER();
	CHECK_TSKCTX();

	if (!t_sense_lock()) {
		t_lock_cpu();
	}
	ercd = E_OK;

  error_exit:
	LOG_LOC_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_loc_cpu */

/*
 *  lock cpu(used non-task context)
 */
#ifdef TOPPERS_iloc_cpu

ER
iloc_cpu(void)
{
	ER		ercd;

	LOG_ILOC_CPU_ENTER();
	CHECK_INTCTX();

	if (!i_sense_lock()) {
		i_lock_cpu();
	}
	ercd = E_OK;

  error_exit:
	LOG_ILOC_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iloc_cpu */

/*
 *  unlock cpu
 *
 *  when cpu is locked, API which may cause a dispatch cannot be called,
 *  so when cpu is unlocked, no need to make a dispatch.
 */
#ifdef TOPPERS_unl_cpu

ER
unl_cpu(void)
{
	ER		ercd;

	LOG_UNL_CPU_ENTER();
	CHECK_TSKCTX();

	if (t_sense_lock()) {
		t_unlock_cpu();
	}
	ercd = E_OK;

  error_exit:
	LOG_UNL_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_unl_cpu */

/*
 * unlock cpu(used in non-task context)
 *
 * when cpu is locked, API which may cause a dispatch cannot be called,
 * so when cpu is unlocked, no need to make a dispatch.．
 */
#ifdef TOPPERS_iunl_cpu

ER
iunl_cpu(void)
{
	ER		ercd;

	LOG_IUNL_CPU_ENTER();
	CHECK_INTCTX();

	if (i_sense_lock()) {
		i_unlock_cpu();
	}
	ercd = E_OK;

  error_exit:
	LOG_IUNL_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iunl_cpu */

/*
 *  disable the dispatcher
 */
#ifdef TOPPERS_dis_dsp

ER
dis_dsp(void)
{
	ER		ercd;

	LOG_DIS_DSP_ENTER();
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	disdsp = true;
	dspflg = false;
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_DIS_DSP_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_dis_dsp */

/*
 *  enable the dispatcher
 */
#ifdef TOPPERS_ena_dsp

ER
ena_dsp(void)
{
	ER		ercd;

	LOG_ENA_DSP_ENTER();
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	disdsp = false;
	if (ipmflg) {
		dspflg = true;
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_ENA_DSP_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ena_dsp */

/*
 *  sense current context
 */
#ifdef TOPPERS_sns_ctx

bool_t
sns_ctx(void)
{
	bool_t	state;

	LOG_SNS_CTX_ENTER();
	state = sense_context() ? true : false;
	LOG_SNS_CTX_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_sns_ctx */

/*
 *  sense whether cpu is locked
 */
#ifdef TOPPERS_sns_loc

bool_t
sns_loc(void)
{
	bool_t	state;

	LOG_SNS_LOC_ENTER();
	state = x_sense_lock() ? true : false;
	LOG_SNS_LOC_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_sns_loc */

/*
 *  sense the value of disdsp(disable dispatch)
 */
#ifdef TOPPERS_sns_dsp

bool_t
sns_dsp(void)
{
	bool_t	state;

	LOG_SNS_DSP_ENTER();
	state = disdsp;
	LOG_SNS_DSP_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_sns_dsp */

/*
 *  sense whether in dispatch pending state
 */
#ifdef TOPPERS_sns_dpn

bool_t
sns_dpn(void)
{
	bool_t	state;

	LOG_SNS_DPN_ENTER();
	state = (sense_context() || t_sense_lock() || !dspflg) ? true : false;
	LOG_SNS_DPN_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_sns_dpn */

/*
 *  sense whether kernel is active
 */
#ifdef TOPPERS_sns_ker

bool_t
sns_ker(void)
{
	bool_t	state;

	LOG_SNS_KER_ENTER();
	state = kerflg ? false : true;
	LOG_SNS_KER_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_sns_ker */
