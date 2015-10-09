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
 *  @(#) $Id: semaphore.h 264 2007-07-12 08:23:10Z hiro $
 */

/*
 *	semaphore function
 */

#ifndef TOPPERS_SEMAPHORE_H
#define TOPPERS_SEMAPHORE_H

#include <queue.h>

/*
 *  semaphore init block
 *
 * this block is based on the structure of wait object init block,
 * can be viewed as subclass of wait object init block.
 */
typedef struct semaphore_initialization_block {
	ATR			sematr;			/* semaphore attribute */
	uint_t		isemcnt;		/* init value of semaphore */
	uint_t		maxsem;			/* max count of semaphore */
} SEMINIB;

/*
 *  semaphore control block
 *
 * this block is based on the structure of wait object control block,
 * can be viewed as subclass of wait object control block.
 *
 */
typedef struct semaphore_control_block {
	QUEUE		wait_queue;		/* semaphore waiting task queue */
	const SEMINIB *p_seminib;	/* pointer to semaphore init block */
	uint_t		semcnt;			/* current count of semaphore */
} SEMCB;

/*
 *   semaphore wait information
 *
 * this block is based on the structure of wait object information block,
 * can be viewed as subclass of wait object information block
 *
 */
typedef struct semaphore_waiting_information {
	WINFO	winfo;			/* standard wait information block */
	SEMCB	*p_semcb;		/* pointer to semaphore control block */
} WINFO_SEM;

/*
 *  max value of semaphore id (see kernel_cfg.c)
 */
extern const ID	tmax_semid;

/*
 *  semaphore init block array (see kernel_cfg.c)
 */
extern const SEMINIB	seminib_table[];

/*
 * semaphore control block array (see kernel_cfg.c)
 */
extern SEMCB	semcb_table[];

/*
 *   get semaphore id from the pointer to semaphore control block
 */
#define	SEMID(p_semcb)	((ID)(((p_semcb) - semcb_table) + TMIN_SEMID))

/*
 *  semaphore initialization
 */
extern void	initialize_semaphore(void);

#endif /* TOPPERS_SEMAPHORE_H */
