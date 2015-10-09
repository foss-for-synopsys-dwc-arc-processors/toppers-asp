/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      High Reliable system Profile Kernel
 *
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
 *  @(#) $Id: bit_mutex.c 762 2008-03-07 23:51:41Z hiro $
 */

/*
 *		mutex function intergrity check
 */

#include "kernel/kernel_impl.h"
#include "kernel/task.h"
#include "kernel/mutex.h"

/*
 *  macro to get the control block from the mutex ID (in mutex.c)
 */
#define INDEX_MTX(mtxid)	((uint_t)((mtxid) - TMIN_MTXID))
#define get_mtxcb(mtxid)	(&(mtxcb_table[INDEX_MTX(mtxid)]))

/*
 *  macro to get the pointer to the mutex control block from the pointer
 *  to mutex_queue in it.(in mutex.c)
 */
#define MTXCB_QUEUE(p_queue) \
			((MTXCB *)(((char *) p_queue) - offsetof(MTXCB, mutex_queue)))

/*
 *  macro to determine the protocol of a mutex (in mutex.c)
 */
#define MTXPROTO_MASK			0x03U
#define MTXPROTO(p_mtxcb)		((p_mtxcb)->p_mtxinib->mtxatr & MTXPROTO_MASK)
#define MTX_CEILING(p_mtxcb)	(MTXPROTO(p_mtxcb) == TA_CEILING)

/*
 *  error code definition
 */
#define E_SYS_LINENO	ERCD(E_SYS, -(__LINE__))

/*
 *  check the validity of the control block address
 */
#define VALID_TCB(p_tcb) \
		((((char *) p_tcb) - ((char *) tcb_table)) % sizeof(TCB) == 0 \
			&& TMIN_TSKID <= TSKID(p_tcb) && TSKID(p_tcb) <= tmax_tskid)

#define VALID_MTXCB(p_mtxcb) \
		((((char *) p_mtxcb) - ((char *) mtxcb_table)) % sizeof(MTXCB) == 0 \
			&& TMIN_MTXID <= MTXID(p_mtxcb) && MTXID(p_mtxcb) <= tmax_mtxid)

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
			return(true);				/* p_entry is in p_queue*/
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
 *  check for task
 */
static ER
bit_mutex_task(ID tskid)
{
	TCB			*p_tcb;
	MTXCB		*p_mtxcb;
	QUEUE		*p_queue, *p_next;
	uint_t		priority;

	if (!(TMIN_TSKID <= (tskid) && (tskid) <= tmax_tskid)) {
		return(E_ID);
	}
	p_tcb = get_tcb(tskid);
	priority = p_tcb->bpriority;

	/*
	 *  check whether the task lock some mutex
	 */
	p_queue = p_tcb->mutex_queue.p_next;
	if (p_queue->p_prev != &(p_tcb->mutex_queue)) {
		return(E_SYS_LINENO);
	}
	while (p_queue != &(p_tcb->mutex_queue)) {
		p_mtxcb = MTXCB_QUEUE(p_queue);
		if (!VALID_MTXCB(p_mtxcb)) {
			return(E_SYS_LINENO);
		}

		/*
		 *  check whether the task locked the mutex
		 */
		if (p_mtxcb->p_loctsk != p_tcb) {
			return(E_SYS_LINENO);
		}

		/*
		 * calculate current priority
		 */
		if (MTXPROTO(p_mtxcb)) {
			if (p_mtxcb->p_mtxinib->ceilpri < priority) {
				priority = p_mtxcb->p_mtxinib->ceilpri;
			}
		}

		/*
		 * move next item in the queue
		 */
		p_next = p_queue->p_next;
		if (p_next->p_prev != p_queue) {
			return(E_SYS_LINENO);
		}
		p_queue = p_next;
	}

	/*
	 *  check current priority
	 */
	if (p_tcb->priority != p_tcb->priority) {
		return(E_SYS_LINENO);
	}

	/*
	 *  check whether the task is waiting for some mutex
	 */
	if (TSTAT_WAIT_MTX(p_tcb->tstat)) {
		p_mtxcb = ((WINFO_MTX *)(p_tcb->p_winfo))->p_mtxcb;
		if (!VALID_MTXCB(p_mtxcb)) {
			return(E_SYS_LINENO);
		}
		if (!in_queue(&(p_mtxcb->wait_queue), &(p_tcb->task_queue))) {
			return(E_SYS_LINENO);
		}
	}
	return(E_OK);
}

/*
 *  check for mutex
 */
static ER
bit_mutex_mutex(ID mtxid)
{
	MTXCB		*p_mtxcb;
	TCB			*p_tcb;
	QUEUE		*p_queue, *p_next;
	uint_t		priority;

	if (!(TMIN_MTXID <= (mtxid) && (mtxid) <= tmax_mtxid)) {
		return(E_ID);
	}
	p_mtxcb = get_mtxcb(mtxid);

	/*
	 *  check the pointer to initialization block
	 */
	if (p_mtxcb->p_mtxinib != &(mtxinib_table[INDEX_MTX(mtxid)])) {
		return(E_SYS_LINENO);
	}

	/*
	 *  check the mutex waiting queue
	 */
	p_queue = p_mtxcb->wait_queue.p_next;
	if (p_queue->p_prev != &(p_mtxcb->wait_queue)) {
		return(E_SYS_LINENO);
	}
	priority = TMIN_TPRI;
	while (p_queue != &(p_mtxcb->wait_queue)) {
		p_tcb = (TCB *) p_queue;
		if (!VALID_TCB(p_tcb)) {
			return(E_SYS_LINENO);
		}

		/*
		 *  check whether the tasks are waiting 
		 *  in the order of priority
		 */
		if (MTXPROTO(p_mtxcb) != TA_NULL) {
			if (p_tcb->priority < priority) {
				return(E_SYS_LINENO);
			}
		}
		priority = p_tcb->priority;

		/*
		 *  check task state
		 *
		 */
		if (!TSTAT_WAIT_MTX(p_tcb->tstat)) {
			return(E_SYS_LINENO);
		}

		/*
		 *  check the ceiling priority
		 */
		if (MTXPROTO(p_mtxcb) == TA_CEILING) {
			if (p_tcb->bpriority < p_mtxcb->p_mtxinib->ceilpri) {
				return(E_SYS_LINENO);
			}
		}

		/*
		 *  proceed to the next element of the queue
		 */
		p_next = p_queue->p_next;
		if (p_next->p_prev != p_queue) {
			return(E_SYS_LINENO);
		}
		p_queue = p_next;
	}

	/*
	 *  move to next item
	 */
	p_tcb = p_mtxcb->p_loctsk;
	if (p_tcb == NULL) {
		/*
		 *  if the mutex is not locked
		 */
		if (!queue_empty(&(p_mtxcb->wait_queue))) {
			return(E_SYS_LINENO);
		}
	}
	else {
		/*
		 *  if the mutex is locked
		 *  
		 */
		if (!VALID_TCB(p_tcb)) {
			return(E_SYS_LINENO);
		}
		if (!in_queue(&(p_tcb->mutex_queue), &(p_mtxcb->mutex_queue))) {
			return(E_SYS_LINENO);
		}

		/*
		 *  check the ceiling priority 
		 */
		if (MTXPROTO(p_mtxcb) == TA_CEILING) {
			if (p_tcb->bpriority < p_mtxcb->p_mtxinib->ceilpri) {
				return(E_SYS_LINENO);
			}
		}
	}
	return(E_OK);
}

/*
 *  the main routine of mutex function integrity check
 */
ER
bit_mutex(void)
{
	ID		tskid, mtxid;
	ER		ercd;

	/*
	 *  check every task
	 */
	for (tskid = TMIN_TSKID; tskid <= tmax_tskid; tskid++) {
		ercd = bit_mutex_task(tskid);
		if (ercd != E_OK) {
			return(ercd);
		}
	}

	/*
	 *  check every mutex
	 */
	for (mtxid = TMIN_MTXID; mtxid <= tmax_mtxid; mtxid++) {
		ercd = bit_mutex_mutex(mtxid);
		if (ercd != E_OK) {
			return(ercd);
		}
	}
	return(E_OK);
}
