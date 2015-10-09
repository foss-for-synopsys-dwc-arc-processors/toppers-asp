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
 *  $Id: task.h 2008 2010-12-31 12:41:42Z ertl-hiro $
 */


/*
 *	task management module
 */

#ifndef TOPPERS_TASK_H
#define TOPPERS_TASK_H

#include <queue.h>
#include "time_event.h"

/*
 *  default definition of trace macro
 */
#ifndef LOG_TSKSTAT
#define LOG_TSKSTAT(p_tcb)
#endif /* LOG_TSKSTAT */

/*
 *  macro of switch between internal priority and external priority
 */
#define INT_PRIORITY(x)		((uint_t)((x) - TMIN_TPRI))
#define EXT_TSKPRI(x)		((PRI)(x) + TMIN_TPRI)

/*
 *  task states
 *
 *
 *  There is no difference between task running and task ready, both can be
 *  viewed as task runnable.
 *
 *  waiting-suspended state means task in the mixed state of waiting and suspended
 */
#define TS_DORMANT		0x00U			/* dormant state */
#define TS_RUNNABLE		0x01U			/* runnable(ready and running) state */
#define TS_WAITING		0x02U			/* waiting state */
#define TS_SUSPENDED	0x04U			/* suspended state */

/* wait reasons */
#define TS_WAIT_DLY		(0x00U << 3)	/* wait time to pass */
#define TS_WAIT_SLP		(0x01U << 3)	/* wait to be waked up */
#define TS_WAIT_RDTQ	(0x02U << 3)	/* wait to receive dataqueue */
#define TS_WAIT_RPDQ	(0x03U << 3)	/* wait to receive priority dataqueue */
#define TS_WAIT_SEM		(0x04U << 3)	/* wait to get semaphore */
#define TS_WAIT_FLG		(0x05U << 3)	/* wait for eventflag */
#define TS_WAIT_SDTQ	(0x06U << 3)	/* wait to send dataqueue */
#define TS_WAIT_SPDQ	(0x07U << 3)	/* wait to send priority dataqueue */
#define TS_WAIT_MBX		(0x08U << 3)	/* wait to receive from mailbox */
#define TS_WAIT_MPF		(0x09U << 3)	/* wait to get memory pool */

/*
 *  identify task state
 *
 *  TSTAT_DORMANT	: whether task is in dormant state
 *  TSTAT_RUNNABLE 	: whether task is runnable
 *  TSTAT_WAITING	: whether task is in waiting
 *  TSTAT_SUSPENDED : whether task is suspended
 */
#define TSTAT_DORMANT(tstat)	((tstat) == TS_DORMANT)
#define TSTAT_RUNNABLE(tstat)	(((tstat) & TS_RUNNABLE) != 0U)
#define TSTAT_WAITING(tstat)	(((tstat) & TS_WAITING) != 0U)
#define TSTAT_SUSPENDED(tstat)	(((tstat) & TS_SUSPENDED) != 0U)

/*
 *  identify the reason of task waiting
 *
 *  TSTAT_WAIT_SLP is to check whether task is in sleep, TSTAT_WAIT_WOBJ
 *  is to  check whether task is waiting for some synchronization and
 *  communication object(whether task is in the waiting queue of
 *  synchronization and communication object). TSTAT_WAIT_WOBJCB is to
 *  check whether task is in the waiting queue of common management
 *  block(WOBJCB) of synchronization and communication object.
 *
 */
#define TS_WAIT_MASK	(0x0fU << 3)	/* mask of waiting reason */

#define TSTAT_WAIT_SLP(tstat)		(((tstat) & TS_WAIT_MASK) == TS_WAIT_SLP)
#define TSTAT_WAIT_WOBJ(tstat)		(((tstat) & TS_WAIT_MASK) >= TS_WAIT_RDTQ)
#define TSTAT_WAIT_WOBJCB(tstat)	(((tstat) & TS_WAIT_MASK) >= TS_WAIT_SEM)

/*
 *   standard wait information
 *
 *  when task is in waiting state, the settings of TCB and WINFO pointed by
 *  p_winfo are the following :
 *
 *  (a) TCB's tstat is set to TS_WAITING, the waiting reason should also
 *  be written to tstat.
 *
 *  (b) When timeout is needed, a new time event block should be
 *  registed．This time event block is a local variable in service calls
 *  which make the task into waiting states. The p_tmevtb of WINFO will
 *  be set to the address of this time event block. If timeout is not
 *  needed, p_tmevtb is set to NULL
 *
 *  For synchronization and communication objects, WINFO_WOBJ(defined in
 *  wait.h) is used and has a new field of p_wobjcb. Then, the
 *  processings of (c)-(e) are necessary. For other situations(sleep,
 *  and task delay), the processings of (c)-(e) are not necessary.
 *
 *  (c) use task_queue of TCB to put the task into the waiting queue
 *  of synchronization and communication objects
 *
 *  (d) set the p_wobjcb of WINFO_WOBJ to the address of control block
 *  of synchronization and communication objects
 *
 *  (e) If additional information of synchronization and communication
 *  objects is needed(e.g. count for semaphore),  new data structure based
 *  on WINFO_WOBJ is defined and used instead of WINFO_WOBJ.
 *
 *  when task is released from waiting state，the waiting result is stored
 *  in wercd of WINFO. To save memory usage, wercd and p_tmevtb are in
 *  one union, and wercd will be used after task is released from waiting
 *  state, so that time event block must be unregistered before wercd is
 *  set.
 */
