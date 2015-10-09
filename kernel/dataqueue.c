/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: dataqueue.c 1960 2010-11-20 02:20:40Z ertl-hiro $
 */

/*
 *	dataqueue function 
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "dataqueue.h"

/*
 *  default definition of trace macro
 */
#ifndef LOG_SND_DTQ_ENTER
#define LOG_SND_DTQ_ENTER(dtqid, data)
#endif /* LOG_SND_DTQ_ENTER */

#ifndef LOG_SND_DTQ_LEAVE
#define LOG_SND_DTQ_LEAVE(ercd)
#endif /* LOG_SND_DTQ_LEAVE */

#ifndef LOG_PSND_DTQ_ENTER
#define LOG_PSND_DTQ_ENTER(dtqid, data)
#endif /* LOG_PSND_DTQ_ENTER */

#ifndef LOG_PSND_DTQ_LEAVE
#define LOG_PSND_DTQ_LEAVE(ercd)
#endif /* LOG_PSND_DTQ_LEAVE */

#ifndef LOG_IPSND_DTQ_ENTER
#define LOG_IPSND_DTQ_ENTER(dtqid, data)
#endif /* LOG_IPSND_DTQ_ENTER */

#ifndef LOG_IPSND_DTQ_LEAVE
#define LOG_IPSND_DTQ_LEAVE(ercd)
#endif /* LOG_IPSND_DTQ_LEAVE */

#ifndef LOG_TSND_DTQ_ENTER
#define LOG_TSND_DTQ_ENTER(dtqid, data, tmout)
#endif /* LOG_TSND_DTQ_ENTER */

#ifndef LOG_TSND_DTQ_LEAVE
#define LOG_TSND_DTQ_LEAVE(ercd)
#endif /* LOG_TSND_DTQ_LEAVE */

#ifndef LOG_FSND_DTQ_ENTER
#define LOG_FSND_DTQ_ENTER(dtqid, data)
#endif /* LOG_FSND_DTQ_ENTER */

#ifndef LOG_FSND_DTQ_LEAVE
#define LOG_FSND_DTQ_LEAVE(ercd)
#endif /* LOG_FSND_DTQ_LEAVE */

#ifndef LOG_IFSND_DTQ_ENTER
#define LOG_IFSND_DTQ_ENTER(dtqid, data)
#endif /* LOG_IFSND_DTQ_ENTER */

#ifndef LOG_IFSND_DTQ_LEAVE
#define LOG_IFSND_DTQ_LEAVE(ercd)
#endif /* LOG_IFSND_DTQ_LEAVE */

#ifndef LOG_RCV_DTQ_ENTER
#define LOG_RCV_DTQ_ENTER(dtqid, p_data)
#endif /* LOG_RCV_DTQ_ENTER */

#ifndef LOG_RCV_DTQ_LEAVE
#define LOG_RCV_DTQ_LEAVE(ercd, data)
#endif /* LOG_RCV_DTQ_LEAVE */

#ifndef LOG_PRCV_DTQ_ENTER
#define LOG_PRCV_DTQ_ENTER(dtqid, p_data)
#endif /* LOG_PRCV_DTQ_ENTER */

#ifndef LOG_PRCV_DTQ_LEAVE
#define LOG_PRCV_DTQ_LEAVE(ercd, data)
#endif /* LOG_PRCV_DTQ_LEAVE */

#ifndef LOG_TRCV_DTQ_ENTER
#define LOG_TRCV_DTQ_ENTER(dtqid, p_data, tmout)
#endif /* LOG_TRCV_DTQ_ENTER */

#ifndef LOG_TRCV_DTQ_LEAVE
#define LOG_TRCV_DTQ_LEAVE(ercd, data)
#endif /* LOG_TRCV_DTQ_LEAVE */

#ifndef LOG_INI_DTQ_ENTER
#define LOG_INI_DTQ_ENTER(dtqid)
#endif /* LOG_INI_DTQ_ENTER */

#ifndef LOG_INI_DTQ_LEAVE
#define LOG_INI_DTQ_LEAVE(ercd)
#endif /* LOG_INI_DTQ_LEAVE */

#ifndef LOG_REF_DTQ_ENTER
#define LOG_REF_DTQ_ENTER(dtqid, pk_rdtq)
#endif /* LOG_REF_DTQ_ENTER */

#ifndef LOG_REF_DTQ_LEAVE
#define LOG_REF_DTQ_LEAVE(ercd, pk_rdtq)
#endif /* LOG_REF_DTQ_LEAVE */

/*
 *  total number of dataqueues
 */
#define tnum_dtq	((uint_t)(tmax_dtqid - TMIN_DTQID + 1))

