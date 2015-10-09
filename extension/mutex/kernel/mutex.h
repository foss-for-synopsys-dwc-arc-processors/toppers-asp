/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
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
 * 
 *  $Id: task.h 2018 2010-12-31 13:43:05Z ertl-hiro $
 */

/*
 *		mutex functions 
 */

#ifndef TOPPERS_MUTEX_H
#define TOPPERS_MUTEX_H

#include <queue.h>

/*
 *  mutex initialization block
 *
 * this block is based on the structure of wait object init block,
 * can be viewed as subclass of wait object init block.
 */
typedef struct mutex_initialization_block {
	ATR			mtxatr;			/* mutex attribute */
	uint_t		ceilpri;		/* ceiling priority */
} MTXINIB;

/*
 *  mutex control block 
 *
 * this block is based on the structure of wait object control block,
 * can be viewed as subclass of wait object control block.
 */
typedef struct mutex_control_block {
	QUEUE		wait_queue;		/* mutex wait queue */
	const MTXINIB *p_mtxinib;	/* pointer to init block */
	TCB			*p_loctsk;		/* the task that lock the mutex */
	QUEUE		mutex_queue;	/* the mutex queue locked by task */
} MTXCB;

/*
 *  mutex wait information
 *
 * this block is based on the structure of wait object information block,
 * can be viewed as subclass of wait object information block.
 *
 */
typedef struct mutex_waiting_information {
	WINFO	winfo;			/* standard wait information block */
	MTXCB	*p_mtxcb;		/* pointer to mutex control block which the task waits */
} WINFO_MTX;

/*
 *  the max id of mutex (kernel_cfg.c)
 */
extern const ID	tmax_mtxid;

/*
 *  mutex init block table (kernel_cfg.c)
 */
extern const MTXINIB	mtxinib_table[];

/*
 *  mutex control block table (kernel_cfg.c)
 */
extern MTXCB	mtxcb_table[];

/*
 *  get mutex id from the pointer to mutex control block
 */
#define	MTXID(p_mtxcb)	((ID)(((p_mtxcb) - mtxcb_table) + TMIN_MTXID))

/*
 *   mutex initialization 
 */
extern void	initialize_mutex(void);

/*
 *  check whether the ceiling priority is violated 
 *
 *  this function checks whether ceiling priority is violated before
 *  chg_pri
 *  if the ceiling priority is lower (smaller value) than the bpriority
 *  of the task pointed by the p_tcb  and locking the mutex, return
 *  false. if not, return true.
 *
 */
extern bool_t	(*mtxhook_check_ceilpri)(TCB *p_tcb, uint_t bpriority);
extern bool_t	mutex_check_ceilpri(TCB *p_tcb, uint_t bpriority);

/* 
 *  check whether the task pointed by the p_tcb has locked some mutex.
 *  if some mutex is locked, return true, if not, return false.бе
 */
extern bool_t	(*mtxhook_scan_ceilmtx)(TCB *p_tcb);
extern bool_t	mutex_scan_ceilmtx(TCB *p_tcb);

/* 
 *  calculate the current priority of the task which locked the mutex
 *
 */
extern uint_t	mutex_calc_priority(TCB *p_tcb);

/*
 *  release mutex
 *
 *  release the mutex pointed by p_mtxcb. If there are other tasks
 *  waiting for this mutex, let other tasks lock this mutex.
 *  
 */
extern bool_t	mutex_release(MTXCB *p_mtxcb);

/*
 *  release all the mutexs locked by task
 *
 *  release all the mutexs locked by the task pointed by the p_tcb.  If
 *  there are other tasks waiting for the released mutex, let them lock
 *  it.
 *
 *  this function is called when the task exits, the task priority will
 *  not be changed. however, this function may chang other tasks'
 *  priority, so when this function is called, the dspflg should be
 *  checked. if necessary, make task dispatch.
 */
extern bool_t	(*mtxhook_release_all)(TCB *p_tcb);
extern bool_t	mutex_release_all(TCB *p_tcb);

#endif /* TOPPERS_MUTEX_H */