typedef union waiting_information {
	ER		wercd;			/* error code when tasks get out of wait status*/
	TMEVTB	*p_tmevtb;		/* time event for wait */
} WINFO;

/*
 *  task init block
 *
 *  The part that won't be changed of task information will be put in
 *  ROM(task init block). The part that may be changed of task
 *  information will be put in RAM(task control block, TCB). During
 *  initialization, TCB's init block pointer will be set to the address
 *  of task init block. This method is better in performance than
 *  storing TCB's address in task init block, although which can save
 *  more RAM. This method also is applied for other objects.
 *
 * 	The information of task exception is also stored in task init block.
 */
typedef struct task_initialization_block {
	ATR			tskatr;			/* task attribute */
	intptr_t	exinf;			/* task parameter */
	TASK		task;			/* task address */
	uint_t		ipriority;		/* task init priority (internal) */

#ifdef USE_TSKINICTXB
	TSKINICTXB	tskinictxb;		/* task init block */
#else /* USE_TSKINICTXB */
	SIZE		stksz;			/* task stack size(round value) */
	void		*stk;			/* tast stack */
#endif /* USE_TSKINICTXB */

	ATR			texatr;			/* task exception attribute*/
	TEXRTN		texrtn;			/* task exception routine */
} TINIB;

/*
 *  priority's bit width in TCB's bit field
 *
 *  Dependent on processor's architecture, there exists a trade off
 *  between memory overhead of bit field in TCB and performance.
 */
#ifndef TBIT_TCB_PRIORITY
#define	TBIT_TCB_PRIORITY		8		/* priority's bit width */
#endif /* TBIT_TCB_PRIORITY */

/*
 *  Task Control Blcok(TCB)
 *
 *  In ASP kernel, because the max activate request and the max wakeup
 *  request is set 1, just two bool value can be used to check whether
 *  there is an activate request or wakeup request. Because max count
 *  of suspend request(TMAX_SUSCNT) is 1, there is no need of suspend
 *  nest count(suscnt).
 *
 *  The fields in TCB are only valid during specific task state. The
 *  valid conditions are listed below:
 *
 *  # valid after initialization:
 *  		p_tinib，tstat，actque
 *  # valid except dormant state(dormant state is default status)
 *  		priority，wupque，enatex，texptn
 *  # vaild in waiting state(including waiting-suspended):
 *  		p_winfo
 *  # valid in ready state and waiting state(for synchronization and
 *    communication state):
 *  		task_queue
 *  # valid in ready state, waiting state, suspended state and
 *  waiting-suspeneded state:
 *  		tskctxb
 */
typedef struct task_control_block {
	QUEUE			task_queue;		/* task queue */
	const TINIB		*p_tinib;		/* pointer to task init block */

#ifdef UINT8_MAX
	uint8_t			tstat;			/* task status */
#else /* UINT8_MAX */
	BIT_FIELD_UINT	tstat : 8;		/* task status(bit field）*/
#endif /* UINT8_MAX */
#if defined(UINT8_MAX) && (TBIT_TCB_PRIORITY == 8)
	uint8_t			priority;		/* current priority*/
#else /* defined(UINT8_MAX) && (TBIT_TCB_PRIORITY == 8) */
	BIT_FIELD_UINT	priority : TBIT_TCB_PRIORITY;
									/* current priority */
#endif  /* defined(UINT8_MAX) && (TBIT_TCB_PRIORITY == 8) */
	BIT_FIELD_BOOL	actque : 1;		/* whether a activation request exists */
	BIT_FIELD_BOOL	wupque : 1;		/* whether a wakeup request exists */
	BIT_FIELD_BOOL	enatex : 1;		/* whether task exception handling is enabled */

	TEXPTN			texptn;			/* task exception pattern */
	WINFO			*p_winfo;		/* pointer to task wait information */
	TSKCTXB			tskctxb;		/* task context block */
} TCB;

/*
 *  current running task
 *
 *  TCB pointer of current running task. When no task is running,
 *  p_runtsk = NULL.
 *
 *  In service call, p_runtsk is used to refer the information of
 *  calling task. Only in the dispatcher and initialization, p_runtsk
 *  maybe modified.
 */
extern TCB	*p_runtsk;

/*
 *  next to run task
 *
 *  the task with highest precedence in the ready task. When no tasks
 *  are ready, p_schedtsk = NULL
 *
 *  When dispatcher is disabled or suspeneded, it's not limited that
 *  p_schedtsk the same with p_schedtsk (p_schedtsk may be different
 *  with p_runtsk).
 */