/*
 *  get dataqueue control block from dataqueue ID
 */
#define INDEX_DTQ(dtqid)	((uint_t)((dtqid) - TMIN_DTQID))
#define get_dtqcb(dtqid)	(&(dtqcb_table[INDEX_DTQ(dtqid)]))

/*
 *  dataqueue initialization
 */
#ifdef TOPPERS_dtqini

void
initialize_dataqueue(void)
{
	uint_t	i;
	DTQCB	*p_dtqcb;

	for (p_dtqcb = dtqcb_table, i = 0; i < tnum_dtq; p_dtqcb++, i++) {
		queue_initialize(&(p_dtqcb->swait_queue));
		p_dtqcb->p_dtqinib = &(dtqinib_table[i]);
		queue_initialize(&(p_dtqcb->rwait_queue));
		p_dtqcb->count = 0U;
		p_dtqcb->head = 0U;
		p_dtqcb->tail = 0U;
	}
}

#endif /* TOPPERS_dtqini */

/*
 *  enqueue data into the array for dataqueue 
 */
#ifdef TOPPERS_dtqenq

void
enqueue_data(DTQCB *p_dtqcb, intptr_t data)
{
	(p_dtqcb->p_dtqinib->p_dtqmb + p_dtqcb->tail)->data = data;
	p_dtqcb->count++;
	p_dtqcb->tail++;
	if (p_dtqcb->tail >= p_dtqcb->p_dtqinib->dtqcnt) {
		p_dtqcb->tail = 0U;
	}
}

#endif /* TOPPERS_dtqenq */

/*
 *  force enqueue data. if dataqueue is full, the head will be covered by
 *  the tail
 */
#ifdef TOPPERS_dtqfenq

void
force_enqueue_data(DTQCB *p_dtqcb, intptr_t data)
{
	(p_dtqcb->p_dtqinib->p_dtqmb + p_dtqcb->tail)->data = data;
	p_dtqcb->tail++;
	if (p_dtqcb->tail >= p_dtqcb->p_dtqinib->dtqcnt) {
		p_dtqcb->tail = 0U;
	}
	if (p_dtqcb->count < p_dtqcb->p_dtqinib->dtqcnt) {
		p_dtqcb->count++;
	}
	else {
		p_dtqcb->head = p_dtqcb->tail;
	}
}

#endif /* TOPPERS_dtqfenq */

/*
 *  dequeue the data
 */
#ifdef TOPPERS_dtqdeq

void
dequeue_data(DTQCB *p_dtqcb, intptr_t *p_data)
{
	*p_data = (p_dtqcb->p_dtqinib->p_dtqmb + p_dtqcb->head)->data;
	p_dtqcb->count--;
	p_dtqcb->head++;
	if (p_dtqcb->head >= p_dtqcb->p_dtqinib->dtqcnt) {
		p_dtqcb->head = 0U;
	}
}

#endif /* TOPPERS_dtqdeq */

/*
 *  send data into dataqueue
 *  kernel will be told by p_reqdsp that whether a dispatch will be made
 */
#ifdef TOPPERS_dtqsnd

bool_t
send_data(DTQCB *p_dtqcb, intptr_t data, bool_t *p_reqdsp)
{
	TCB		*p_tcb;

	if (!queue_empty(&(p_dtqcb->rwait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_dtqcb->rwait_queue));
		((WINFO_DTQ *)(p_tcb->p_winfo))->data = data;
		*p_reqdsp = wait_complete(p_tcb);
		return(true);
	}
	else if (p_dtqcb->count < p_dtqcb->p_dtqinib->dtqcnt) {
		enqueue_data(p_dtqcb, data);
		*p_reqdsp = false;
		return(true);
	}
	else {
		return(false);
	}
}

#endif /* TOPPERS_dtqsnd */

/*
 * force to send data into dataqueue
 */
#ifdef TOPPERS_dtqfsnd

bool_t
force_send_data(DTQCB *p_dtqcb, intptr_t data)
{
	TCB		*p_tcb;

	if (!queue_empty(&(p_dtqcb->rwait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_dtqcb->rwait_queue));
		((WINFO_DTQ *)(p_tcb->p_winfo))->data = data;
		return(wait_complete(p_tcb));
	}
	else {
		force_enqueue_data(p_dtqcb, data);
		return(false);
	}
}

#endif /* TOPPERS_dtqfsnd */

/*
 *  receive data from dataqueue
 */
#ifdef TOPPERS_dtqrcv

