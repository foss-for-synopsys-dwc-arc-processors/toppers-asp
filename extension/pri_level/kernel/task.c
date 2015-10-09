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
 *  $Id: task.c 2018 2010-12-31 13:43:05Z ertl-hiro $
 */

/*
 *		 task management module
 */

#include "kernel_impl.h"
#include "wait.h"
#include "task.h"

/*
 *  default definition of trace macro
 */
#ifndef LOG_TEX_ENTER
#define LOG_TEX_ENTER(p_tcb, texptn)
#endif /* LOG_TEX_ENTER */

#ifndef LOG_TEX_LEAVE
#define LOG_TEX_LEAVE(p_tcb)
#endif /* LOG_TEX_LEAVE */

#ifdef TOPPERS_tskini

/*
 *  current running task
 */
TCB		*p_runtsk;

/*
 *   next task to run
 */
TCB		*p_schedtsk;

/*
 *  task dispatch request/task exception request flag
 */
bool_t	reqflg;

/*
 *  interrupt priority mask flag
 *  true: no interrupts are masked
 *  false: some interrupts are masked
 */
bool_t	ipmflg;

/*
 *  disable dispatch flag
 *  ture: disable dispatch
 *  false: enable dispatch
 */
bool_t	disdsp;

/*
 *  dispatch pending flag
 *  ture:  dispatcher is not suspended,
 *  	  a task dispatch can be executed immediately
 *  false: dispatcher is suspended
 *  	   a task dispatch cannot be executed immediately
 *  disdsp sometimes is a mirror of dspflg
 *
 *  dispatch pending state
 *     1. in non-task context
 *     2. cpu is locked
 *     3. all interrupts are disabled
 *     4  ipm is not zero(some interrupts are masked)
 */
bool_t	dspflg;

/*
 *  ready queue
 *  each priority has a ready queue
 */
QUEUE	ready_queue[TNUM_TPRI];

/*
 *  Bitmap of the ready queue for search
 */
#ifdef PRIMAP_LEVEL_1
uint16_t	ready_primap;
#else /* PRIMAP_LEVEL_1 */
uint16_t	ready_primap1;
uint16_t	ready_primap2[TNUM_PRIMAP2];
#endif /* PRIMAP_LEVEL_1 */

/*
 *   task management module initialization
 */
void
initialize_task(void)
{
	uint_t	i, j;
	TCB		*p_tcb;

	p_runtsk = p_schedtsk = NULL;
	reqflg = false;
	ipmflg = true;
	disdsp = false;
	dspflg = true;

	for (i = 0; i < TNUM_TPRI; i++) {
		queue_initialize(&(ready_queue[i]));
	}
#ifdef PRIMAP_LEVEL_1
	ready_primap = 0U;
#else /* PRIMAP_LEVEL_1 */
	ready_primap1 = 0U;
	for (i = 0; i < TNUM_PRIMAP2; i++) {
		ready_primap2[i] = 0U;
	}
#endif /* PRIMAP_LEVEL_1 */

	for (i = 0; i < tnum_tsk; i++) {
		j = INDEX_TSK(torder_table[i]);
		p_tcb = &(tcb_table[j]);
		p_tcb->p_tinib = &(tinib_table[j]);
		p_tcb->actque = false;
		make_dormant(p_tcb);
		if ((p_tcb->p_tinib->tskatr & TA_ACT) != 0U) {
			make_active(p_tcb);
		}
	}
}

#endif /* TOPPERS_tskini */

/*
 *  bitmap search
 *
 *  search the 1st bit 1 in bitmap from right to left. the number of this
 *  bit 1 will be returned. the bitmap should not be zero. because 16 priorities
 *  are supported in defaut, bitmap is 16 bit unsigned int(uint16_t)
 *
 *  for the processors supporting bit search instructions, it's better to use
 *  hardware instructions than bitmap_search function. on this occasion, target
 *  dependent bitmap_search needs to be implemented, and the macro OMIT_BITMAP_SEARCH
 *  needs to be defined. The search order of bitmap can be reversed from left to right
 *  then the macro PRIMAP_BIT needs to be re-defined.
 *
 *
 *  if ffs is supported in library, the following definition is also possible
 *		#define	bitmap_search(bitmap) (ffs(bitmap) - 1)
 */
#ifndef PRIMAP_BIT
#define	PRIMAP_BIT(pri)		(1U << (pri))
#endif /* PRIMAP_BIT */

#ifndef OMIT_BITMAP_SEARCH

static const unsigned char bitmap_search_table[] = { 0, 1, 0, 2, 0, 1, 0,
												3, 0, 1, 0, 2, 0, 1, 0 };

