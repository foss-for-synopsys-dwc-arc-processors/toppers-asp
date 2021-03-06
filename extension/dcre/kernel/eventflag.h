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
 *  @(#) $Id: eventflag.h 1966 2010-11-20 07:23:56Z ertl-hiro $
 */

/*
 *		eventflag function
 */

#ifndef TOPPERS_EVENTFLAG_H
#define TOPPERS_EVENTFLAG_H

#include <queue.h>

/*
 *  eventflag init block
 *
 * this block is based on the structure of wait object init block,
 * can be viewed as subclass of wait object init block.
 *
 */
typedef struct eventflag_initialization_block {
	ATR			flgatr;			/* eventflag attribute */
	FLGPTN		iflgptn;		/* the init value of eventflag*/
} FLGINIB;

/*
 *  eventflag control block
 *
 * this block is based on the structure of wait object control block,
 * can be viewed as subclass of wait object control block.
 */
typedef struct eventflag_control_block {
	QUEUE		wait_queue;		/* eventflag waiting task queue */
	const FLGINIB *p_flginib;	/* pointer to eventflag init block */
	FLGPTN		flgptn;			/* current pattern value of eventflag */
} FLGCB;

/*
 *  eventflag wait information
 *
 * this block is based on the structure of wait object information block,
 * can be viewed as subclass of wait object information block.
 *
 *  to save memory, flgptn can share the memory with waiptn and wfmode in a union,
 *  because they won't be accessed simultaneously
 */
typedef struct eventflag_waiting_information {
	WINFO		winfo;			/* standard wait information block */
	FLGCB		*p_flgcb;		/* pointer to eventflag control block */
	FLGPTN		waiptn;			/* wait pattern */
	MODE		wfmode;			/* wait mode   */
	FLGPTN		flgptn;			/* eventflag pattern value afer wait */
} WINFO_FLG;

/*
 *  list of dataqueue control block which is not used
 */
extern QUEUE	free_flgcb;

/*
 *  max value of eventflag id (see kernel_cfg.c)
 */
extern const ID	tmax_flgid;
extern const ID	tmax_sflgid;

/*
 *  eventflag init block array (see kernel_cfg.c)
 */
extern const FLGINIB	flginib_table[];
extern FLGINIB			aflginib_table[];

/*
 *  eventflag control block array (see kernel_cfg.c)
 */
extern FLGCB	flgcb_table[];

/*
 *   get eventflag id from the pointer to eventflag control block
 */
#define	FLGID(p_flgcb)	((ID)(((p_flgcb) - flgcb_table) + TMIN_FLGID))

/*
 *  eventflag initialization
 */
extern void	initialize_eventflag(void);

/*
 *  check whether events specified in waiptn happpened
 *  current pattern value is returned by p_flgptn
 */
extern bool_t	check_flg_cond(FLGCB *p_flgcb, FLGPTN waiptn,
								MODE wfmode, FLGPTN *p_flgptn);

#endif /* TOPPERS_EVENTFLAG_H */
