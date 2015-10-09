/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2005-2009 by Embedded and Real-Time Systems Laboratory
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
 */

/*
 *		mutex function
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "mutex.h"

/*
 *   default definition of trace macro
 */
#ifndef LOG_LOC_MTX_ENTER
#define LOG_LOC_MTX_ENTER(mtxid)
#endif /* LOG_LOC_MTX_ENTER */

#ifndef LOG_LOC_MTX_LEAVE
#define LOG_LOC_MTX_LEAVE(ercd)
#endif /* LOG_LOC_MTX_LEAVE */

#ifndef LOG_PLOC_MTX_ENTER
#define LOG_PLOC_MTX_ENTER(mtxid)
#endif /* LOG_PLOC_MTX_ENTER */

#ifndef LOG_PLOC_MTX_LEAVE
#define LOG_PLOC_MTX_LEAVE(ercd)
#endif /* LOG_PLOC_MTX_LEAVE */

#ifndef LOG_TLOC_MTX_ENTER
#define LOG_TLOC_MTX_ENTER(mtxid, tmout)
#endif /* LOG_TLOC_MTX_ENTER */

#ifndef LOG_TLOC_MTX_LEAVE
#define LOG_TLOC_MTX_LEAVE(ercd)
#endif /* LOG_TLOC_MTX_LEAVE */

#ifndef LOG_UNL_MTX_ENTER
#define LOG_UNL_MTX_ENTER(mtxid)
#endif /* LOG_UNL_MTX_ENTER */

#ifndef LOG_UNL_MTX_LEAVE
#define LOG_UNL_MTX_LEAVE(ercd)
#endif /* LOG_UNL_MTX_LEAVE */

#ifndef LOG_INI_MTX_ENTER
#define LOG_INI_MTX_ENTER(mtxid)
#endif /* LOG_INI_MTX_ENTER */

#ifndef LOG_INI_MTX_LEAVE
#define LOG_INI_MTX_LEAVE(ercd)
#endif /* LOG_INI_MTX_LEAVE */

#ifndef LOG_REF_MTX_ENTER
#define LOG_REF_MTX_ENTER(mtxid, pk_rmtx)
#endif /* LOG_REF_MTX_ENTER */

#ifndef LOG_REF_MTX_LEAVE
#define LOG_REF_MTX_LEAVE(ercd, pk_rmtx)
#endif /* LOG_REF_MTX_LEAVE */

/*
 *  total number of mutex
 */
#define tnum_mtx	((uint_t)(tmax_mtxid - TMIN_MTXID + 1))

/*
 *  get mutex control block from mutex id
 */
#define INDEX_MTX(mtxid)	((uint_t)((mtxid) - TMIN_MTXID))
#define get_mtxcb(mtxid)	(&(mtxcb_table[INDEX_MTX(mtxid)]))

/*
 *  macro to get a mutex control block pointer from a pointer to the
 *  mutex queue in it
 */
#define MTXCB_QUEUE(p_queue) \
			((MTXCB *)(((char *) p_queue) - offsetof(MTXCB, mutex_queue)))

/*
 *  macro to determine the protocol of a mutex
 */
#define MTXPROTO_MASK			0x03U
#define MTXPROTO(p_mtxcb)		((p_mtxcb)->p_mtxinib->mtxatr & MTXPROTO_MASK)
#define MTX_CEILING(p_mtxcb)	(MTXPROTO(p_mtxcb) == TA_CEILING)

/*
 *  variables for the hook routine call
 */
#ifdef TOPPERS_mtxhook

bool_t	(*mtxhook_check_ceilpri)(TCB *p_tcb, uint_t bpriority) = NULL;
bool_t	(*mtxhook_scan_ceilmtx)(TCB *p_tcb) = NULL;
bool_t	(*mtxhook_release_all)(TCB *p_tcb) = NULL;

#endif /* TOPPERS_mtxhook */

/*
 *  mutex initialization
 */
#ifdef TOPPERS_mtxini

void
initialize_mutex(void)
{
	uint_t	i;
	MTXCB	*p_mtxcb;

	mtxhook_check_ceilpri = mutex_check_ceilpri;
	mtxhook_scan_ceilmtx = mutex_scan_ceilmtx;
	mtxhook_release_all = mutex_release_all;

	for (p_mtxcb = mtxcb_table, i = 0; i < tnum_mtx; p_mtxcb++, i++) {
		queue_initialize(&(p_mtxcb->wait_queue));
		p_mtxcb->p_mtxinib = &(mtxinib_table[i]);
		p_mtxcb->p_loctsk = NULL;
	}
}