Inline uint_t
bitmap_search(uint16_t bitmap)
{
	uint_t	n = 0U;

	assert(bitmap != 0U);
	if ((bitmap & 0x00ffU) == 0U) {
		bitmap >>= 8;
		n += 8;
	}
	if ((bitmap & 0x0fU) == 0U) {
		bitmap >>= 4;
		n += 4;
	}
	return(n + bitmap_search_table[(bitmap & 0x0fU) - 1]);
}

#endif /* OMIT_BITMAP_SEARCH */

/*
 *  check whether bitmap of ready queue is empty
 */
Inline bool_t
primap_empty(void)
{
#ifdef PRIMAP_LEVEL_1
	return(ready_primap == 0U);
#else /* PRIMAP_LEVEL_1 */
	return(ready_primap1 == 0U);
#endif /* PRIMAP_LEVEL_1 */
}

/*
 *  search bitmap of ready queue
 */
Inline uint_t
primap_search(void)
{
#ifdef PRIMAP_LEVEL_1
	return(bitmap_search(ready_primap));
#else /* PRIMAP_LEVEL_1 */
	uint_t	i;

	i = bitmap_search(ready_primap1);
	return(i * TBIT_PRIMAP + bitmap_search(ready_primap2[i]));
#endif /* PRIMAP_LEVEL_1 */
}

/*
 *  set bitmap of ready queue
 */
Inline void
primap_set(uint_t pri)
{
#ifdef PRIMAP_LEVEL_1
	ready_primap |= PRIMAP_BIT(pri);
#else /* PRIMAP_LEVEL_1 */
	ready_primap2[pri / TBIT_PRIMAP] |= PRIMAP_BIT(pri % TBIT_PRIMAP);
	ready_primap1 |= PRIMAP_BIT(pri / TBIT_PRIMAP);
#endif /* PRIMAP_LEVEL_1 */
}

/*
 *  clear bitmap of ready queue
 */
Inline void
primap_clear(uint_t pri)
{
#ifdef PRIMAP_LEVEL_1
	ready_primap &= ~PRIMAP_BIT(pri);
#else /* PRIMAP_LEVEL_1 */
	if ((ready_primap2[pri / TBIT_PRIMAP] &= ~PRIMAP_BIT(pri % TBIT_PRIMAP))
																	== 0U) {
		ready_primap1 &= ~PRIMAP_BIT(pri / TBIT_PRIMAP);
	}
#endif /* PRIMAP_LEVEL_1 */
}

/*
 *  search the task with highest precedence
 *  (search the next scheduled task)
 */
#ifdef TOPPERS_tsksched

TCB *
search_schedtsk(void)
{
	uint_t	schedpri;

	schedpri = primap_search();
	return((TCB *)(ready_queue[schedpri].p_next));
}

#endif /* TOPPERS_tsksched */

/*
 *  make the task runnable
 *
 *  update the task precedence.
 */
#ifdef TOPPERS_tskrun

bool_t
make_runnable(TCB *p_tcb)
{
	uint_t	pri = p_tcb->priority;

	queue_insert_prev(&(ready_queue[pri]), &(p_tcb->task_queue));
	primap_set(pri);

	if (p_schedtsk == (TCB *) NULL || pri < p_schedtsk->priority) {
		p_schedtsk = p_tcb;
		return(dspflg);
	}
	return(false);
}

#endif /* TOPPERS_tskrun */

/*
 *  make task unrunnable
 *
 *  when the task with highest precedence is made unrunnable,
 *  next task in ready queue with same priority will be scheduled.
 *  if the ready queue is empty, the next task should be scheduled will
 *  be searched
 */
#ifdef TOPPERS_tsknrun

bool_t
make_non_runnable(TCB *p_tcb)
{
	uint_t	pri = p_tcb->priority;
	QUEUE	*p_queue = &(ready_queue[pri]);

	queue_delete(&(p_tcb->task_queue));
	if (queue_empty(p_queue)) {
		primap_clear(pri);
		if (p_schedtsk == p_tcb) {
			p_schedtsk = primap_empty() ? (TCB *) NULL : search_schedtsk();
			return(dspflg);
		}
	}
	else {
		if (p_schedtsk == p_tcb) {
			p_schedtsk = (TCB *)(p_queue->p_next);
			return(dspflg);
		}
	}
	return(false);
}

#endif /* TOPPERS_tsknrun */

/*
 *   make the task dormant
 */
#ifdef TOPPERS_tskdmt

void
make_dormant(TCB *p_tcb)
{
	p_tcb->tstat = TS_DORMANT;
	p_tcb->priority = p_tcb->p_tinib->ipriority;
	p_tcb->wupque = false;
	p_tcb->enatex = false;
	p_tcb->texptn = 0U;
	LOG_TSKSTAT(p_tcb);
}

