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
 *  @(#) $Id: wait.c 1841 2010-07-10 17:37:34Z ertl-hiro $
 */

/*
 *		wait status management module
 */

#include "kernel_impl.h"
#include "wait.h"

/*
 *  make task into wait status(with timeout)
 */
#ifdef TOPPERS_waimake

void
make_wait_tmout(WINFO *p_winfo, TMEVTB *p_tmevtb, TMO tmout)
{
	(void) make_non_runnable(p_runtsk);
	p_runtsk->p_winfo = p_winfo;
	if (tmout > 0) {
		p_winfo->p_tmevtb = p_tmevtb;
		tmevtb_enqueue(p_tmevtb, (RELTIM) tmout,
						(CBACK) wait_tmout, (void *) p_runtsk);
	}
	else {
		assert(tmout == TMO_FEVR);
		p_winfo->p_tmevtb = NULL;
	}
}

#endif /* TOPPERS_waimake */

/*
 *  task wait complete
 */
#ifdef TOPPERS_waicmp

bool_t
wait_complete(TCB *p_tcb)
{
	wait_dequeue_tmevtb(p_tcb);
	p_tcb->p_winfo->wercd = E_OK;
	return(make_non_wait(p_tcb));
}

#endif /* TOPPERS_waicmp */

/*
 *  wait complete because of timeout
 */
#ifdef TOPPERS_waitmo

void
wait_tmout(TCB *p_tcb)
{
	wait_dequeue_wobj(p_tcb);
	p_tcb->p_winfo->wercd = E_TMOUT;
	if (make_non_wait(p_tcb)) {
		reqflg = true;
	}

	/*
	 *  unlock cpu,  so high priority interrupts
	 *  can be accepted and handled
	 */
	i_unlock_cpu();
	i_lock_cpu();
}

#endif /* TOPPERS_waitmo */
#ifdef TOPPERS_waitmook

void
wait_tmout_ok(TCB *p_tcb)
{
	p_tcb->p_winfo->wercd = E_OK;
	if (make_non_wait(p_tcb)) {
		reqflg = true;
	}

	/*
	 *  unlock cpu,  so high priority interrupts
	 *  can be accepted and handled
	 */
	i_unlock_cpu();
	i_lock_cpu();
}

#endif /* TOPPERS_waitmook */

/*
 *  forcibly release the wait statu of task
 */
#ifdef TOPPERS_wairel

bool_t
wait_release(TCB *p_tcb)
{
	wait_dequeue_wobj(p_tcb);
	wait_dequeue_tmevtb(p_tcb);
	p_tcb->p_winfo->wercd = E_RLWAI;
	return(make_non_wait(p_tcb));
}

#endif /* TOPPERS_wairel */

/*
 *  insert task into waiting queue of related wait objects. 
 * 
 *  the task can be inserted in FIFO or in priority according attribute of
 *  related wait objects
 *
 */
Inline void
wobj_queue_insert(WOBJCB *p_wobjcb)
{
	if ((p_wobjcb->p_wobjinib->wobjatr & TA_TPRI) != 0U) {
		queue_insert_tpri(&(p_wobjcb->wait_queue), p_runtsk);
	}
	else {
		queue_insert_prev(&(p_wobjcb->wait_queue), &(p_runtsk->task_queue));
	}
}

/*
 * make task into wait status and insert task into waiting queue
 * of related wait objects
 */
#ifdef TOPPERS_wobjwai

void
wobj_make_wait(WOBJCB *p_wobjcb, WINFO_WOBJ *p_winfo_wobj)
{
	make_wait(&(p_winfo_wobj->winfo));
	wobj_queue_insert(p_wobjcb);
	p_winfo_wobj->p_wobjcb = p_wobjcb;
	LOG_TSKSTAT(p_runtsk);
}

#endif /* TOPPERS_wobjwai */
#ifdef TOPPERS_wobjwaitmo
/*
 * make task into wait status and into waiting queue
 * of related wait objects(with timeout)
 */
void
wobj_make_wait_tmout(WOBJCB *p_wobjcb, WINFO_WOBJ *p_winfo_wobj,
								TMEVTB *p_tmevtb, TMO tmout)
{
	make_wait_tmout(&(p_winfo_wobj->winfo), p_tmevtb, tmout);
	wobj_queue_insert(p_wobjcb);
	p_winfo_wobj->p_wobjcb = p_wobjcb;
	LOG_TSKSTAT(p_runtsk);
}

#endif /* TOPPERS_wobjwaitmo */

/*
 *  waiting queue initialization
 */
#ifdef TOPPERS_iniwque

bool_t
init_wait_queue(QUEUE *p_wait_queue)
{
	TCB		*p_tcb;
	bool_t	dspreq = false;

	while (!queue_empty(p_wait_queue)) {
		p_tcb = (TCB *) queue_delete_next(p_wait_queue);
		wait_dequeue_tmevtb(p_tcb);
		p_tcb->p_winfo->wercd = E_DLT;
		if (make_non_wait(p_tcb)) {
			dspreq = true;
		};
	}
	return(dspreq);
}

#endif /* TOPPERS_iniwque */