#endif /* TOPPERS_mtxini */

/*
 *  check whether the ceiling priority is violated 
 */
#ifdef TOPPERS_mtxchk

bool_t
mutex_check_ceilpri(TCB *p_tcb, uint_t bpriority)
{
	QUEUE	*p_queue;
	MTXCB	*p_mtxcb;

	/*
	 *  when the task acquired a mutex (mutex_queue is not empty), 
	 *  if bpriority is lower than ceiling priority, return false.
	 */
	p_queue = p_tcb->mutex_queue.p_next;
	while (p_queue != &(p_tcb->mutex_queue)) {
		p_mtxcb = MTXCB_QUEUE(p_queue);
		if (MTX_CEILING(p_mtxcb) && bpriority < p_mtxcb->p_mtxinib->ceilpri) {
			return(false);
		}
		p_queue = p_queue->p_next;
	}

	/*
	 *  when the task is waiting for a mutex, 
	 *  if bpriority is lower than ceiling priority, return false¡¥
	 */
	if (TSTAT_WAIT_MTX(p_tcb->tstat)) {
		p_mtxcb = ((WINFO_MTX *)(p_tcb->p_winfo))->p_mtxcb;
		if (MTX_CEILING(p_mtxcb) && bpriority < p_mtxcb->p_mtxinib->ceilpri) {
			return(false);
		}
	}

	/*
	 *  it the above conditions are not matched, return true.
	 */
	return(true);
}

#endif /* TOPPERS_mtxchk */

/*
 *  check whether a task's mutex queue is empty (whether a task acquired a
 *  mutex)
 */
#ifdef TOPPERS_mtxscan

bool_t
mutex_scan_ceilmtx(TCB *p_tcb)
{
	QUEUE	*p_queue;
	MTXCB	*p_mtxcb;

	p_queue = p_tcb->mutex_queue.p_next;
	while (p_queue != &(p_tcb->mutex_queue)) {
		p_mtxcb = MTXCB_QUEUE(p_queue);
		if (MTX_CEILING(p_mtxcb)) {
			return(true);
		}
		p_queue = p_queue->p_next;
	}
	return(false);
}

#endif /* TOPPERS_mtxscan */

/*
 *  calculate the current task priority
 */
#ifdef TOPPERS_mtxcalc

uint_t
mutex_calc_priority(TCB *p_tcb)
{
	uint_t	priority;
	QUEUE	*p_queue;
	MTXCB	*p_mtxcb;

	priority = p_tcb->bpriority;
	p_queue = p_tcb->mutex_queue.p_next;
	while (p_queue != &(p_tcb->mutex_queue)) {
		p_mtxcb = MTXCB_QUEUE(p_queue);
		if (MTX_CEILING(p_mtxcb) && p_mtxcb->p_mtxinib->ceilpri < priority) {
			priority = p_mtxcb->p_mtxinib->ceilpri;
		}
		p_queue = p_queue->p_next;
	}
	return(priority);
}

#endif /* TOPPERS_mtxcalc */

/*
 *  change priority when the task acquired a mutex
 */
Inline bool_t
mutex_raise_priority(TCB *p_tcb, uint_t newpri)
{
	if (newpri < p_tcb->priority) {
		return(change_priority(p_tcb, newpri, true));
	}
	return(false);
}

/*
 *  change priority if the task released a mutex
 */
Inline bool_t
mutex_drop_priority(TCB *p_tcb, uint_t oldpri)
{
	uint_t	newpri;

	if (oldpri == p_tcb->priority) {
		newpri = mutex_calc_priority(p_tcb);
		if (newpri != p_tcb->priority) {
			return(change_priority(p_tcb, newpri, true));
		}
	}
	return(false);
}

/*
 *  acquire the mutex
 */
Inline bool_t
mutex_acquire(TCB *p_loctsk, MTXCB *p_mtxcb)
{
	p_mtxcb->p_loctsk = p_loctsk;
	queue_insert_prev(&(p_loctsk->mutex_queue), &(p_mtxcb->mutex_queue));
	if (MTX_CEILING(p_mtxcb)) {
		return(mutex_raise_priority(p_loctsk, p_mtxcb->p_mtxinib->ceilpri));
	}
	return(false);
}

/*
 *  release the mutex
 */
#ifdef TOPPERS_mtxrel

