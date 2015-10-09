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
 *  @(#) $Id: wait.h 1841 2010-07-10 17:37:34Z ertl-hiro $
 */

/*
 *	waiting status management module
 *
 */

#ifndef TOPPERS_WAIT_H
#define TOPPERS_WAIT_H

#include "task.h"
#include "time_event.h"

/*
 *  insert task into waiting queue according to task priority
 *
 *  if tasks with same priority are already in waiting queue,
 *  new task will be inserted into the tail of tasks with same
 *  priority
 */
Inline void
queue_insert_tpri(QUEUE *p_queue, TCB *p_tcb)
{
	QUEUE	*p_entry;
	uint_t	priority = p_tcb->priority;

	for (p_entry = p_queue->p_next; p_entry != p_queue;
										p_entry = p_entry->p_next) {
		if (priority < ((TCB *) p_entry)->priority) {
			break;
		}
	}
	queue_insert_prev(p_entry, &(p_tcb->task_queue));
}

/*
 *  make task into waiting status
 *
 *  make the current running task into waiting status and removed from
 *  ready queue.
 *  set the value of p_winfo and p_tmevtb
 *
 */
Inline void
make_wait(WINFO *p_winfo)
{
	(void) make_non_runnable(p_runtsk);
	p_runtsk->p_winfo = p_winfo;
	p_winfo->p_tmevtb = NULL;
}

/*
 *  make task into waiting status(with timeout)
 *
 *  make the current running task into waiting status and removed from
 *  ready queue.
 *  set the value of p_winfo and p_tmevtb.
 *  time event will be registered.
 */
extern void	make_wait_tmout(WINFO *p_winfo, TMEVTB *p_tmevtb, TMO tmout);

/*
 *  make the task get out of waiting status
 *
 *
 *  the task specified in p_tcb will get out of waiting status, and be made
 *  runnable if not suspended. if a dispatch should be made, return true,
 *  or false
 *
 */
Inline bool_t
make_non_wait(TCB *p_tcb)
{
	assert(TSTAT_WAITING(p_tcb->tstat));

	if (!TSTAT_SUSPENDED(p_tcb->tstat)) {
		/*
		 *  from waiting status to ready status
		 */
		p_tcb->tstat = TS_RUNNABLE;
		LOG_TSKSTAT(p_tcb);
		return(make_runnable(p_tcb));
	}
	else {
		/*
		 *  from wait-suspended status to suspended status
		 */
		p_tcb->tstat = TS_SUSPENDED;
		LOG_TSKSTAT(p_tcb);
		return(false);
	}
}

/*
 *  dequeue the task from waiting queue of wait objects
 *
 *
 */
Inline void
wait_dequeue_wobj(TCB *p_tcb)
{
	if (TSTAT_WAIT_WOBJ(p_tcb->tstat)) {
		queue_delete(&(p_tcb->task_queue));
	}
}

/*
 *  dequeue the time event of task if it is enqueued
 *
 *  the time event block of task specified by p_tcb will
 *  be dequeued from time event heap if it is enqueued before
 */
Inline void
wait_dequeue_tmevtb(TCB *p_tcb)
{
	if (p_tcb->p_winfo->p_tmevtb != NULL) {
		tmevtb_dequeue(p_tcb->p_winfo->p_tmevtb);
	}
}

/*
 *  wait complete
 *
 *  release the waiting state of task specified by p_tcb.
 *  if its time event block is enqueued, it will be dequeued.
 *  If a dispatch is needed, return true.
 */
extern bool_t	wait_complete(TCB *p_tcb);

/*
 *  wait complete when timeout
 *
 *  If the task specified by p_tcb is in some waiting queue, it will be
 *  dequeued from that queue, its status will be updated. For wait_tmout,
 *	the value of p_tcb->p_winfo->wercd is set to E_TMOUT, for
 *	wait_tmout_ok, the value of p_tcb->p_winfo->wercd is set to E_OK.
 *	If a dispatch is needed, reqflg will be seto true.
 *
 *  because wait_tmout_ok is only used in dly_tsk, there are no task
 *  dequeue operation in it.
 *
 *  these two functions, as time event's callback, are called in system
 *  time tick interrupt handler.
 *
 */
extern void	wait_tmout(TCB *p_tcb);
extern void	wait_tmout_ok(TCB *p_tcb);

/*
 *  forcibly relase the waiting state of task
 *
 * 	the task specified by p_tcb will be forcibly released from waiting
 * 	state. If the task is in some waiting queue, it will be
 *  dequeued from that queue. If its time event is enqueued, that
 *  time event will be dequeued. The value of p_tcb->p_winfo->wercd is
 *  set to E_RLWAI. If a dispatch is needed, return true.
 *
 */
extern bool_t	wait_release(TCB *p_tcb);

/*
 *  get the first task in task waiting queue
 *	specified by p_wait_queue
 *
 */
Inline ID
wait_tskid(QUEUE *p_wait_queue)
{
	if (!queue_empty(p_wait_queue)) {
		return(TSKID((TCB *) p_wait_queue->p_next));
	}
	else {
		return(TSK_NONE);
	}
}

/*
 *
 *  The synchronization and communication objects' init block and
 *  control block have the same head block. The following are the
 *  common blocks and routines for wait objects which are used for
 *  synchronization and communication
 *
 *  If the synchronization and communication objects have multi wait
 *  queues, the following routines can only be used to process the first
 *  wait queue(wait_queue in WOBJCB). The following routines will refer the
 *  TA_TPRI bit of object attribute, if the TA_TPRI bit of object
 *  attribute is used for other purposes, they can not be used.
 *
 */

/*
 *  common init block of wait objects
 */
typedef struct wait_object_initialization_block {
	ATR			wobjatr;		/* object attribute */
} WOBJINIB;

/*
 *  common control block of wait objects
 */
typedef struct wait_object_control_block {
	QUEUE		wait_queue;		/* waiting queue */
	const WOBJINIB *p_wobjinib;	/* pointer to init block */
} WOBJCB;

/*
 *  common wait info block of wait objects
 *
 */
typedef struct wait_object_waiting_information {
	WINFO	winfo;			/* standard wait information */
	WOBJCB	*p_wobjcb;		/* pointer to wait object control block */
} WINFO_WOBJ;

/*
 *  make task into waiting state and enqueue it into waiting queue.
 *  in wobj_make_wait_tmout, a time event block will be enqueued
 *
 */
extern void	wobj_make_wait(WOBJCB *p_wobjcb, WINFO_WOBJ *p_winfo);
extern void	wobj_make_wait_tmout(WOBJCB *p_wobjcb, WINFO_WOBJ *p_winfo,
											TMEVTB *p_tmevtb, TMO tmout);

/*
 *  change task's position in waiting queue when
 *  its priority is changed
 */
Inline void
wobj_change_priority(WOBJCB *p_wobjcb, TCB *p_tcb)
{
	if ((p_wobjcb->p_wobjinib->wobjatr & TA_TPRI) != 0U) {
		queue_delete(&(p_tcb->task_queue));
		queue_insert_tpri(&(p_wobjcb->wait_queue), p_tcb);
	}
}

/*
 *  waiting queue initialization
 *．
 *
 *  re-initialize the task waiting queue specified by p_wait_queue,
 *  where  all tasks will be release from waiting state.
 *  if a dispatch is necessary, return true
 *
 */
extern bool_t	init_wait_queue(QUEUE *p_wait_queue);

#endif /* TOPPERS_WAIT_H */