extern TCB	*p_schedtsk;

/*
 *  dispatch/task exception handling request flag
 *
 *  In the exit of interrupt handler/CPU exception handler, this flag
 *  shows that a diapatch or some task's exception handling is needed
 */
extern bool_t	reqflg;

/*
 *  interrupt priority mask flag
 *
 *  when interrupt priority mask is not zero,  which means some interrupts
 *  with low priority are masked, ipmflg is true.
 */
extern bool_t	ipmflg;

/*
 *  dispatch disable flag
 *
 *  ture: dispatch is disabled
 *  false: dispatch is enabled
 */
extern bool_t	disdsp;

/*
 *  dispatch possible flag
 *
 *  when ipm is zero, meaning dispatcher is allowed to work, dspflg is
 *  true.
 */
extern bool_t	dspflg;

/*
 *  ready queue
 *
 *  ready queue is a queue of tasks ready to run. every priority has a
 *  ready queue
 */
extern QUEUE	ready_queue[TNUM_TPRI];

/*
 *  bitmap for ready queue
 *
 *  ready_primap is used to search efficiently the right ready queue
 *  of tasks. Although the use of bitmap can reduce the memory access,
 *  if the target processor dose not have instructions for bitmap
 *  operations, or the priorites are few, maybe it's not very efficient.
 *
 *  In defaut, there are 16 priorities,  ready_primap's type is uint16_t
 */
extern uint16_t	ready_primap;

/*
 *  max value of task id(see kernel_cfg.c)
 */
extern const ID	tmax_tskid;

/*
 *  task init block area(see kernel_cfg.c)
 */
extern const TINIB	tinib_table[];

/*
 *  task order table (see kernel_cfg.c)
 */
extern const ID	torder_table[];

/*
 *  task control block table (see kernel_cfg.c)
 */
extern TCB	tcb_table[];

/*
 *  total number of task
 */
#define tnum_tsk	((uint_t)(tmax_tskid - TMIN_TSKID + 1))

/*
 *  get task control block from task id
 */
#define INDEX_TSK(tskid)	((uint_t)((tskid) - TMIN_TSKID))
#define get_tcb(tskid)		(&(tcb_table[INDEX_TSK(tskid)]))
#define get_tcb_self(tskid)	((tskid) == TSK_SELF ? p_runtsk : get_tcb(tskid))

/*
 *  get task id from task control block
 */
#define	TSKID(p_tcb)	((ID)(((p_tcb) - tcb_table) + TMIN_TSKID))

/*
 *  task management module initialization
 */
extern void	initialize_task(void);

/*
 *
 *  search task with highest precedence.
 *
 *  search task with highest precedence in task ready queue, the address
 *  of TCB will be returned. if ready queue is empty, this function should not
 *  be called
 */
extern TCB	*search_schedtsk(void);

/*
 *  make task runnable(ready state)
 *
 *  make task runnable and enqueue it into ready queue.
 *  if a dispatch is required, return true, or false
 */
extern bool_t	make_runnable(TCB *p_tcb);

/*
 *  make task un-runnable(from ready to other states)
 *
 *  remove task from ready queue.
 *  if a dispatch is required, return true, or false
 */
extern bool_t	make_non_runnable(TCB *p_tcb);

/*
 *  make task into dormant state
 *
 *  task will be re-initialized
 */
extern void	make_dormant(TCB *p_tcb);

/*
 *  make task active
 *
 *  make task active
   if a dispatch is required, return true, or false
 */
extern bool_t	make_active(TCB *p_tcb);

/*
 *  change task priority
 *
 *．change task priority to new priority.
 *  if a dispatch is required, return true, or false
 */
extern bool_t	change_priority(TCB *p_tcb, uint_t newpri);

/*
 *  rotate task precedence in the ready queue with specified priority
 *
 *  if a dispatch is required, return true, or false
 */
extern bool_t	rotate_ready_queue(uint_t pri);

/*
 *  call task exception routine
 *
 *  call task exception routine. Before task exception routine is
 *  called, the exception pattern(texptn) of running task will be
 *  cleared, TCB's enatex will be set false, and CPU will be unlocked.
 *
 *  After return from task exception routine, first cpu will be locked,
 *
 *  if exception pattern(texptn) of running task is not 0, the task
 *  exception routine will be called again.
 *
 *  call_texrtn is designed to be called when enatex is true and texptn
 *  is not 0. When CPU is locked, call_texptn will not be called.
 *
 */
extern void	call_texrtn(void);

/*
 *  callback of task exception
 *
 *
 *  the conditions of task exception are met, task exception routine will
 *  be called in this function and CPU is unlocked
 *
 *
 *  this function will be called in dispatcher and exit of interrupt handler
 *  and exception handler, and only be called when CPU is locked
 *
 *  this function can also be implemented in target dependent parts. On this
 *  occasion, macro OMIT_CALLTEX must be defined.
 *
 */
extern void	calltex(void);

#endif /* TOPPERS_TASK_H */