bool_t
mutex_release(MTXCB *p_mtxcb)
{
	TCB		*p_tcb;

	if (queue_empty(&(p_mtxcb->wait_queue))) {
		p_mtxcb->p_loctsk = NULL;
		return(false);
	}
	else {
		/*
		 *  get the next task in the wait queue of the mutex, and let
		 *  this task lock the mutex
		 */
		p_tcb = (TCB *) queue_delete_next(&(p_mtxcb->wait_queue));
		wait_dequeue_tmevtb(p_tcb);
		p_tcb->p_winfo->wercd = E_OK;

		p_mtxcb->p_loctsk = p_tcb;
		queue_insert_prev(&(p_tcb->mutex_queue), &(p_mtxcb->mutex_queue));
		if (MTX_CEILING(p_mtxcb)) {
			if (p_mtxcb->p_mtxinib->ceilpri < p_tcb->priority) {
				p_tcb->priority = p_mtxcb->p_mtxinib->ceilpri;
			}
		}
		return(make_non_wait(p_tcb));
	}
}

#endif /* TOPPERS_mtxrel */

/*
 *  release all acquired mutex of the task
 */
#ifdef TOPPERS_mtxrela

bool_t
mutex_release_all(TCB *p_tcb)
{
	MTXCB	*p_mtxcb;
	bool_t	dspreq = false;

	while (!queue_empty(&(p_tcb->mutex_queue))) {
		p_mtxcb = MTXCB_QUEUE(p_tcb->mutex_queue.p_next);
		queue_delete(&(p_mtxcb->mutex_queue));
		if (mutex_release(p_mtxcb)) {
			dspreq = true;
		}
	}
	return(dspreq);
}

#endif /* TOPPERS_mtxrela */

/*
 *  lock the mutex
 */
#ifdef TOPPERS_loc_mtx

