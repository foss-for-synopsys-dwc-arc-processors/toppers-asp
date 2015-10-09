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
 *  @(#) $Id: pridataq.h 748 2008-03-07 17:18:06Z hiro $
 */


/*
 *	priority dataqueue function
 */

#ifndef TOPPERS_PRIDATAQ_H
#define TOPPERS_PRIDATAQ_H

#include <queue.h>

/*
 *  priority dataqueue management block
 *  (data block)
 */
typedef struct pridata_management_block PDQMB;

struct pridata_management_block {
	PDQMB		*p_next;		/* point to next one */
	intptr_t	data;			/* data  */
	PRI			datapri;		/* data priority */
};

/*
 *  priority dataqueue init block
 *
 *   this block is based on the structure of wait object init block,
 * can be viewed as subclass of wait object init block.
 */
typedef struct pridataq_initialization_block {
	ATR			pdqatr;			/* priority dataqueue attribute */
	uint_t		pdqcnt;			/* capacity of priority dataqueue */
	PRI			maxdpri;		/* max priority*/
	PDQMB		*p_pdqmb;		/* pointer to priority dataqueue management block area */
} PDQINIB;

/*
 *  priority dataqueue control block
 *
 * this block is based on the structure of wait object control block,
 * can be viewed as subclass of wait object control block.
 */
typedef struct pridataq_control_block {
	QUEUE		swait_queue;	/* priority dataqueue send waiting task queue */
	const PDQINIB *p_pdqinib;	/* pointer to init block */
	QUEUE		rwait_queue;	/* priority dataqueue receive waiting task queue*/
	uint_t		count;			/* data count in priority dataqueue */
	PDQMB		*p_head;		/* first data block  */
	uint_t		unused;			/* first unused data block */
	PDQMB		*p_freelist;	/* free list of data block */
} PDQCB;

/*
 *
 * priority dataqueue wait information
 *
 * this block is based on the structure of wait object information block,
 * can be viewed as subclass of wait object information block.
 *
 *  send and receive share the same wait information for each task
 */
typedef struct pridataq_waiting_information {
	WINFO		winfo;			/* standard wait information block */
	PDQCB		*p_pdqcb;		/* pointer to priority dataqueue control block */
	intptr_t	data;			/* data(receive or send) */
	PRI			datapri;		/* data priority */
} WINFO_PDQ;

/*
 *   max value of eventflag id (see kernel_cfg.c)
 */
extern const ID	tmax_pdqid;

/*
 *   eventflag init block array (see kernel_cfg.c)
 */
extern const PDQINIB	pdqinib_table[];

/*
 * eventflag control block array (see kernel_cfg.c)
 */
extern PDQCB	pdqcb_table[];

/*
 * get eventflag id from the pointer to eventflag control block
 */
#define	PDQID(p_pdqcb)	((ID)(((p_pdqcb) - pdqcb_table) + TMIN_PDQID))

/*
 *  eventflag initialization
 */
extern void	initialize_pridataq(void);

/*
 *  enqueue data into priority dataqueue management block(data block) area
 */
extern void	enqueue_pridata(PDQCB *p_pdqcb, intptr_t data, PRI datapri);

/*
 *  dequeue data from priority dataqueue management block(data block) area
 */
extern void	dequeue_pridata(PDQCB *p_pdqcb, intptr_t *p_data, PRI *p_datapri);

/*
 *  send data into priority dataqueue
 */
extern bool_t	send_pridata(PDQCB *p_pdqcb, intptr_t data,
											PRI datapri, bool_t *p_reqdsp);

/*
 *  receive data from priority dataqueue
 */
extern bool_t	receive_pridata(PDQCB *p_pdqcb, intptr_t *p_data,
											PRI *p_datapri, bool_t *p_reqdsp);

#endif /* TOPPERS_PRIDATAQ_H */
