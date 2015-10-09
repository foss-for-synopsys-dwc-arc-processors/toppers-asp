/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2007 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: overrun.c 1902 2010-08-17 22:38:52Z ertl-hiro $
 */

/*
 *		overrun handler module
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "overrun.h"

#ifdef TOPPERS_SUPPORT_OVRHDR
#include "target_timer.h"

/*
 *  the default definition of the trace log macros
 */
#ifndef LOG_OVR_ENTER
#define LOG_OVR_ENTER(p_runtsk)
#endif /* LOG_OVR_ENTER */

#ifndef LOG_OVR_LEAVE
#define LOG_OVR_LEAVE(p_runtsk)
#endif /* LOG_OVR_LEAVE */

#ifndef LOG_STA_OVR_ENTER
#define LOG_STA_OVR_ENTER(tskid, ovrtim)
#endif /* LOG_STA_OVR_ENTER */

#ifndef LOG_STA_OVR_LEAVE
#define LOG_STA_OVR_LEAVE(ercd)
#endif /* LOG_STA_OVR_LEAVE */

#ifndef LOG_ISTA_OVR_ENTER
#define LOG_ISTA_OVR_ENTER(tskid, ovrtim)
#endif /* LOG_ISTA_OVR_ENTER */

#ifndef LOG_ISTA_OVR_LEAVE
#define LOG_ISTA_OVR_LEAVE(ercd)
#endif /* LOG_ISTA_OVR_LEAVE */

#ifndef LOG_ISTP_OVR_ENTER
#define LOG_ISTP_OVR_ENTER(tskid)
#endif /* LOG_ISTP_OVR_ENTER */

#ifndef LOG_ISTP_OVR_LEAVE
#define LOG_ISTP_OVR_LEAVE(ercd)
#endif /* LOG_ISTP_OVR_LEAVE */

#ifndef LOG_STP_OVR_ENTER
#define LOG_STP_OVR_ENTER(tskid)
#endif /* LOG_STP_OVR_ENTER */

#ifndef LOG_STP_OVR_LEAVE
#define LOG_STP_OVR_LEAVE(ercd)
#endif /* LOG_STP_OVR_LEAVE */

#ifndef LOG_REF_OVR_ENTER
#define LOG_REF_OVR_ENTER(tskid, pk_rovr)
#endif /* LOG_REF_OVR_ENTER */

#ifndef LOG_REF_OVR_LEAVE
#define LOG_REF_OVR_LEAVE(ercd, pk_rovr)
#endif /* LOG_REF_OVR_LEAVE */

#ifdef TOPPERS_ovrini

/*
 *  the flag to show whether overrun timer is running
 */
bool_t	ovrtimer_flag;

/*
 *  overrun handler initialization function
 */
void
initialize_overrun(void)
{
	ovrtimer_flag = false;
}

#endif /* TOPPERS_ovrini */

/*
 *  start the overrun timer
 */
#ifdef TOPPERS_ovrsta
#ifndef OMIT_OVRTIMER_START

void
ovrtimer_start(void)
{
	if (p_runtsk->leftotm > 0U) {
		target_ovrtimer_start(p_runtsk->leftotm);
		ovrtimer_flag = true;
	}
}

#endif /* OMIT_OVRTIMER_START */
#endif /* TOPPERS_ovrsta */

/*
 *  stop the overrun timer
 */
#ifdef TOPPERS_ovrstp
#ifndef OMIT_OVRTIMER_STOP

void
ovrtimer_stop(void)
{
	if (ovrtimer_flag) {
		assert(p_runtsk->leftotm > 0U);
		p_runtsk->leftotm = target_ovrtimer_stop();
		ovrtimer_flag = false;
	}
}

#endif /* OMIT_OVRTIMER_STOP */
#endif /* TOPPERS_ovrstp */

/*
 *  start overrun handler
 */
#ifdef TOPPERS_sta_ovr