#endif /* TOPPERS_tskdmt */

/*
 *   make the task active
 */
#ifdef TOPPERS_tskact

bool_t
make_active(TCB *p_tcb)
{
	activate_context(p_tcb);
	p_tcb->tstat = TS_RUNNABLE;
	LOG_TSKSTAT(p_tcb);
	return(make_runnable(p_tcb));
}

#endif /* TOPPERS_tskact */

/*
 *  change task priority
 *
 *
 *  when task is runnable,  its position in ready queue will be changed.
 *  if task is in the task waiting queue of some wait object, its position
 *  will be changed
 */
#ifdef TOPPERS_tskpri

bool_t
change_priority(TCB *p_tcb, uint_t newpri)
{
	uint_t	oldpri;

	oldpri = p_tcb->priority;
	p_tcb->priority = newpri;

	if (TSTAT_RUNNABLE(p_tcb->tstat)) {
		/*
		 *   when task is runnable
		 */
		queue_delete(&(p_tcb->task_queue));
		if (queue_empty(&(ready_queue[oldpri]))) {
			primap_clear(oldpri);
		}
		queue_insert_prev(&(ready_queue[newpri]), &(p_tcb->task_queue));
		primap_set(newpri);

		if (p_schedtsk == p_tcb) {
			if (newpri >= oldpri) {
				p_schedtsk = search_schedtsk();
				return(p_schedtsk != p_tcb && dspflg);
			}
		}
		else {
			if (newpri < p_schedtsk->priority) {
				p_schedtsk = p_tcb;
				return(dspflg);
			}
		}
	}
	else {
		if (TSTAT_WAIT_WOBJCB(p_tcb->tstat)) {
			/*
			 *   when task is in the task waiting queue of some wait object
			 */
			wobj_change_priority(((WINFO_WOBJ *)(p_tcb->p_winfo))->p_wobjcb,
																	p_tcb);
		}
	}
	return(false);
}

#endif /* TOPPERS_tskpri */

/*
 *  rotate the precedence of tasks in the same ready queue
 *  specified by priority
 *
 *  the head task will be moved to tail.
 */
#ifdef TOPPERS_tskrot

bool_t
rotate_ready_queue(uint_t pri)
{
	QUEUE	*p_queue = &(ready_queue[pri]);
	QUEUE	*p_entry;

	if (!queue_empty(p_queue) && p_queue->p_next->p_next != p_queue) {
		p_entry = queue_delete_next(p_queue);
		queue_insert_prev(p_queue, p_entry);
		if (p_schedtsk == (TCB *) p_entry) {
			p_schedtsk = (TCB *)(p_queue->p_next);
			return(dspflg);
		}
	}
	return(false);
}

#endif /* TOPPERS_tskrot */

/*
 *  call task exception routine
 *
 *  after exception handling, return to the
 *  state before exception.
 */
#ifdef TOPPERS_tsktex

void
call_texrtn(void)
{
	TEXPTN	texptn;
	bool_t	saved_disdsp;

	saved_disdsp = disdsp;
	p_runtsk->enatex = false;
	do {
		texptn = p_runtsk->texptn;
		p_runtsk->texptn = 0U;

		t_unlock_cpu();
		LOG_TEX_ENTER(p_runtsk, texptn);
		(*((TEXRTN)(p_runtsk->p_tinib->texrtn)))(texptn,
												p_runtsk->p_tinib->exinf);
		LOG_TEX_LEAVE(p_runtsk);
		if (!t_sense_lock()) {
			t_lock_cpu();
		}
		if (!ipmflg) {
			t_set_ipm(TIPM_ENAALL);
			ipmflg = true;
		}
		disdsp = saved_disdsp;
		dspflg = !disdsp;
		p_runtsk->enatex = false;
		if (p_runtsk != p_schedtsk && dspflg) {
			/*
			 *  p_runtsk->enatex must be set false, because in
			 *  'dispatch', call_texrtn may be called again if enatex
			 *  is not false this will cost a lot of stack
			 *
			 *  if a task dispatch is required in task exception routine
			 *  but suspended. The dispacth pending state will be released and task dispatch will be
			 *  done here
			 */
			dispatch();
		}
	} while (p_runtsk->texptn != 0U);
	p_runtsk->enatex = true;
}

/*
 *   callback of task exception handling
 */
#ifndef OMIT_CALLTEX

void
calltex(void)
{
	if (p_runtsk->enatex && p_runtsk->texptn != 0U && ipmflg) {
		call_texrtn();
	}
}

#endif /* OMIT_CALLTEX */
#endif /* TOPPERS_tsktex */