ER
loc_mtx(ID mtxid)
{
	MTXCB	*p_mtxcb;
	WINFO_MTX winfo_mtx;
	ER		ercd;

	LOG_LOC_MTX_ENTER(mtxid);
	CHECK_DISPATCH();
	CHECK_MTXID(mtxid);
	p_mtxcb = get_mtxcb(mtxid);

	t_lock_cpu();
	if (MTX_CEILING(p_mtxcb)
				&& p_runtsk->bpriority < p_mtxcb->p_mtxinib->ceilpri) {
		ercd = E_ILUSE;
	}
	else if (p_mtxcb->p_loctsk == NULL) {
		(void) mutex_acquire(p_runtsk, p_mtxcb);
		/*
	 	 *  when a task acquired the mutex, its priority may be rasied,
		 *  but there is no need to dispatch here, because the task is
		 *  already in running.
		 */
		assert(!(p_runtsk != p_schedtsk && dspflg));
		ercd = E_OK;
	}
	else if (p_mtxcb->p_loctsk == p_runtsk) {
		ercd = E_ILUSE;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_MTX);
		wobj_make_wait((WOBJCB *) p_mtxcb, (WINFO_WOBJ *) &winfo_mtx);
		dispatch();
		ercd = winfo_mtx.winfo.wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_LOC_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_loc_mtx */

/*
 *  mutex lock (polling)
 */
#ifdef TOPPERS_ploc_mtx

ER
ploc_mtx(ID mtxid)
{
	MTXCB	*p_mtxcb;
	ER		ercd;

	LOG_PLOC_MTX_ENTER(mtxid);
	CHECK_TSKCTX_UNL();
	CHECK_MTXID(mtxid);
	p_mtxcb = get_mtxcb(mtxid);

	t_lock_cpu();
	if (MTX_CEILING(p_mtxcb)
				&& p_runtsk->bpriority < p_mtxcb->p_mtxinib->ceilpri) {
		ercd = E_ILUSE;
	}
	else if (p_mtxcb->p_loctsk == NULL) {
		(void) mutex_acquire(p_runtsk, p_mtxcb);
		/*
	 	 *  when a task acquired the mutex, its priority may be rasied,
		 *  but there is no need to dispatch here, because the task is
		 *  already in running.
		 */
		assert(!(p_runtsk != p_schedtsk && dspflg));
		ercd = E_OK;
	}
	else if (p_mtxcb->p_loctsk == p_runtsk) {
		ercd = E_ILUSE;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

  error_exit:
	LOG_PLOC_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ploc_mtx */

/*
 *  lock the mutex (with timeout) 
 */
#ifdef TOPPERS_tloc_mtx

ER
tloc_mtx(ID mtxid, TMO tmout)
{
	MTXCB	*p_mtxcb;
	WINFO_MTX winfo_mtx;
	TMEVTB	tmevtb;
	ER		ercd;

	LOG_TLOC_MTX_ENTER(mtxid, tmout);
	CHECK_DISPATCH();
	CHECK_MTXID(mtxid);
	CHECK_TMOUT(tmout);
	p_mtxcb = get_mtxcb(mtxid);

	t_lock_cpu();
	if (MTX_CEILING(p_mtxcb)
				&& p_runtsk->bpriority < p_mtxcb->p_mtxinib->ceilpri) {
		ercd = E_ILUSE;
	}
	else if (p_mtxcb->p_loctsk == NULL) {
		(void) mutex_acquire(p_runtsk, p_mtxcb);
		/*
	 	 *  when a task acquired the mutex, its priority may be rasied,
		 *  but there is no need to dispatch here, because the task is
		 *  already in running.
		 */
		assert(!(p_runtsk != p_schedtsk && dspflg));
		ercd = E_OK;
	}
	else if (p_mtxcb->p_loctsk == p_runtsk) {
		ercd = E_ILUSE;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_MTX);
		wobj_make_wait_tmout((WOBJCB *) p_mtxcb, (WINFO_WOBJ *) &winfo_mtx,
														&tmevtb, tmout);
		dispatch();
		ercd = winfo_mtx.winfo.wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_TLOC_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_tloc_mtx */

/*
 *  unlock the mutex
 */
#ifdef TOPPERS_unl_mtx

ER
unl_mtx(ID mtxid)
{
	MTXCB	*p_mtxcb;
	bool_t	dspreq = false;
	ER		ercd;

	LOG_UNL_MTX_ENTER(mtxid);
	CHECK_TSKCTX_UNL();
	CHECK_MTXID(mtxid);
	p_mtxcb = get_mtxcb(mtxid);

	t_lock_cpu();
	if (p_mtxcb->p_loctsk != p_runtsk) {
		ercd = E_ILUSE;
	}
	else {
		queue_delete(&(p_mtxcb->mutex_queue));
		if (MTX_CEILING(p_mtxcb)) {
			if (mutex_drop_priority(p_runtsk, p_mtxcb->p_mtxinib->ceilpri)) {
				dspreq = true;
			}
		}
		if (mutex_release(p_mtxcb)) {
			dspreq = true;
		}
		if (dspreq) {
			dispatch();
		}
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_UNL_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_unl_mtx */

/*
 *  initialize the mutex
 */
#ifdef TOPPERS_ini_mtx

ER
ini_mtx(ID mtxid)
{
	MTXCB	*p_mtxcb;
	TCB		*p_loctsk;
	bool_t	dspreq;
	ER		ercd;

	LOG_INI_MTX_ENTER(mtxid);
	CHECK_TSKCTX_UNL();
	CHECK_MTXID(mtxid);
	p_mtxcb = get_mtxcb(mtxid);

	t_lock_cpu();
	dspreq = init_wait_queue(&(p_mtxcb->wait_queue));
	p_loctsk = p_mtxcb->p_loctsk;
	if (p_loctsk != NULL) {
		queue_delete(&(p_mtxcb->mutex_queue));
		p_mtxcb->p_loctsk = NULL;
		if (MTX_CEILING(p_mtxcb)) {
			if (mutex_drop_priority(p_loctsk, p_mtxcb->p_mtxinib->ceilpri)) {
				dspreq = true;
			}
		}
	}
	if (dspreq) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_INI_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_mtx */

/*
 *  refer the mutex
 */
#ifdef TOPPERS_ref_mtx

ER
ref_mtx(ID mtxid, T_RMTX *pk_rmtx)
{
	MTXCB	*p_mtxcb;
	ER		ercd;

	LOG_REF_MTX_ENTER(mtxid, pk_rmtx);
	CHECK_TSKCTX_UNL();
	CHECK_MTXID(mtxid);
	p_mtxcb = get_mtxcb(mtxid);

	t_lock_cpu();
	pk_rmtx->htskid = (p_mtxcb->p_loctsk != NULL) ? TSKID(p_mtxcb->p_loctsk)
													: TSK_NONE;
	pk_rmtx->wtskid = wait_tskid(&(p_mtxcb->wait_queue));
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_REF_MTX_LEAVE(ercd, pk_rmtx);
	return(ercd);
}

#endif /* TOPPERS_ref_mtx */