ER
sta_ovr(ID tskid, OVRTIM ovrtim)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_STA_OVR_ENTER(tskid, ovrtim);
	CHECK_TSKCTX_UNL();
	CHECK_OBJ(ovrinib.ovrhdr != NULL);
	CHECK_TSKID_SELF(tskid);
	CHECK_PAR(0U < ovrtim && ovrtim <= TMAX_OVRTIM);
	p_tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (p_tcb == p_runtsk) {
		if (p_tcb->leftotm > 0U) {
			(void) target_ovrtimer_stop();
		}
		target_ovrtimer_start(ovrtim);
		ovrtimer_flag = true;
	}
	p_tcb->leftotm = ovrtim;
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_STA_OVR_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sta_ovr */

/*
 *  start overrun handler (for non-task context)
 */
#ifdef TOPPERS_ista_ovr

ER
ista_ovr(ID tskid, OVRTIM ovrtim)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_ISTA_OVR_ENTER(tskid, ovrtim);
	CHECK_INTCTX_UNL();
	CHECK_OBJ(ovrinib.ovrhdr != NULL);
	CHECK_TSKID(tskid);
	CHECK_PAR(0U < ovrtim && ovrtim <= TMAX_OVRTIM);
	p_tcb = get_tcb(tskid);

	i_lock_cpu();
	p_tcb->leftotm = ovrtim;
	ercd = E_OK;
	i_unlock_cpu();

  error_exit:
	LOG_ISTA_OVR_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ista_ovr */

/*
 *  stop overrun handler
 */
#ifdef TOPPERS_stp_ovr

ER
stp_ovr(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_STP_OVR_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_OBJ(ovrinib.ovrhdr != NULL);
	CHECK_TSKID_SELF(tskid);
	p_tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (p_tcb->leftotm > 0U) {
		if (p_tcb == p_runtsk) {
			(void) target_ovrtimer_stop();
			ovrtimer_flag = false;
		}
		p_tcb->leftotm = 0U;
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_STP_OVR_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_stp_ovr */

/*
 *  stop overrun handleri (for non-task context)
 */
#ifdef TOPPERS_istp_ovr

ER
istp_ovr(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_ISTP_OVR_ENTER(tskid);
	CHECK_INTCTX_UNL();
	CHECK_OBJ(ovrinib.ovrhdr != NULL);
	CHECK_TSKID(tskid);
	p_tcb = get_tcb(tskid);

	i_lock_cpu();
	p_tcb->leftotm = 0U;
	ercd = E_OK;
	i_unlock_cpu();

  error_exit:
	LOG_ISTP_OVR_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_istp_ovr */

/*
 *  refer the status of overrun handler
 */
#ifdef TOPPERS_ref_ovr

ER
ref_ovr(ID tskid, T_ROVR *pk_rovr)
{
	TCB		*p_tcb;
	ER		ercd;
    
	LOG_REF_OVR_ENTER(tskid, pk_rovr);
	CHECK_TSKCTX_UNL();
	CHECK_OBJ(ovrinib.ovrhdr != NULL);
	CHECK_TSKID_SELF(tskid);
	p_tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (p_tcb->leftotm > 0U) {
		pk_rovr->ovrstat = TOVR_STA;
		if (p_tcb == p_runtsk) {
			pk_rovr->leftotm = target_ovrtimer_get_current();
		}
		else {
			pk_rovr->leftotm = p_tcb->leftotm;
		}
	}
	else {
		pk_rovr->ovrstat = TOVR_STP;
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_REF_OVR_LEAVE(ercd, pk_rovr);
	return(ercd);
}

#endif /* TOPPERS_ref_ovr */

/*
 *  call overrun handler
 */
#ifdef TOPPERS_ovrcal

void
call_ovrhdr(void)
{
	PRI		saved_ipm;

	assert(sense_context());
	assert(!i_sense_lock());
	assert(ovrinib.ovrhdr != NULL);

	p_runtsk->leftotm = 0U;
	saved_ipm = i_get_ipm();

	LOG_OVR_ENTER(p_runtsk);
	(*((OVRHDR)(ovrinib.ovrhdr)))(TSKID(p_runtsk), p_runtsk->p_tinib->exinf);
	LOG_OVR_LEAVE(p_runtsk);

	if (i_sense_lock()) {
		i_unlock_cpu();
	}
	i_set_ipm(saved_ipm);
}

#endif /* TOPPERS_ovrcal */
#endif /* TOPPERS_SUPPORT_OVRHDR */
