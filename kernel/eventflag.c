/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2008 by Embedded and Real-Time Systems Laboratory
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
 *  	             (a) The above copyright notice, this use conditions, and the
 *         	disclaimer shown below must be shown without modification in
 *     		the document provided with the redistributed software, such as
 *      		the user manual.
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
 *  @(#) $Id: eventflag.c 748 2008-03-07 17:18:06Z hiro $
 */

/*
 *	 eventflag function
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "eventflag.h"

/*
 *   default definition of trace macro
 */
#ifndef LOG_SET_FLG_ENTER
#define LOG_SET_FLG_ENTER(flgid, setptn)
#endif /* LOG_SET_FLG_ENTER */

#ifndef LOG_SET_FLG_LEAVE
#define LOG_SET_FLG_LEAVE(ercd)
#endif /* LOG_SET_FLG_LEAVE */

#ifndef LOG_ISET_FLG_ENTER
#define LOG_ISET_FLG_ENTER(flgid, setptn)
#endif /* LOG_ISET_FLG_ENTER */

#ifndef LOG_ISET_FLG_LEAVE
#define LOG_ISET_FLG_LEAVE(ercd)
#endif /* LOG_ISET_FLG_LEAVE */

#ifndef LOG_CLR_FLG_ENTER
#define LOG_CLR_FLG_ENTER(flgid, clrptn)
#endif /* LOG_CLR_FLG_ENTER */

#ifndef LOG_CLR_FLG_LEAVE
#define LOG_CLR_FLG_LEAVE(ercd)
#endif /* LOG_CLR_FLG_LEAVE */

#ifndef LOG_WAI_FLG_ENTER
#define LOG_WAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn)
#endif /* LOG_WAI_FLG_ENTER */

#ifndef LOG_WAI_FLG_LEAVE
#define LOG_WAI_FLG_LEAVE(ercd, flgptn)
#endif /* LOG_WAI_FLG_LEAVE */

#ifndef LOG_POL_FLG_ENTER
#define LOG_POL_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn)
#endif /* LOG_POL_FLG_ENTER */

#ifndef LOG_POL_FLG_LEAVE
#define LOG_POL_FLG_LEAVE(ercd, flgptn)
#endif /* LOG_POL_FLG_LEAVE */

#ifndef LOG_TWAI_FLG_ENTER
#define LOG_TWAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn, tmout)
#endif /* LOG_TWAI_FLG_ENTER */

#ifndef LOG_TWAI_FLG_LEAVE
#define LOG_TWAI_FLG_LEAVE(ercd, flgptn)
#endif /* LOG_TWAI_FLG_LEAVE */

#ifndef LOG_INI_FLG_ENTER
#define LOG_INI_FLG_ENTER(flgid)
#endif /* LOG_INI_FLG_ENTER */

#ifndef LOG_INI_FLG_LEAVE
#define LOG_INI_FLG_LEAVE(ercd)
#endif /* LOG_INI_FLG_LEAVE */

#ifndef LOG_REF_FLG_ENTER
#define LOG_REF_FLG_ENTER(flgid, pk_rflg)
#endif /* LOG_REF_FLG_ENTER */

#ifndef LOG_REF_FLG_LEAVE
#define LOG_REF_FLG_LEAVE(ercd, pk_rflg)
#endif /* LOG_REF_FLG_LEAVE */

/*
 *  total number of eventflag
 */
#define tnum_flg	((uint_t)(tmax_flgid - TMIN_FLGID + 1))

/*
 *  get event control block from eventflag id
 */
#define INDEX_FLG(flgid)	((uint_t)((flgid) - TMIN_FLGID))
#define get_flgcb(flgid)	(&(flgcb_table[INDEX_FLG(flgid)]))

/*
 *  eventflag initialization
 */
#ifdef TOPPERS_flgini

void
initialize_eventflag(void)
{
	uint_t	i;
	FLGCB	*p_flgcb;

	for (p_flgcb = flgcb_table, i = 0; i < tnum_flg; p_flgcb++, i++) {
		queue_initialize(&(p_flgcb->wait_queue));
		p_flgcb->p_flginib = &(flginib_table[i]);
		p_flgcb->flgptn = p_flgcb->p_flginib->iflgptn;
	}
}

#endif /* TOPPERS_flgini */

/*
 *  check eventflag condition accroding to waiting mode
 */
#ifdef TOPPERS_flgcnd

bool_t
check_flg_cond(FLGCB *p_flgcb, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn)
{
	if ((wfmode & TWF_ORW) != 0U ? (p_flgcb->flgptn & waiptn) != 0U
									: (p_flgcb->flgptn & waiptn) == waiptn) {
		*p_flgptn = p_flgcb->flgptn;
		if ((p_flgcb->p_flginib->flgatr & TA_CLR) != 0U) {
			p_flgcb->flgptn = 0U;
		}
		return(true);
	}
	return(false);
}

#endif /* TOPPERS_flgcnd */

/*
 *  set eventflag
 */
#ifdef TOPPERS_set_flg

ER
set_flg(ID flgid, FLGPTN setptn)
{
	FLGCB	*p_flgcb;
	QUEUE	*p_queue;
	TCB		*p_tcb;
	WINFO_FLG *p_winfo_flg;
	bool_t	dspreq = false;
	ER		ercd;

	LOG_SET_FLG_ENTER(flgid, setptn);
	CHECK_TSKCTX_UNL();
	CHECK_FLGID(flgid);
	p_flgcb = get_flgcb(flgid);

	t_lock_cpu();
	p_flgcb->flgptn |= setptn;
	p_queue = p_flgcb->wait_queue.p_next;
	while (p_queue != &(p_flgcb->wait_queue)) {
		p_tcb = (TCB *) p_queue;
		p_queue = p_queue->p_next;
		p_winfo_flg = (WINFO_FLG *)(p_tcb->p_winfo);
		if (check_flg_cond(p_flgcb, p_winfo_flg->waiptn,
							p_winfo_flg->wfmode, &(p_winfo_flg->flgptn))) {
			queue_delete(&(p_tcb->task_queue));
			if (wait_complete(p_tcb)) {
				dspreq = true;
			}
			if ((p_flgcb->p_flginib->flgatr & TA_CLR) != 0U) {
				break;
			}
		}
	}
	if (dspreq) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_SET_FLG_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_set_flg */

/*
 *  set eventflag called in non-task context
 */
#ifdef TOPPERS_iset_flg

ER
iset_flg(ID flgid, FLGPTN setptn)
{
	FLGCB	*p_flgcb;
	QUEUE	*p_queue;
	TCB		*p_tcb;
	WINFO_FLG *p_winfo_flg;
	ER		ercd;

	LOG_ISET_FLG_ENTER(flgid, setptn);
	CHECK_INTCTX_UNL();
	CHECK_FLGID(flgid);
	p_flgcb = get_flgcb(flgid);

	i_lock_cpu();
	p_flgcb->flgptn |= setptn;
	p_queue = p_flgcb->wait_queue.p_next;
	while (p_queue != &(p_flgcb->wait_queue)) {
		p_tcb = (TCB *) p_queue;
		p_queue = p_queue->p_next;
		p_winfo_flg = (WINFO_FLG *)(p_tcb->p_winfo);
		if (check_flg_cond(p_flgcb, p_winfo_flg->waiptn,
							p_winfo_flg->wfmode, &(p_winfo_flg->flgptn))) {
			queue_delete(&(p_tcb->task_queue));
			if (wait_complete(p_tcb)) {
				reqflg = true;
			}
			if ((p_flgcb->p_flginib->flgatr & TA_CLR) != 0U) {
				break;
			}
		}
	}
	ercd = E_OK;
	i_unlock_cpu();

  error_exit:
	LOG_ISET_FLG_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iset_flg */

/*
 *  clear eventflag
 */
#ifdef TOPPERS_clr_flg

ER
clr_flg(ID flgid, FLGPTN clrptn)
{
	FLGCB	*p_flgcb;
	ER		ercd;

	LOG_CLR_FLG_ENTER(flgid, clrptn);
	CHECK_TSKCTX_UNL();
	CHECK_FLGID(flgid);
	p_flgcb = get_flgcb(flgid);

	t_lock_cpu();
	p_flgcb->flgptn &= clrptn; 
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_CLR_FLG_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_clr_flg */

/*
 *  wait for eventflag
 */
#ifdef TOPPERS_wai_flg

ER
wai_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn)
{
	FLGCB	*p_flgcb;
	WINFO_FLG winfo_flg;
	ER		ercd;

	LOG_WAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn);
	CHECK_DISPATCH();
	CHECK_FLGID(flgid);
	CHECK_PAR(waiptn != 0U);
	CHECK_PAR(wfmode == TWF_ORW || wfmode == TWF_ANDW);
	p_flgcb = get_flgcb(flgid);

	t_lock_cpu();
	if ((p_flgcb->p_flginib->flgatr & TA_WMUL) == 0U
					&& !queue_empty(&(p_flgcb->wait_queue))) {
		ercd = E_ILUSE;
	}
	else if (check_flg_cond(p_flgcb, waiptn, wfmode, p_flgptn)) {
		ercd = E_OK;
	}
	else {
		winfo_flg.waiptn = waiptn;
		winfo_flg.wfmode = wfmode;
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_FLG);
		wobj_make_wait((WOBJCB *) p_flgcb, (WINFO_WOBJ *) &winfo_flg);
		dispatch();
		ercd = winfo_flg.winfo.wercd;
		if (ercd == E_OK) {
			*p_flgptn = winfo_flg.flgptn;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_WAI_FLG_LEAVE(ercd, *p_flgptn);
	return(ercd);
}

#endif /* TOPPERS_wai_flg */

/*
 *  wait for eventflag by polling
 */
#ifdef TOPPERS_pol_flg

ER
pol_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn)
{
	FLGCB	*p_flgcb;
	ER		ercd;

	LOG_POL_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn);
	CHECK_TSKCTX_UNL();
	CHECK_FLGID(flgid);
	CHECK_PAR(waiptn != 0U);
	CHECK_PAR(wfmode == TWF_ORW || wfmode == TWF_ANDW);
	p_flgcb = get_flgcb(flgid);

	t_lock_cpu();
	if ((p_flgcb->p_flginib->flgatr & TA_WMUL) == 0U
					&& !queue_empty(&(p_flgcb->wait_queue))) {
		ercd = E_ILUSE;
	}
	else if (check_flg_cond(p_flgcb, waiptn, wfmode, p_flgptn)) {
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

  error_exit:
	LOG_POL_FLG_LEAVE(ercd, *p_flgptn);
	return(ercd);
}

#endif /* TOPPERS_pol_flg */

/*
 *  wait for eventflag with timeout
 */
#ifdef TOPPERS_twai_flg

ER
twai_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn, TMO tmout)
{
	FLGCB	*p_flgcb;
	WINFO_FLG winfo_flg;
	TMEVTB	tmevtb;
	ER		ercd;

	LOG_TWAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn, tmout);
	CHECK_DISPATCH();
	CHECK_FLGID(flgid);
	CHECK_PAR(waiptn != 0U);
	CHECK_PAR(wfmode == TWF_ORW || wfmode == TWF_ANDW);
	CHECK_TMOUT(tmout);
	p_flgcb = get_flgcb(flgid);

	t_lock_cpu();
	if ((p_flgcb->p_flginib->flgatr & TA_WMUL) == 0U
					&& !queue_empty(&(p_flgcb->wait_queue))) {
		ercd = E_ILUSE;
	}
	else if (check_flg_cond(p_flgcb, waiptn, wfmode, p_flgptn)) {
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		winfo_flg.waiptn = waiptn;
		winfo_flg.wfmode = wfmode;
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_FLG);
		wobj_make_wait_tmout((WOBJCB *) p_flgcb, (WINFO_WOBJ *) &winfo_flg,
														&tmevtb, tmout);
		dispatch();
		ercd = winfo_flg.winfo.wercd;
		if (ercd == E_OK) {
			*p_flgptn = winfo_flg.flgptn;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_TWAI_FLG_LEAVE(ercd, *p_flgptn);
	return(ercd);
}

#endif /* TOPPERS_twai_flg */

/*
 *  reinitiate eventflag
 *  all eventflags have been initiated statically before kernel runs
 *  eventflag can be initiated again by this function
 */
#ifdef TOPPERS_ini_flg

ER
ini_flg(ID flgid)
{
	FLGCB	*p_flgcb;
	bool_t	dspreq;
	ER		ercd;
    
	LOG_INI_FLG_ENTER(flgid);
	CHECK_TSKCTX_UNL();
	CHECK_FLGID(flgid);
	p_flgcb = get_flgcb(flgid);

	t_lock_cpu();
	dspreq = init_wait_queue(&(p_flgcb->wait_queue));
	p_flgcb->flgptn = p_flgcb->p_flginib->iflgptn;
	if (dspreq) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_INI_FLG_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_flg */

/*
 *  refer the status of eventflag
 */
#ifdef TOPPERS_ref_flg

ER
ref_flg(ID flgid, T_RFLG *pk_rflg)
{
	FLGCB	*p_flgcb;
	ER		ercd;
    
	LOG_REF_FLG_ENTER(flgid, pk_rflg);
	CHECK_TSKCTX_UNL();
	CHECK_FLGID(flgid);
	p_flgcb = get_flgcb(flgid);

	t_lock_cpu();
	pk_rflg->wtskid = wait_tskid(&(p_flgcb->wait_queue));
	pk_rflg->flgptn = p_flgcb->flgptn;
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_REF_FLG_LEAVE(ercd, pk_rflg);
	return(ercd);
}

#endif /* TOPPERS_ref_flg */
