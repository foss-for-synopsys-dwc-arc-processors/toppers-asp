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
 *  $Id: task_except.c 2023 2011-01-02 08:59:06Z ertl-hiro $
 */

/*
 *	task exception function
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"

/*
 *  default definition of trace macro
 */
#ifndef LOG_RAS_TEX_ENTER
#define LOG_RAS_TEX_ENTER(tskid, rasptn)
#endif /* LOG_RAS_TEX_ENTER */

#ifndef LOG_RAS_TEX_LEAVE
#define LOG_RAS_TEX_LEAVE(ercd)
#endif /* LOG_RAS_TEX_LEAVE */

#ifndef LOG_IRAS_TEX_ENTER
#define LOG_IRAS_TEX_ENTER(tskid, rasptn)
#endif /* LOG_IRAS_TEX_ENTER */

#ifndef LOG_IRAS_TEX_LEAVE
#define LOG_IRAS_TEX_LEAVE(ercd)
#endif /* LOG_IRAS_TEX_LEAVE */

#ifndef LOG_DIS_TEX_ENTER
#define LOG_DIS_TEX_ENTER()
#endif /* LOG_DIS_TEX_ENTER */

#ifndef LOG_DIS_TEX_LEAVE
#define LOG_DIS_TEX_LEAVE(ercd)
#endif /* LOG_DIS_TEX_LEAVE */

#ifndef LOG_ENA_TEX_ENTER
#define LOG_ENA_TEX_ENTER()
#endif /* LOG_ENA_TEX_ENTER */

#ifndef LOG_ENA_TEX_LEAVE
#define LOG_ENA_TEX_LEAVE(ercd)
#endif /* LOG_ENA_TEX_LEAVE */

#ifndef LOG_SNS_TEX_ENTER
#define LOG_SNS_TEX_ENTER()
#endif /* LOG_SNS_TEX_ENTER */

#ifndef LOG_SNS_TEX_LEAVE
#define LOG_SNS_TEX_LEAVE(state)
#endif /* LOG_SNS_TEX_LEAVE */

#ifndef LOG_REF_TEX_ENTER
#define LOG_REF_TEX_ENTER(tskid, pk_rtex)
#endif /* LOG_REF_TEX_ENTER */

#ifndef LOG_REF_TEX_LEAVE
#define LOG_REF_TEX_LEAVE(ercd, pk_rtex)
#endif /* LOG_REF_TEX_LEAVE */

/*
 *  raise task exception
 */
#ifdef TOPPERS_ras_tex

ER
ras_tex(ID tskid, TEXPTN rasptn)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_RAS_TEX_ENTER(tskid, rasptn);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	CHECK_PAR(rasptn != 0U);
	p_tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (TSTAT_DORMANT(p_tcb->tstat) || p_tcb->p_tinib->texrtn == NULL) {
		ercd = E_OBJ;
	}
	else {
		p_tcb->texptn |= rasptn;
		if (p_tcb == p_runtsk && p_runtsk->enatex && ipmflg) {
			call_texrtn();
		}
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_RAS_TEX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ras_tex */

/*
 *  raise task exception(used in non-task context)
 */
#ifdef TOPPERS_iras_tex

ER
iras_tex(ID tskid, TEXPTN rasptn)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_IRAS_TEX_ENTER(tskid, rasptn);
	CHECK_INTCTX_UNL();
	CHECK_TSKID(tskid);
	CHECK_PAR(rasptn != 0U);
	p_tcb = get_tcb(tskid);

	i_lock_cpu();
	if (TSTAT_DORMANT(p_tcb->tstat) || p_tcb->p_tinib->texrtn == NULL) {
		ercd = E_OBJ;
	}
	else {
		p_tcb->texptn |= rasptn;
		if (p_tcb == p_runtsk && p_runtsk->enatex && ipmflg) {
			reqflg = true;
		}
		ercd = E_OK;
	}
	i_unlock_cpu();

  error_exit:
	LOG_IRAS_TEX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iras_tex */

/*
 *  disable task exception
 */
#ifdef TOPPERS_dis_tex

ER
dis_tex(void)
{
	ER		ercd;

	LOG_DIS_TEX_ENTER();
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	if (p_runtsk->p_tinib->texrtn == NULL) {
		ercd = E_OBJ;
	}
	else {
		p_runtsk->enatex = false;
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_DIS_TEX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_dis_tex */

/*
 *  enable task exception
 */
#ifdef TOPPERS_ena_tex

ER
ena_tex(void)
{
	ER		ercd;

	LOG_ENA_TEX_ENTER();
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	if (p_runtsk->p_tinib->texrtn == NULL) {
		ercd = E_OBJ;
	}
	else {
		p_runtsk->enatex = true;
		if (p_runtsk->texptn != 0U && ipmflg) {
			call_texrtn();
		}
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_ENA_TEX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ena_tex */

/*
 *  sense whether task exception is enabled
 */
#ifdef TOPPERS_sns_tex

bool_t
sns_tex(void)
{
	bool_t	state;

	LOG_SNS_TEX_ENTER();
	state = (p_runtsk != NULL && p_runtsk->enatex) ? false : true;
	LOG_SNS_TEX_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_sns_tex */

/*
 *  refer the status of task exception
 */
#ifdef TOPPERS_ref_tex

ER
ref_tex(ID tskid, T_RTEX *pk_rtex)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_REF_TEX_ENTER(tskid, pk_rtex);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	p_tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (TSTAT_DORMANT(p_tcb->tstat) || p_tcb->p_tinib->texrtn == NULL) {
		ercd = E_OBJ;
	}
	else {
		pk_rtex->texstat = (p_tcb->enatex) ? TTEX_ENA : TTEX_DIS;
		pk_rtex->pndptn = p_tcb->texptn;
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_REF_TEX_LEAVE(ercd, pk_rtex);
	return(ercd);
}

#endif /* TOPPERS_ref_tex */
