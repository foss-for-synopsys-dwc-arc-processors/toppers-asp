/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2005-2008 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: bit_kernel.c 1544 2009-05-08 07:53:48Z ertl-hiro $
 */

/*
 *		the kernel integrity check
 */

#include "kernel/kernel_impl.h"
#include "kernel/task.h"
#include "kernel/wait.h"
#include "kernel/semaphore.h"
#include "kernel/eventflag.h"
#include "kernel/dataqueue.h"
#include "kernel/pridataq.h"
#include "kernel/mailbox.h"
#include "kernel/mutex.h"
#include "kernel/mempfix.h"
#include "kernel/time_event.h"

/*
 *  error code definition
 */
#define E_SYS_LINENO	ERCD(E_SYS, -(__LINE__))

/*
 *  check the address validity of control block
 */
#define VALID_TCB(p_tcb) \
		((((char *) p_tcb) - ((char *) tcb_table)) % sizeof(TCB) == 0 \
			&& TMIN_TSKID <= TSKID(p_tcb) && TSKID(p_tcb) <= tmax_tskid)

#define VALID_SEMCB(p_semcb) \
		((((char *) p_semcb) - ((char *) semcb_table)) % sizeof(SEMCB) == 0 \
			&& TMIN_SEMID <= SEMID(p_semcb) && SEMID(p_semcb) <= tmax_semid)

#define VALID_FLGCB(p_flgcb) \
		((((char *) p_flgcb) - ((char *) flgcb_table)) % sizeof(FLGCB) == 0 \
			&& TMIN_FLGID <= FLGID(p_flgcb) && FLGID(p_flgcb) <= tmax_flgid)

#define VALID_DTQCB(p_dtqcb) \
		((((char *) p_dtqcb) - ((char *) dtqcb_table)) % sizeof(DTQCB) == 0 \
			&& TMIN_DTQID <= DTQID(p_dtqcb) && DTQID(p_dtqcb) <= tmax_dtqid)

#define VALID_PDQCB(p_pdqcb) \
		((((char *) p_pdqcb) - ((char *) pdqcb_table)) % sizeof(PDQCB) == 0 \
			&& TMIN_PDQID <= PDQID(p_pdqcb) && PDQID(p_pdqcb) <= tmax_pdqid)

#define VALID_MBXCB(p_mbxcb) \
		((((char *) p_mbxcb) - ((char *) mbxcb_table)) % sizeof(MBXCB) == 0 \
			&& TMIN_MBXID <= MBXID(p_mbxcb) && MBXID(p_mbxcb) <= tmax_mbxid)

#define VALID_MTXCB(p_mtxcb) \
		((((char *) p_mtxcb) - ((char *) mtxcb_table)) % sizeof(MTXCB) == 0 \
			&& TMIN_MTXID <= MTXID(p_mtxcb) && MTXID(p_mtxcb) <= tmax_mtxid)

#define VALID_MPFCB(p_mpfcb) \
		((((char *) p_mpfcb) - ((char *) mpfcb_table)) % sizeof(MPFCB) == 0 \
			&& TMIN_MPFID <= MPFID(p_mpfcb) && MPFID(p_mpfcb) <= tmax_mpfid)

/*
 *  queue error check functions
 *
 *  check whether p_entry is in p_queue, if it is, return true, if not
 *  return false. if double link is not maintained, return false.
 */
static bool_t
in_queue(QUEUE *p_queue, QUEUE *p_entry)
{
	QUEUE	*p_current, *p_next;

	p_current = p_queue->p_next;
	if (p_current->p_prev != p_queue) {
		return(false);					/* double link is not maintained */
	}
	while (p_current != p_queue) {
		if (p_current == p_entry) {
			return(true);				/* p_entry is in p_queue */
		}

		/*
		 *  move next item in the queue
		 */
		p_next = p_current->p_next;
		if (p_next->p_prev != p_current) {
			return(false);				 /* double link is not maintained */
		}
		p_current = p_next;
	}
	return(false);
}

/*
 *  check whether the addr is in the stack pointed by p_tinib
 */
static bool_t
on_stack(void *addr, const TINIB *p_tinib)
{
	if (p_tinib->stk <= addr
				&& addr < (void *)((char *)(p_tinib->stk) + p_tinib->stksz)) {
		return(true);
	}
	return(false);
}

/*
 * check task integrity
 */
