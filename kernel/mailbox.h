/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005,2006 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: mailbox.h 264 2007-07-12 08:23:10Z hiro $
 */

/*
 *	mailbox function
 */

#ifndef TOPPERS_MAILBOX_H
#define TOPPERS_MAILBOX_H

#include <queue.h>

/*
 *  mailbox init block
 *
 * this block is based on the structure of wait object init block,
 * can be viewed as subclass of wait object init block.
 */
typedef struct mailbox_initialization_block {
	ATR			mbxatr;			/* mailbox attribute */
	PRI			maxmpri;		/* max message priority */
} MBXINIB;

/*
 *  mailbox control block
 *
 *  this block is based on the structure of wait object information block,
 *  can be viewed as subclass of wait object information block.
 *
 *  when priority message queue used, pk_last has no meaning.
 *  when message queue is empty(pk_head is null), pk_last has no meaning
 *
 */
typedef struct mailbox_control_block {
	QUEUE		wait_queue;		/* mailbox waiting task queue*/
	const MBXINIB *p_mbxinib;	/* pointer to mailbox init block */
	T_MSG		*pk_head;		/* pointer to the head message */
	T_MSG		*pk_last;		/* pointer to the last message */
} MBXCB;

/*
 *  mailbox wait information
 *
 * this block is based on the structure of wait object information block,
 *  can be viewed as subclass of wait object information block.
 */
typedef struct mailbox_waiting_information {
	WINFO		winfo;			/* standard wait information block  */
	MBXCB		*p_mbxcb;		/* pointer to mailbox control block */
	T_MSG		*pk_msg;		/* received message */
} WINFO_MBX;

/*
 *   max value of mailbox id (see kernel_cfg.c)
 */
extern const ID	tmax_mbxid;

/*
 *  mailbox init block array (see kernel_cfg.c)
 */
extern const MBXINIB	mbxinib_table[];

/*
 *  mailbox control block array (see kernel_cfg.c)
 */
extern MBXCB	mbxcb_table[];

/*
 *  get mailbox id from the pointer to mailbox control block
 */
#define	MBXID(p_mbxcb)	((ID)(((p_mbxcb) - mbxcb_table) + TMIN_MBXID))

/*
 *  mailbox initializatio
 */
extern void	initialize_mailbox(void);

#endif /* TOPPERS_MAILBOX_H */
