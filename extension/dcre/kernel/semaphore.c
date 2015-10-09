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
 *  @(#) $Id: semaphore.c 1966 2010-11-20 07:23:56Z ertl-hiro $
 */

/*
 *	semaphore function
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "semaphore.h"

/*
 *  default definition of trace macro
 */
#ifndef LOG_ACRE_SEM_ENTER
#define LOG_ACRE_SEM_ENTER(pk_csem)
#endif /* LOG_ACRE_SEM_ENTER */

#ifndef LOG_ACRE_SEM_LEAVE
#define LOG_ACRE_SEM_LEAVE(ercd)
#endif /* LOG_ACRE_SEM_LEAVE */

#ifndef LOG_DEL_SEM_ENTER
#define LOG_DEL_SEM_ENTER(semid)
#endif /* LOG_DEL_SEM_ENTER */

#ifndef LOG_DEL_SEM_LEAVE
#define LOG_DEL_SEM_LEAVE(ercd)
#endif /* LOG_DEL_SEM_LEAVE */

#ifndef LOG_SIG_SEM_ENTER
#define LOG_SIG_SEM_ENTER(semid)
#endif /* LOG_SIG_SEM_ENTER */

#ifndef LOG_SIG_SEM_LEAVE
#define LOG_SIG_SEM_LEAVE(ercd)
#endif /* LOG_SIG_SEM_LEAVE */

#ifndef LOG_ISIG_SEM_ENTER
#define LOG_ISIG_SEM_ENTER(semid)
#endif /* LOG_ISIG_SEM_ENTER */

#ifndef LOG_ISIG_SEM_LEAVE
#define LOG_ISIG_SEM_LEAVE(ercd)
#endif /* LOG_ISIG_SEM_LEAVE */

#ifndef LOG_WAI_SEM_ENTER
#define LOG_WAI_SEM_ENTER(semid)
#endif /* LOG_WAI_SEM_ENTER */

#ifndef LOG_WAI_SEM_LEAVE
#define LOG_WAI_SEM_LEAVE(ercd)
#endif /* LOG_WAI_SEM_LEAVE */

#ifndef LOG_POL_SEM_ENTER
#define LOG_POL_SEM_ENTER(semid)
#endif /* LOG_POL_SEM_ENTER */

#ifndef LOG_POL_SEM_LEAVE
#define LOG_POL_SEM_LEAVE(ercd)
#endif /* LOG_POL_SEM_LEAVE */

#ifndef LOG_TWAI_SEM_ENTER
#define LOG_TWAI_SEM_ENTER(semid, tmout)
#endif /* LOG_TWAI_SEM_ENTER */

#ifndef LOG_TWAI_SEM_LEAVE
#define LOG_TWAI_SEM_LEAVE(ercd)
#endif /* LOG_TWAI_SEM_LEAVE */

#ifndef LOG_INI_SEM_ENTER
#define LOG_INI_SEM_ENTER(semid)
#endif /* LOG_INI_SEM_ENTER */

#ifndef LOG_INI_SEM_LEAVE
#define LOG_INI_SEM_LEAVE(ercd)
#endif /* LOG_INI_SEM_LEAVE */

#ifndef LOG_REF_SEM_ENTER
#define LOG_REF_SEM_ENTER(semid, pk_rsem)
#endif /* LOG_REF_SEM_ENTER */

#ifndef LOG_REF_SEM_LEAVE
#define LOG_REF_SEM_LEAVE(ercd, pk_rsem)
#endif /* LOG_REF_SEM_LEAVE */

/*
 *  total number of semaphores
 */
#define tnum_sem	((uint_t)(tmax_semid - TMIN_SEMID + 1))
#define tnum_ssem	((uint_t)(tmax_ssemid - TMIN_SEMID + 1))

/*
 *   get semaphore control block from semaphore id
 */
#define INDEX_SEM(semid)	((uint_t)((semid) - TMIN_SEMID))
#define get_semcb(semid)	(&(semcb_table[INDEX_SEM(semid)]))

#ifdef TOPPERS_semini

/*
 *  list of free semaphore control blocks 
 */
QUEUE	free_semcb;

/*
 *  semaphore initialization
 */
void
initialize_semaphore(void)
{
	uint_t	i, j;
	SEMCB	*p_semcb;
	SEMINIB	*p_seminib;

	for (p_semcb = semcb_table, i = 0; i < tnum_ssem; p_semcb++, i++) {
		queue_initialize(&(p_semcb->wait_queue));
		p_semcb->p_seminib = &(seminib_table[i]);
		p_semcb->semcnt = p_semcb->p_seminib->isemcnt;
	}
	queue_initialize(&free_semcb);
	for (j = 0; i < tnum_sem; p_semcb++, i++, j++) {
		p_seminib = &(aseminib_table[j]);
		p_seminib->sematr = TA_NOEXS;
		p_semcb->p_seminib = ((const SEMINIB *) p_seminib);
		queue_insert_prev(&free_semcb, &(p_semcb->wait_queue));
	}
}

#endif /* TOPPERS_semini */

/*
 *  create semaphore
 */
#ifdef TOPPERS_acre_sem

ER_UINT
acre_sem(const T_CSEM *pk_csem)
{
	SEMCB	*p_semcb;
	SEMINIB	*p_seminib;
	ER		ercd;

	LOG_ACRE_SEM_ENTER(pk_csem);
	CHECK_TSKCTX_UNL();
	CHECK_RSATR(pk_csem->sematr, TA_TPRI);
	CHECK_PAR(0 <= pk_csem->isemcnt && pk_csem->isemcnt <= pk_csem->maxsem);
	CHECK_PAR(1 <= pk_csem->maxsem && pk_csem->maxsem <= TMAX_MAXSEM);

	t_lock_cpu();
	if (queue_empty(&free_semcb)) {
		ercd = E_NOID;
	}
	else {
		p_semcb = ((SEMCB *) queue_delete_next(&free_semcb));
		p_seminib = (SEMINIB *)(p_semcb->p_seminib);
		p_seminib->sematr = pk_csem->sematr;
		p_seminib->isemcnt = pk_csem->isemcnt;
		p_seminib->maxsem = pk_csem->maxsem;

		queue_initialize(&(p_semcb->wait_queue));
		p_semcb->semcnt = p_semcb->p_seminib->isemcnt;
		ercd = SEMID(p_semcb);
	}
	t_unlock_cpu();

  error_exit:
	LOG_ACRE_SEM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_acre_sem */

/*
 *  delete semaphore
 */
#ifdef TOPPERS_del_sem

ER
del_sem(ID semid)
{
	SEMCB	*p_semcb;
	SEMINIB	*p_seminib;
	bool_t	dspreq;
	ER		ercd;

	LOG_DEL_SEM_ENTER(semid);
	CHECK_TSKCTX_UNL();
	CHECK_SEMID(semid);
	p_semcb = get_semcb(semid);

	t_lock_cpu();
	if (p_semcb->p_seminib->sematr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (SEMID(p_semcb) > tmax_ssemid) {
		dspreq = init_wait_queue(&(p_semcb->wait_queue));
		p_seminib = (SEMINIB *)(p_semcb->p_seminib);
		p_seminib->sematr = TA_NOEXS;
		queue_insert_prev(&free_semcb, &(p_semcb->wait_queue));
		if (dspreq) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		ercd = E_OBJ;
	}
	t_unlock_cpu();

  error_exit:
	LOG_DEL_SEM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_del_sem */

/*
 *  release semaphore (signal semaphore)
 */
#ifdef TOPPERS_sig_sem

ER
sig_sem(ID semid)
{
	SEMCB	*p_semcb;
	TCB		*p_tcb;
	ER		ercd;

	LOG_SIG_SEM_ENTER(semid);
	CHECK_TSKCTX_UNL();
	CHECK_SEMID(semid);
	p_semcb = get_semcb(semid);

	t_lock_cpu();
	if (p_semcb->p_seminib->sematr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (!queue_empty(&(p_semcb->wait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_semcb->wait_queue));
		if (wait_complete(p_tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (p_semcb->semcnt < p_semcb->p_seminib->maxsem) {
		p_semcb->semcnt += 1;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	t_unlock_cpu();

  error_exit:
	LOG_SIG_SEM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sig_sem */

/*
 *  release semaphore (signal semaphore)(used in non-task context)
 */
#ifdef TOPPERS_isig_sem

ER
isig_sem(ID semid)
{
	SEMCB	*p_semcb;
	TCB		*p_tcb;
	ER		ercd;

	LOG_ISIG_SEM_ENTER(semid);
	CHECK_INTCTX_UNL();
	CHECK_SEMID(semid);
	p_semcb = get_semcb(semid);

	i_lock_cpu();
	if (p_semcb->p_seminib->sematr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (!queue_empty(&(p_semcb->wait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_semcb->wait_queue));
		if (wait_complete(p_tcb)) {
			reqflg = true;
		}
		ercd = E_OK;
	}
	else if (p_semcb->semcnt < p_semcb->p_seminib->maxsem) {
		p_semcb->semcnt += 1;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	i_unlock_cpu();

  error_exit:
	LOG_ISIG_SEM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_isig_sem */

/*
 *  accquire semaphore (wait for semaphore)
 */
#ifdef TOPPERS_wai_sem

ER
wai_sem(ID semid)
{
	SEMCB	*p_semcb;
	WINFO_SEM winfo_sem;
	ER		ercd;

	LOG_WAI_SEM_ENTER(semid);
	CHECK_DISPATCH();
	CHECK_SEMID(semid);
	p_semcb = get_semcb(semid);

	t_lock_cpu();
	if (p_semcb->p_seminib->sematr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_semcb->semcnt >= 1) {
		p_semcb->semcnt -= 1;
		ercd = E_OK;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_SEM);
		wobj_make_wait((WOBJCB *) p_semcb, (WINFO_WOBJ *) &winfo_sem);
		dispatch();
		ercd = winfo_sem.winfo.wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_WAI_SEM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_wai_sem */

/*
 *  accquire semaphore by polling
 */
#ifdef TOPPERS_pol_sem

ER
pol_sem(ID semid)
{
	SEMCB	*p_semcb;
	ER		ercd;

	LOG_POL_SEM_ENTER(semid);
	CHECK_TSKCTX_UNL();
	CHECK_SEMID(semid);
	p_semcb = get_semcb(semid);

	t_lock_cpu();
	if (p_semcb->p_seminib->sematr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_semcb->semcnt >= 1) {
		p_semcb->semcnt -= 1;
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

  error_exit:
	LOG_POL_SEM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_pol_sem */

/*
 *   accquire semaphore (wait for semaphore with timeout)
 */
#ifdef TOPPERS_twai_sem

ER
twai_sem(ID semid, TMO tmout)
{
	SEMCB	*p_semcb;
	WINFO_SEM winfo_sem;
	TMEVTB	tmevtb;
	ER		ercd;

	LOG_TWAI_SEM_ENTER(semid, tmout);
	CHECK_DISPATCH();
	CHECK_SEMID(semid);
	CHECK_TMOUT(tmout);
	p_semcb = get_semcb(semid);

	t_lock_cpu();
	if (p_semcb->p_seminib->sematr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_semcb->semcnt >= 1) {
		p_semcb->semcnt -= 1;
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_SEM);
		wobj_make_wait_tmout((WOBJCB *) p_semcb, (WINFO_WOBJ *) &winfo_sem,
														&tmevtb, tmout);
		dispatch();
		ercd = winfo_sem.winfo.wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_TWAI_SEM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_twai_sem */

/*
 *  re-init semaphore
 */
#ifdef TOPPERS_ini_sem

ER
ini_sem(ID semid)
{
	SEMCB	*p_semcb;
	bool_t	dspreq;
	ER		ercd;

	LOG_INI_SEM_ENTER(semid);
	CHECK_TSKCTX_UNL();
	CHECK_SEMID(semid);
	p_semcb = get_semcb(semid);

	t_lock_cpu();
	if (p_semcb->p_seminib->sematr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else {
		dspreq = init_wait_queue(&(p_semcb->wait_queue));
		p_semcb->semcnt = p_semcb->p_seminib->isemcnt;
		if (dspreq) {
			dispatch();
		}
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_INI_SEM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_sem */

/*
 *  refer the status of semaphore
 */
#ifdef TOPPERS_ref_sem

ER
ref_sem(ID semid, T_RSEM *pk_rsem)
{
	SEMCB	*p_semcb;
	ER		ercd;

	LOG_REF_SEM_ENTER(semid, pk_rsem);
	CHECK_TSKCTX_UNL();
	CHECK_SEMID(semid);
	p_semcb = get_semcb(semid);

	t_lock_cpu();
	if (p_semcb->p_seminib->sematr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else {
		pk_rsem->wtskid = wait_tskid(&(p_semcb->wait_queue));
		pk_rsem->semcnt = p_semcb->semcnt;
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_REF_SEM_LEAVE(ercd, pk_rsem);
	return(ercd);
}

#endif /* TOPPERS_ref_sem */