static ER
bit_task(ID tskid)
{
	TCB			*p_tcb;
	const TINIB	*p_tinib;
	uint_t		tstat, tstat_wait, priority;
	TMEVTB		*p_tmevtb;
	SEMCB		*p_semcb;
	FLGCB		*p_flgcb;
	DTQCB		*p_dtqcb;
	PDQCB		*p_pdqcb;
	MBXCB		*p_mbxcb;
	MTXCB		*p_mtxcb;
	MPFCB		*p_mpfcb;

	if (!(TMIN_TSKID <= (tskid) && (tskid) <= tmax_tskid)) {
		return(E_ID);
	}
	p_tcb = get_tcb(tskid);
	p_tinib = p_tcb->p_tinib;
	tstat = p_tcb->tstat;
	tstat_wait = (tstat & TS_WAIT_MASK);
	priority = p_tcb->priority;

	/*
	 *  check the pointer to initialization block
	 */
	if (p_tinib != &(tinib_table[INDEX_TSK(tskid)])) {
		return(E_SYS_LINENO);
	}

	/*
	 *  check tstat
	 */
	switch (tstat & (TS_RUNNABLE | TS_WAITING | TS_SUSPENDED)) {
	case TS_DORMANT:
		if (tstat != TS_DORMANT) {
			return(E_SYS_LINENO);
		}
		break;
	case TS_RUNNABLE:
		if (tstat != TS_RUNNABLE) {
			return(E_SYS_LINENO);
		}
		break;
	case TS_WAITING:
	case (TS_WAITING | TS_SUSPENDED):
		if (!(TS_WAIT_DLY <= tstat_wait && tstat_wait <= TS_WAIT_MTX)) {
			return(E_SYS_LINENO);
		}
		if ((tstat & ~(TS_WAIT_MASK | TS_RUNNABLE | TS_WAITING | TS_SUSPENDED))
																	!= 0U) {
			return(E_SYS_LINENO);
		}
		break;
	case TS_SUSPENDED:
		if (tstat != TS_SUSPENDED) {
			return(E_SYS_LINENO);
		}
		break;
	default:
		return(E_SYS_LINENO);
	}

	/*
	 *  check actque
	 */
	if (TSTAT_DORMANT(tstat) && p_tcb->actque) {
		return(E_SYS_LINENO);
	}

	/*
	 *  check priority
	 */
	if (priority >= TNUM_TPRI) {
		return(E_SYS_LINENO);
	}

	/*
	 *  check texrtn
	 */
	if (p_tcb->p_tinib->texrtn == NULL && p_tcb->texptn != 0U) {
		return(E_SYS_LINENO);
	}

	/*
	 *  check if dormant
	 */
	if (TSTAT_DORMANT(tstat)) {
		if (!(priority == p_tinib->ipriority)
					&& (p_tcb->wupque == false)
					&& (p_tcb->enatex == false)
					&& (p_tcb->texptn == 0U)) {
			return(E_SYS_LINENO);
		}
	}

	/*
	 *  check whether the task is in runnable state
	 */
	if (TSTAT_RUNNABLE(tstat)) {
		if (!in_queue(&ready_queue[priority], &(p_tcb->task_queue))) {
			return(E_SYS_LINENO);
		}
	}

	/*
	 *  check whether the task is in waiting state
	 */
	if (TSTAT_WAITING(tstat)) {
		if (!on_stack(p_tcb->p_winfo, p_tinib)) {
			return(E_SYS_LINENO);
		}
		p_tmevtb = p_tcb->p_winfo->p_tmevtb;
		if (p_tmevtb != NULL) {
			if (!on_stack(p_tmevtb, p_tinib)) {
				return(E_SYS_LINENO);
			}
			/*
			 *  check (*p_tmevtb) (to be done)
			 */
		}

		switch (tstat & TS_WAIT_MASK) {
			case TS_WAIT_SLP:
				if (p_tcb->wupque == true) {
					return(E_SYS_LINENO);
				}
				break;

			case TS_WAIT_DLY:
				if (p_tmevtb == NULL) {
					return(E_SYS_LINENO);
				}
				break;

			case TS_WAIT_SEM:
				p_semcb = ((WINFO_SEM *)(p_tcb->p_winfo))->p_semcb;
				if (!VALID_SEMCB(p_semcb)) {
					return(E_SYS_LINENO);
				}
				if (!in_queue(&(p_semcb->wait_queue), &(p_tcb->task_queue))) {
					return(E_SYS_LINENO);
				}
				break;

			case TS_WAIT_FLG:
				p_flgcb = ((WINFO_FLG *)(p_tcb->p_winfo))->p_flgcb;
				if (!VALID_FLGCB(p_flgcb)) {
					return(E_SYS_LINENO);
				}
				if (!in_queue(&(p_flgcb->wait_queue), &(p_tcb->task_queue))) {
					return(E_SYS_LINENO);
				}
				break;

			case TS_WAIT_SDTQ:
				p_dtqcb = ((WINFO_DTQ *)(p_tcb->p_winfo))->p_dtqcb;
				if (!VALID_DTQCB(p_dtqcb)) {
					return(E_SYS_LINENO);
				}
				if (!in_queue(&(p_dtqcb->swait_queue), &(p_tcb->task_queue))) {
					return(E_SYS_LINENO);
				}
				break;

			case TS_WAIT_RDTQ:
				p_dtqcb = ((WINFO_DTQ *)(p_tcb->p_winfo))->p_dtqcb;
				if (!VALID_DTQCB(p_dtqcb)) {
					return(E_SYS_LINENO);
				}
				if (!in_queue(&(p_dtqcb->rwait_queue), &(p_tcb->task_queue))) {
					return(E_SYS_LINENO);
				}
				break;

			case TS_WAIT_SPDQ:
				p_pdqcb = ((WINFO_PDQ *)(p_tcb->p_winfo))->p_pdqcb;
				if (!VALID_PDQCB(p_pdqcb)) {
					return(E_SYS_LINENO);
				}
				if (!in_queue(&(p_pdqcb->swait_queue), &(p_tcb->task_queue))) {
					return(E_SYS_LINENO);
				}
				break;

			case TS_WAIT_RPDQ:
				p_pdqcb = ((WINFO_PDQ *)(p_tcb->p_winfo))->p_pdqcb;
				if (!VALID_PDQCB(p_pdqcb)) {
					return(E_SYS_LINENO);
				}
				if (!in_queue(&(p_pdqcb->rwait_queue), &(p_tcb->task_queue))) {
					return(E_SYS_LINENO);
				}
				break;

			case TS_WAIT_MBX:
				p_mbxcb = ((WINFO_MBX *)(p_tcb->p_winfo))->p_mbxcb;
				if (!VALID_MBXCB(p_mbxcb)) {
					return(E_SYS_LINENO);
				}
				if (!in_queue(&(p_mbxcb->wait_queue), &(p_tcb->task_queue))) {
					return(E_SYS_LINENO);
				}
				break;

			case TS_WAIT_MTX:
				p_mtxcb = ((WINFO_MTX *)(p_tcb->p_winfo))->p_mtxcb;
				if (!VALID_MTXCB(p_mtxcb)) {
					return(E_SYS_LINENO);
				}
				if (!in_queue(&(p_mtxcb->wait_queue), &(p_tcb->task_queue))) {
					return(E_SYS_LINENO);
				}
				break;

			case TS_WAIT_MPF:
				p_mpfcb = ((WINFO_MPF *)(p_tcb->p_winfo))->p_mpfcb;
				if (!VALID_MPFCB(p_mpfcb)) {
					return(E_SYS_LINENO);
				}
				if (!in_queue(&(p_mpfcb->wait_queue), &(p_tcb->task_queue))) {
					return(E_SYS_LINENO);
				}
				break;
		}
	}

	/*
	 *  check tskctxb
	 */
	if (!TSTAT_DORMANT(tstat) && p_tcb != p_runtsk) {
		/*
		 *  check the target dependent part
		 */
#if 0
		if (bit_tskctxb(&(p_tcb->tskctxb))) {
			return(E_SYS_LINENO);
		}
#endif
	}
	return(E_OK);
}

