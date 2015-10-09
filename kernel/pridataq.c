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
 *  @(#) $Id: pridataq.c 1774 2010-03-19 12:45:46Z ertl-hiro $
 *
 */


/*
 *	priority dataqueue function
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "pridataq.h"

/*
 *   default definition of trace macro
 */
#ifndef LOG_SND_PDQ_ENTER
#define LOG_SND_PDQ_ENTER(pdqid, data, datapri)
#endif /* LOG_SND_PDQ_ENTER */

#ifndef LOG_SND_PDQ_LEAVE
#define LOG_SND_PDQ_LEAVE(ercd)
#endif /* LOG_SND_PDQ_LEAVE */

#ifndef LOG_PSND_PDQ_ENTER
#define LOG_PSND_PDQ_ENTER(pdqid, data, datapri)
#endif /* LOG_PSND_PDQ_ENTER */

#ifndef LOG_PSND_PDQ_LEAVE
#define LOG_PSND_PDQ_LEAVE(ercd)
#endif /* LOG_PSND_PDQ_LEAVE */

#ifndef LOG_IPSND_PDQ_ENTER
#define LOG_IPSND_PDQ_ENTER(pdqid, data, datapri)
#endif /* LOG_IPSND_PDQ_ENTER */

#ifndef LOG_IPSND_PDQ_LEAVE
#define LOG_IPSND_PDQ_LEAVE(ercd)
#endif /* LOG_IPSND_PDQ_LEAVE */

#ifndef LOG_TSND_PDQ_ENTER
#define LOG_TSND_PDQ_ENTER(pdqid, data, datapri, tmout)
#endif /* LOG_TSND_PDQ_ENTER */

#ifndef LOG_TSND_PDQ_LEAVE
#define LOG_TSND_PDQ_LEAVE(ercd)
#endif /* LOG_TSND_PDQ_LEAVE */

#ifndef LOG_RCV_PDQ_ENTER
#define LOG_RCV_PDQ_ENTER(pdqid, p_data, p_datapri)
#endif /* LOG_RCV_PDQ_ENTER */

#ifndef LOG_RCV_PDQ_LEAVE
#define LOG_RCV_PDQ_LEAVE(ercd, data, datapri)
#endif /* LOG_RCV_PDQ_LEAVE */

#ifndef LOG_PRCV_PDQ_ENTER
#define LOG_PRCV_PDQ_ENTER(pdqid, p_data, p_datapri)
#endif /* LOG_PRCV_PDQ_ENTER */

#ifndef LOG_PRCV_PDQ_LEAVE
#define LOG_PRCV_PDQ_LEAVE(ercd, data, datapri)
#endif /* LOG_PRCV_PDQ_LEAVE */

#ifndef LOG_TRCV_PDQ_ENTER
#define LOG_TRCV_PDQ_ENTER(pdqid, p_data, p_datapri, tmout)
#endif /* LOG_TRCV_PDQ_ENTER */

#ifndef LOG_TRCV_PDQ_LEAVE
#define LOG_TRCV_PDQ_LEAVE(ercd, data, datapri)
#endif /* LOG_TRCV_PDQ_LEAVE */

#ifndef LOG_INI_PDQ_ENTER
#define LOG_INI_PDQ_ENTER(pdqid)
#endif /* LOG_INI_PDQ_ENTER */

#ifndef LOG_INI_PDQ_LEAVE
#define LOG_INI_PDQ_LEAVE(ercd)
#endif /* LOG_INI_PDQ_LEAVE */

#ifndef LOG_REF_PDQ_ENTER
#define LOG_REF_PDQ_ENTER(pdqid, pk_rpdq)
#endif /* LOG_REF_PDQ_ENTER */

#ifndef LOG_REF_PDQ_LEAVE
#define LOG_REF_PDQ_LEAVE(ercd, pk_rpdq)
#endif /* LOG_REF_PDQ_LEAVE */

/*
 *  total number of priority dataqueues
 */
#define tnum_pdq	((uint_t)(tmax_pdqid - TMIN_PDQID + 1))

/*
 *  get priority dataqueue control block from priority dataqueue id
 */
#define INDEX_PDQ(pdqid)	((uint_t)((pdqid) - TMIN_PDQID))
#define get_pdqcb(pdqid)	(&(pdqcb_table[INDEX_PDQ(pdqid)]))

/*
 *  priority dataqueue initialization
 */
#ifdef TOPPERS_pdqini

void
initialize_pridataq(void)
{
	uint_t	i;
	PDQCB	*p_pdqcb;

	for (p_pdqcb = pdqcb_table, i = 0; i < tnum_pdq; p_pdqcb++, i++) {
		queue_initialize(&(p_pdqcb->swait_queue));
		p_pdqcb->p_pdqinib = &(pdqinib_table[i]);
		queue_initialize(&(p_pdqcb->rwait_queue));
		p_pdqcb->count = 0U;
		p_pdqcb->p_head = NULL;
		p_pdqcb->unused = 0U;
		p_pdqcb->p_freelist = NULL;
	}
}

#endif /* TOPPERS_pdqini */

/*
 *  enqueue data to priority dataqueue management block
 */
#ifdef TOPPERS_pdqenq

void
enqueue_pridata(PDQCB *p_pdqcb, intptr_t data, PRI datapri)
{
	PDQMB	*p_pdqmb;
	PDQMB	**pp_prev_next, *p_next;

	if (p_pdqcb->p_freelist != NULL) {
		p_pdqmb = p_pdqcb->p_freelist;
		p_pdqcb->p_freelist = p_pdqmb->p_next;
	}
	else {
		p_pdqmb = p_pdqcb->p_pdqinib->p_pdqmb + p_pdqcb->unused;
		p_pdqcb->unused++;
	}

	p_pdqmb->data = data;
	p_pdqmb->datapri = datapri;

	pp_prev_next = &(p_pdqcb->p_head);
	while ((p_next = *pp_prev_next) != NULL) {
		if (p_next->datapri > datapri) {
			break;
		}
		pp_prev_next = &(p_next->p_next);
	}
	p_pdqmb->p_next = p_next;
	*pp_prev_next = p_pdqmb;
	p_pdqcb->count++;
}

#endif /* TOPPERS_pdqenq */

/*
 * dequeue data from priority dataqueue management block
 */
#ifdef TOPPERS_pdqdeq

void
dequeue_pridata(PDQCB *p_pdqcb, intptr_t *p_data, PRI *p_datapri)
{
	PDQMB	*p_pdqmb;

	p_pdqmb = p_pdqcb->p_head;
	p_pdqcb->p_head = p_pdqmb->p_next;
	p_pdqcb->count--;

	*p_data = p_pdqmb->data;
	*p_datapri = p_pdqmb->datapri;

	p_pdqmb->p_next = p_pdqcb->p_freelist;
	p_pdqcb->p_freelist = p_pdqmb;
}

#endif /* TOPPERS_pdqdeq */

/*
 *  send data into priority dataqueue through priority dataqueue control block
 */
#ifdef TOPPERS_pdqsnd

bool_t
send_pridata(PDQCB *p_pdqcb, intptr_t data, PRI datapri, bool_t *p_reqdsp)
{
	TCB		*p_tcb;

	if (!queue_empty(&(p_pdqcb->rwait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_pdqcb->rwait_queue));
		((WINFO_PDQ *)(p_tcb->p_winfo))->data = data;
		((WINFO_PDQ *)(p_tcb->p_winfo))->datapri = datapri;
		*p_reqdsp = wait_complete(p_tcb);
		return(true);
	}
	else if (p_pdqcb->count < p_pdqcb->p_pdqinib->pdqcnt) {
		enqueue_pridata(p_pdqcb, data, datapri);
		*p_reqdsp = false;
		return(true);
	}
	else {
		return(false);
	}
}

#endif /* TOPPERS_pdqsnd */

/*
 *  receive data from priority dataqueue
 *  through priority dataqueue control block
 */
#ifdef TOPPERS_pdqrcv

bool_t
receive_pridata(PDQCB *p_pdqcb, intptr_t *p_data,
									PRI *p_datapri, bool_t *p_reqdsp)
{
	TCB		*p_tcb;
	intptr_t data;
	PRI		datapri;

	if (p_pdqcb->count > 0U) {
		dequeue_pridata(p_pdqcb, p_data, p_datapri);
		if (!queue_empty(&(p_pdqcb->swait_queue))) {
			p_tcb = (TCB *) queue_delete_next(&(p_pdqcb->swait_queue));
			data = ((WINFO_PDQ *)(p_tcb->p_winfo))->data;
			datapri = ((WINFO_PDQ *)(p_tcb->p_winfo))->datapri;
			enqueue_pridata(p_pdqcb, data, datapri);
			*p_reqdsp = wait_complete(p_tcb);
		}
		else {
			*p_reqdsp = false;
		}
		return(true);
	}
	else if (!queue_empty(&(p_pdqcb->swait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_pdqcb->swait_queue));
		*p_data = ((WINFO_PDQ *)(p_tcb->p_winfo))->data;
		*p_datapri = ((WINFO_PDQ *)(p_tcb->p_winfo))->datapri;
		*p_reqdsp = wait_complete(p_tcb);
		return(true);
	}
	else {
		return(false);
	}
}

#endif /* TOPPERS_pdqrcv */

/*
 *   send data into priority dataqueue through id
 */
#ifdef TOPPERS_snd_pdq

ER
snd_pdq(ID pdqid, intptr_t data, PRI datapri)
{
	PDQCB	*p_pdqcb;
	WINFO_PDQ winfo_pdq;
	bool_t	reqdsp;
	ER		ercd;

	LOG_SND_PDQ_ENTER(pdqid, data, datapri);
	CHECK_DISPATCH();
	CHECK_PDQID(pdqid);
	p_pdqcb = get_pdqcb(pdqid);
	CHECK_PAR(TMIN_DPRI <= datapri && datapri <= p_pdqcb->p_pdqinib->maxdpri);

	t_lock_cpu();
	if (send_pridata(p_pdqcb, data, datapri, &reqdsp)) {
		if (reqdsp) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		winfo_pdq.data = data;
		winfo_pdq.datapri = datapri;
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_SPDQ);
		wobj_make_wait((WOBJCB *) p_pdqcb, (WINFO_WOBJ *) &winfo_pdq);
		dispatch();
		ercd = winfo_pdq.winfo.wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_SND_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_snd_pdq */

/*
 *  send data into priority dataqueue by polling
 */
#ifdef TOPPERS_psnd_pdq

ER
psnd_pdq(ID pdqid, intptr_t data, PRI datapri)
{
	PDQCB	*p_pdqcb;
	bool_t	reqdsp;
	ER		ercd;

	LOG_PSND_PDQ_ENTER(pdqid, data, datapri);
	CHECK_TSKCTX_UNL();
	CHECK_PDQID(pdqid);
	p_pdqcb = get_pdqcb(pdqid);
	CHECK_PAR(TMIN_DPRI <= datapri && datapri <= p_pdqcb->p_pdqinib->maxdpri);

	t_lock_cpu();
	if (send_pridata(p_pdqcb, data, datapri, &reqdsp)) {
		if (reqdsp) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

  error_exit:
	LOG_PSND_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_psnd_pdq */

/*
 *  send data into priority dataqueue by polling (used in
 *  non-task context)
 */
#ifdef TOPPERS_ipsnd_pdq

ER
ipsnd_pdq(ID pdqid, intptr_t data, PRI datapri)
{
	PDQCB	*p_pdqcb;
	bool_t	reqdsp;
	ER		ercd;

	LOG_IPSND_PDQ_ENTER(pdqid, data, datapri);
	CHECK_INTCTX_UNL();
	CHECK_PDQID(pdqid);
	p_pdqcb = get_pdqcb(pdqid);
	CHECK_PAR(TMIN_DPRI <= datapri && datapri <= p_pdqcb->p_pdqinib->maxdpri);

	i_lock_cpu();
	if (send_pridata(p_pdqcb, data, datapri, &reqdsp)) {
		if (reqdsp) {
			reqflg = true;
		}
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	i_unlock_cpu();

  error_exit:
	LOG_IPSND_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ipsnd_pdq */

/*
 *  send data into priority dataqueue(with timeout)
 */
#ifdef TOPPERS_tsnd_pdq

ER
tsnd_pdq(ID pdqid, intptr_t data, PRI datapri, TMO tmout)
{
	PDQCB	*p_pdqcb;
	WINFO_PDQ winfo_pdq;
	TMEVTB	tmevtb;
	bool_t	reqdsp;
	ER		ercd;

	LOG_TSND_PDQ_ENTER(pdqid, data, datapri, tmout);
	CHECK_DISPATCH();
	CHECK_PDQID(pdqid);
	CHECK_TMOUT(tmout);
	p_pdqcb = get_pdqcb(pdqid);
	CHECK_PAR(TMIN_DPRI <= datapri && datapri <= p_pdqcb->p_pdqinib->maxdpri);

	t_lock_cpu();
	if (send_pridata(p_pdqcb, data, datapri, &reqdsp)) {
		if (reqdsp) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		winfo_pdq.data = data;
		winfo_pdq.datapri = datapri;
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_SPDQ);
		wobj_make_wait_tmout((WOBJCB *) p_pdqcb, (WINFO_WOBJ *) &winfo_pdq,
														&tmevtb, tmout);
		dispatch();
		ercd = winfo_pdq.winfo.wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_TSND_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_tsnd_pdq */

/*
 *  receive data from priority dataqueue
 */
#ifdef TOPPERS_rcv_pdq

ER
rcv_pdq(ID pdqid, intptr_t *p_data, PRI *p_datapri)
{
	PDQCB	*p_pdqcb;
	WINFO_PDQ winfo_pdq;
	bool_t	reqdsp;
	ER		ercd;

	LOG_RCV_PDQ_ENTER(pdqid, p_data, p_datapri);
	CHECK_DISPATCH();
	CHECK_PDQID(pdqid);
	p_pdqcb = get_pdqcb(pdqid);

	t_lock_cpu();
	if (receive_pridata(p_pdqcb, p_data, p_datapri, &reqdsp)) {
		if (reqdsp) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_RPDQ);
		make_wait(&(winfo_pdq.winfo));
		queue_insert_prev(&(p_pdqcb->rwait_queue), &(p_runtsk->task_queue));
		winfo_pdq.p_pdqcb = p_pdqcb;
		LOG_TSKSTAT(p_runtsk);
		dispatch();
		ercd = winfo_pdq.winfo.wercd;
		if (ercd == E_OK) {
			*p_data = winfo_pdq.data;
			*p_datapri = winfo_pdq.datapri;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_RCV_PDQ_LEAVE(ercd, *p_data, *p_datapri);
	return(ercd);
}

#endif /* TOPPERS_rcv_pdq */

/*
 *  receive data from priority dataqueue by polling
 */
#ifdef TOPPERS_prcv_pdq

ER
prcv_pdq(ID pdqid, intptr_t *p_data, PRI *p_datapri)
{
	PDQCB	*p_pdqcb;
	bool_t	reqdsp;
	ER		ercd;

	LOG_PRCV_PDQ_ENTER(pdqid, p_data, p_datapri);
	CHECK_TSKCTX_UNL();
	CHECK_PDQID(pdqid);
	p_pdqcb = get_pdqcb(pdqid);

	t_lock_cpu();
	if (receive_pridata(p_pdqcb, p_data, p_datapri, &reqdsp)) {
		if (reqdsp) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

  error_exit:
	LOG_PRCV_PDQ_LEAVE(ercd, *p_data, *p_datapri);
	return(ercd);
}

#endif /* TOPPERS_prcv_pdq */

/*
 *  receive data from priority dataqueue with timeout
 */
#ifdef TOPPERS_trcv_pdq

ER
trcv_pdq(ID pdqid, intptr_t *p_data, PRI *p_datapri, TMO tmout)
{
	PDQCB	*p_pdqcb;
	WINFO_PDQ winfo_pdq;
	TMEVTB	tmevtb;
	bool_t	reqdsp;
	ER		ercd;

	LOG_TRCV_PDQ_ENTER(pdqid, p_data, p_datapri, tmout);
	CHECK_DISPATCH();
	CHECK_PDQID(pdqid);
	CHECK_TMOUT(tmout);
	p_pdqcb = get_pdqcb(pdqid);

	t_lock_cpu();
	if (receive_pridata(p_pdqcb, p_data, p_datapri, &reqdsp)) {
		if (reqdsp) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_RPDQ);
		make_wait_tmout(&(winfo_pdq.winfo), &tmevtb, tmout);
		queue_insert_prev(&(p_pdqcb->rwait_queue), &(p_runtsk->task_queue));
		winfo_pdq.p_pdqcb = p_pdqcb;
		LOG_TSKSTAT(p_runtsk);
		dispatch();
		ercd = winfo_pdq.winfo.wercd;
		if (ercd == E_OK) {
			*p_data = winfo_pdq.data;
			*p_datapri = winfo_pdq.datapri;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_TRCV_PDQ_LEAVE(ercd, *p_data, *p_datapri);
	return(ercd);
}

#endif /* TOPPERS_trcv_pdq */

/*
 *  re-init priority dataqueue
 */
#ifdef TOPPERS_ini_pdq

ER
ini_pdq(ID pdqid)
{
	PDQCB	*p_pdqcb;
	bool_t	dspreq;
	ER		ercd;
    
	LOG_INI_PDQ_ENTER(pdqid);
	CHECK_TSKCTX_UNL();
	CHECK_PDQID(pdqid);
	p_pdqcb = get_pdqcb(pdqid);

	t_lock_cpu();
	dspreq = init_wait_queue(&(p_pdqcb->swait_queue));
	if (init_wait_queue(&(p_pdqcb->rwait_queue))) {
		dspreq = true;
	}
	p_pdqcb->count = 0U;
	p_pdqcb->p_head = NULL;
	p_pdqcb->unused = 0U;
	p_pdqcb->p_freelist = NULL;
	if (dspreq) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_INI_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_pdq */

/*
 *  refer the status of priority dataqueue
 */
#ifdef TOPPERS_ref_pdq

ER
ref_pdq(ID pdqid, T_RPDQ *pk_rpdq)
{
	PDQCB	*p_pdqcb;
	ER		ercd;
    
	LOG_REF_PDQ_ENTER(pdqid, pk_rpdq);
	CHECK_TSKCTX_UNL();
	CHECK_PDQID(pdqid);
	p_pdqcb = get_pdqcb(pdqid);

	t_lock_cpu();
	pk_rpdq->stskid = wait_tskid(&(p_pdqcb->swait_queue));
	pk_rpdq->rtskid = wait_tskid(&(p_pdqcb->rwait_queue));
	pk_rpdq->spdqcnt = p_pdqcb->count;
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_REF_PDQ_LEAVE(ercd, pk_rpdq);
	return(ercd);
}

#endif /* TOPPERS_ref_pdq */
