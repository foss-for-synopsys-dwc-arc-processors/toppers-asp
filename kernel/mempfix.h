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
 *  @(#) $Id: mempfix.h 264 2007-07-12 08:23:10Z hiro $
 */

/*
 *		fixed size memory pool
 */

#ifndef TOPPERS_MEMPFIX_H
#define TOPPERS_MEMPFIX_H

#include <queue.h>

/*
 * memory pool management block
 *
 *  next is  the index of next empty memory block.
 *  when all blocks are allocated, next is INDEX_ALLOC.
 *  when only one block left empty, next is INDEX_NULL.
 *
 */
typedef struct fixed_memoryblock_management_block {
	uint_t		next;			/* next empty block */
} MPFMB;

/*
 *  memory pool init block
 *
 * this block is based on the structure of wait object init block,
 * can be viewed as subclass of wait object init block.
 */
typedef struct fixed_memorypool_initialization_block {
	ATR			mpfatr;			/* memory pool attribute */
	uint_t		blkcnt;			/* number of memory blocks*/
	uint_t		blksz;			/* size of memory block(round value) */
	void		*mpf;			/* pointer to memory pool area */
	MPFMB		*p_mpfmb;		/* pointer to memory management block */
} MPFINIB;

/*
 *  memory pool control block
 *
 * this block is based on the structure of wait object control block,
 * can be viewed as subclass of wait object control block.
 */
typedef struct fixed_memorypool_control_block {
	QUEUE		wait_queue;		/* memory pool waiting task queue*/
	const MPFINIB *p_mpfinib;	/* pointer to memory pool init block */
	uint_t		fblkcnt;		/* number of free blocks */
	uint_t		unused;			/* the first unused block */
	uint_t		freelist;		/* list of free blocks */
} MPFCB;

/*
 *  memory pool wait information
 *
 * this block is based on the structure of wait object information block,
 * can be viewed as subclass of wait object information block.．
 */
typedef struct fixed_memorypool_waiting_information {
	WINFO		winfo;			/* standard wait information block  */
	MPFCB		*p_mpfcb;		/* pointer to memory pool control block*/
	void		*blk;			/* gotten memory block */
} WINFO_MPF;

/*
 *   max value of memory pool id (see kernel_cfg.c)
 */
extern const ID	tmax_mpfid;

/*
 *   memory pool init block array (see kernel_cfg.c)
 */
extern const MPFINIB	mpfinib_table[];

/*
 * memory pool control block array (see kernel_cfg.c)
 */
extern MPFCB	mpfcb_table[];

/*
 *   get memory pool id from the pointer to memory pool control block
 *
 */
#define	MPFID(p_mpfcb)	((ID)(((p_mpfcb) - mpfcb_table) + TMIN_MPFID))

/*
 *   memory pool initialization
 */
extern void	initialize_mempfix(void);

/*
 *  get memory block from memory poll through memory pool control block
 */
extern void	get_mpf_block(MPFCB *p_mpfcb, void **p_blk);

#endif /* TOPPERS_MEMPFIX_H */