bool_t
receive_data(DTQCB *p_dtqcb, intptr_t *p_data, bool_t *p_reqdsp)
{
	TCB		*p_tcb;
	intptr_t data;

	if (p_dtqcb->count > 0U) {
		dequeue_data(p_dtqcb, p_data);
		if (!queue_empty(&(p_dtqcb->swait_queue))) {
			p_tcb = (TCB *) queue_delete_next(&(p_dtqcb->swait_queue));
			data = ((WINFO_DTQ *)(p_tcb->p_winfo))->data;
			enqueue_data(p_dtqcb, data);
			*p_reqdsp = wait_complete(p_tcb);
		}
		else {
			*p_reqdsp = false;
		}
		return(true);
	}
	else if (!queue_empty(&(p_dtqcb->swait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_dtqcb->swait_queue));
		*p_data = ((WINFO_DTQ *)(p_tcb->p_winfo))->data;
		*p_reqdsp = wait_complete(p_tcb);
		return(true);
	}
	else {
		return(false);
	}
}

#endif /* TOPPERS_dtqrcv */

/*
 *  send data into dataqueue with waiting
 */
#ifdef TOPPERS_snd_dtq

ER
snd_dtq(ID dtqid, intptr_t data)
{
	DTQCB	*p_dtqcb;
	WINFO_DTQ winfo_dtq;
	bool_t	reqdsp;
	ER		ercd;

	LOG_SND_DTQ_ENTER(dtqid, data);
	CHECK_DISPATCH();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if (send_data(p_dtqcb, data, &reqdsp)) {
		if (reqdsp) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		winfo_dtq.data = data;
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_SDTQ);
		wobj_make_wait((WOBJCB *) p_dtqcb, (WINFO_WOBJ *) &winfo_dtq);
		dispatch();
		ercd = winfo_dtq.winfo.wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_SND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_snd_dtq */

/*
 *  send data into dataqueue without waiting
 */
#ifdef TOPPERS_psnd_dtq

ER
psnd_dtq(ID dtqid, intptr_t data)
{
	DTQCB	*p_dtqcb;
	bool_t	reqdsp;
	ER		ercd;

	LOG_PSND_DTQ_ENTER(dtqid, data);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if (send_data(p_dtqcb, data, &reqdsp)) {
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
	LOG_PSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_psnd_dtq */

/*
 *  send data into dataqueue, called in non-task context
 *  without waiting
 */
#ifdef TOPPERS_ipsnd_dtq

ER
ipsnd_dtq(ID dtqid, intptr_t data)
{
	DTQCB	*p_dtqcb;
	bool_t	reqdsp;
	ER		ercd;

	LOG_IPSND_DTQ_ENTER(dtqid, data);
	CHECK_INTCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	i_lock_cpu();
	if (send_data(p_dtqcb, data, &reqdsp)) {
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
	LOG_IPSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ipsnd_dtq */

/*
 *  send data into dataqueue with timeout
 */
#ifdef TOPPERS_tsnd_dtq

ER
tsnd_dtq(ID dtqid, intptr_t data, TMO tmout)
{
	DTQCB	*p_dtqcb;
	WINFO_DTQ winfo_dtq;
	TMEVTB	tmevtb;
	bool_t	reqdsp;
	ER		ercd;

	LOG_TSND_DTQ_ENTER(dtqid, data, tmout);
	CHECK_DISPATCH();
	CHECK_DTQID(dtqid);
	CHECK_TMOUT(tmout);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if (send_data(p_dtqcb, data, &reqdsp)) {
		if (reqdsp) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		winfo_dtq.data = data;
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_SDTQ);
		wobj_make_wait_tmout((WOBJCB *) p_dtqcb, (WINFO_WOBJ *) &winfo_dtq,
														&tmevtb, tmout);
		dispatch();
		ercd = winfo_dtq.winfo.wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_TSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_tsnd_dtq */

/*
 *  force to send data into datauque
 */
#ifdef TOPPERS_fsnd_dtq

ER
fsnd_dtq(ID dtqid, intptr_t data)
{
	DTQCB	*p_dtqcb;	
	ER		ercd;

	LOG_FSND_DTQ_ENTER(dtqid, data);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);
	CHECK_ILUSE(p_dtqcb->p_dtqinib->dtqcnt > 0U);

	t_lock_cpu();
	if (force_send_data(p_dtqcb, data)) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_FSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_fsnd_dtq */

/*
 * force to send data into datauque  called in non-task context
 */
#ifdef TOPPERS_ifsnd_dtq

ER
ifsnd_dtq(ID dtqid, intptr_t data)
{
	DTQCB	*p_dtqcb;
	ER		ercd;

	LOG_IFSND_DTQ_ENTER(dtqid, data);
	CHECK_INTCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);
	CHECK_ILUSE(p_dtqcb->p_dtqinib->dtqcnt > 0U);

	i_lock_cpu();
	if (force_send_data(p_dtqcb, data)) {
		reqflg = true;
	}
	ercd = E_OK;
	i_unlock_cpu();

  error_exit:
	LOG_IFSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ifsnd_dtq */

/*
 *  receive data from dataqueue
 */
#ifdef TOPPERS_rcv_dtq

ER
rcv_dtq(ID dtqid, intptr_t *p_data)
{
	DTQCB	*p_dtqcb;
	WINFO_DTQ winfo_dtq;
	bool_t	reqdsp;
	ER		ercd;

	LOG_RCV_DTQ_ENTER(dtqid, p_data);
	CHECK_DISPATCH();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if (receive_data(p_dtqcb, p_data, &reqdsp)) {
		if (reqdsp) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_RDTQ);
		make_wait(&(winfo_dtq.winfo));
		queue_insert_prev(&(p_dtqcb->rwait_queue), &(p_runtsk->task_queue));
		winfo_dtq.p_dtqcb = p_dtqcb;
		LOG_TSKSTAT(p_runtsk);
		dispatch();
		ercd = winfo_dtq.winfo.wercd;
		if (ercd == E_OK) {
			*p_data = winfo_dtq.data;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_RCV_DTQ_LEAVE(ercd, *p_data);
	return(ercd);
}

#endif /* TOPPERS_rcv_dtq */

/*
 *  receive data from dataqueue without waiting
 */
#ifdef TOPPERS_prcv_dtq

ER
prcv_dtq(ID dtqid, intptr_t *p_data)
{
	DTQCB	*p_dtqcb;
	bool_t	reqdsp;
	ER		ercd;

	LOG_PRCV_DTQ_ENTER(dtqid, p_data);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if (receive_data(p_dtqcb, p_data, &reqdsp)) {
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
	LOG_PRCV_DTQ_LEAVE(ercd, *p_data);
	return(ercd);
}

#endif /* TOPPERS_prcv_dtq */

/*
 *  receive data from dataqueue with timeout
 */
#ifdef TOPPERS_trcv_dtq

ER
trcv_dtq(ID dtqid, intptr_t *p_data, TMO tmout)
{
	DTQCB	*p_dtqcb;
	WINFO_DTQ winfo_dtq;
	TMEVTB	tmevtb;
	bool_t	reqdsp;
	ER		ercd;

	LOG_TRCV_DTQ_ENTER(dtqid, p_data, tmout);
	CHECK_DISPATCH();
	CHECK_DTQID(dtqid);
	CHECK_TMOUT(tmout);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if (receive_data(p_dtqcb, p_data, &reqdsp)) {
		if (reqdsp) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_RDTQ);
		make_wait_tmout(&(winfo_dtq.winfo), &tmevtb, tmout);
		queue_insert_prev(&(p_dtqcb->rwait_queue), &(p_runtsk->task_queue));
		winfo_dtq.p_dtqcb = p_dtqcb;
		LOG_TSKSTAT(p_runtsk);
		dispatch();
		ercd = winfo_dtq.winfo.wercd;
		if (ercd == E_OK) {
			*p_data = winfo_dtq.data;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_TRCV_DTQ_LEAVE(ercd, *p_data);
	return(ercd);
}

#endif /* TOPPERS_trcv_dtq */

/*
 *  reinitialize the dataqueue
 */
#ifdef TOPPERS_ini_dtq

ER
ini_dtq(ID dtqid)
{
	DTQCB	*p_dtqcb;
	bool_t	dspreq;
	ER		ercd;
    
	LOG_INI_DTQ_ENTER(dtqid);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	dspreq = init_wait_queue(&(p_dtqcb->swait_queue));
	if (init_wait_queue(&(p_dtqcb->rwait_queue))) {
		dspreq = true;
	};
	p_dtqcb->count = 0U;
	p_dtqcb->head = 0U;
	p_dtqcb->tail = 0U;
	if (dspreq) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_INI_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_dtq */

/*
 *  refer the status of dataqueue
 */
#ifdef TOPPERS_ref_dtq

ER
ref_dtq(ID dtqid, T_RDTQ *pk_rdtq)
{
	DTQCB	*p_dtqcb;
	ER		ercd;
    
	LOG_REF_DTQ_ENTER(dtqid, pk_rdtq);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	pk_rdtq->stskid = wait_tskid(&(p_dtqcb->swait_queue));
	pk_rdtq->rtskid = wait_tskid(&(p_dtqcb->rwait_queue));
	pk_rdtq->sdtqcnt = p_dtqcb->count;
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_REF_DTQ_LEAVE(ercd, pk_rdtq);
	return(ercd);
}

#endif /* TOPPERS_ref_dtq */