/*
 *  check the semaphore integrity
 */
#define INDEX_SEM(semid)	((uint_t)((semid) - TMIN_SEMID))
#define get_semcb(semid)	(&(semcb_table[INDEX_SEM(semid)]))

static ER
bit_semaphore(ID semid)
{
	SEMCB			*p_semcb;
	const SEMINIB	*p_seminib;
	uint_t			semcnt;
	QUEUE			*p_queue;
	TCB				*p_tcb;

	if (!(TMIN_SEMID <= (semid) && (semid) <= tmax_semid)) {
		return(E_ID);
	}
	p_semcb = get_semcb(semid);
	p_seminib = p_semcb->p_seminib;
	semcnt = p_semcb->semcnt;

	/*
	 *   check the pointer to initialization block
	 */
	if (p_seminib != &(seminib_table[INDEX_SEM(semid)])) {
		return(E_SYS_LINENO);
	}

	/*
	 *  check semcnt
	 */
	if (semcnt > p_seminib->maxsem) {
		return(E_SYS_LINENO);
	}

	/*
	 *  check wait_queue
	 */
	if (semcnt == 0) {
		p_queue = p_semcb->wait_queue.p_next;
		while (p_queue != &(p_semcb->wait_queue)) {
			p_tcb = (TCB *) p_queue;
			p_queue = p_queue->p_next;
			if (!VALID_TCB(p_tcb)) {
				return(E_SYS_LINENO);
			}
			if (p_tcb->tstat != (TS_WAITING | TS_WAIT_SEM)) {
				return(E_SYS_LINENO);
			}
			if (p_semcb != ((WINFO_SEM *)(p_tcb->p_winfo))->p_semcb) {
				return(E_SYS_LINENO);
			}
		}
	}
	else {
		if (!queue_empty(&(p_semcb->wait_queue))) {
			return(E_SYS_LINENO);
		}
	}
	return(E_OK);
}

/*
 *  the main routine of kernel integrity check
 */
ER
bit_kernel(void)
{
	ID		tskid;
	ID		semid;
	ER		ercd;

	/*
	 *  check every task
	 */
	for (tskid = TMIN_TSKID; tskid <= tmax_tskid; tskid++) {
		ercd = bit_task(tskid);
		if (ercd != E_OK) {
			return(ercd);
		}
	}

	/*
	 *  check every semaphore
	 */
	for (semid = TMIN_SEMID; semid <= tmax_semid; semid++) {
		ercd = bit_semaphore(semid);
		if (ercd != E_OK) {
			return(ercd);
		}
	}

	return(E_OK);
}
