/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  @(#) $Id: dataqueue.h 1960 2010-11-20 02:20:40Z ertl-hiro $
 */

/*
 *	dataqueue function
 */

#ifndef TOPPERS_DATAQUEUE_H
#define TOPPERS_DATAQUEUE_H

#include <queue.h>

/*
 *  dataqueue management block
 */
typedef struct data_management_block {
	intptr_t	data;			/* pointer to the array for dataqueue */
} DTQMB;

/*
 * dataqueue init block
 *
 * this block is based on the structure of wait object init block,
 * can be viewed as subclass of wait object init block.
 *
 */
typedef struct dataqueue_initialization_block {
	ATR			dtqatr;			/* dataqueue attribute */
	uint_t		dtqcnt;			/* dataqueue capacity  */
	DTQMB		*p_dtqmb;		/* pointer to dataqueue management block */
} DTQINIB;

/*
 *  dataqueue control block
 *
 * this block is based on the structure of wait object control block,
 * can be viewed as subclass of wait object control block.
 *
 */
typedef struct dataqueue_control_block {
	QUEUE		swait_queue;	/* send task waiting queue */
	const DTQINIB *p_dtqinib;	/* pointer to dataqueue init block */
	QUEUE		rwait_queue;	/* receive task waiting queue */
	uint_t		count;			/* data count in dataqueue */
	uint_t		head;			/* head pointer */
	uint_t		tail;			/* tail pointer */
} DTQCB;

/*
 *  dataqueue wait information
 *
 * this block is based on the structure of wait object information block,
 * can be viewed as subclass of wait object information block.
 * 
 *  send and receive share the same dataqueue waiting information block
 */
typedef struct dataqueue_waiting_information {
	WINFO		winfo;			/* standard wait information block */
	DTQCB		*p_dtqcb;		/* pointer to dataqueue control block */
	intptr_t	data;			/* data for send or receive */
} WINFO_DTQ;

/*
 *  max value of dataqueue id (see kernel_cfg.c)
 */
extern const ID	tmax_dtqid;

/*
 * dataqueue init block array (see kernel_cfg.c)
 */
extern const DTQINIB	dtqinib_table[];

/*
 * dataqueue control block array (see kernel_cfg.c)
 */
extern DTQCB	dtqcb_table[];

/*
 *  get dataqueue id from the pointer to dataqueue control block
 */
#define	DTQID(p_dtqcb)	((ID)(((p_dtqcb) - dtqcb_table) + TMIN_DTQID))

/*
 *  dataqueue initialization 
 */
extern void	initialize_dataqueue(void);

/*
 *  enqueue data 
 */
extern void	enqueue_data(DTQCB *p_dtqcb, intptr_t data);

/*
 *  force to enqueue data
 */
extern void	force_enqueue_data(DTQCB *p_dtqcb, intptr_t data);

/*
 *  dequeue data
 */
extern void	dequeue_data(DTQCB *p_dtqcb, intptr_t *p_data);

/*
 *  send data into dataqueue
 */
extern bool_t	send_data(DTQCB *p_dtqcb, intptr_t data, bool_t *p_reqdsp);

/*
 *  force to send data inot dataqueue
 */
extern bool_t	force_send_data(DTQCB *p_dtqcb, intptr_t data);

/*
 *  receive data from dataqueue
 */
extern bool_t	receive_data(DTQCB *p_dtqcb, intptr_t *p_data,
													bool_t *p_reqdsp);

#endif /* TOPPERS_DATAQUEUE_H */
