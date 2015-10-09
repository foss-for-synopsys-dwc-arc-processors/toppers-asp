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
 *  @(#) $Id: mempfix.c 748 2008-03-07 17:18:06Z hiro $ 
 */

/*
 *		fixed size memory pool
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "mempfix.h"

/*
 *   default definition of trace macro
 */
#ifndef LOG_GET_MPF_ENTER
#define LOG_GET_MPF_ENTER(mpfid, p_blk)
#endif /* LOG_GET_MPF_ENTER */

#ifndef LOG_GET_MPF_LEAVE
#define LOG_GET_MPF_LEAVE(ercd, blk)
#endif /* LOG_GET_MPF_LEAVE */

#ifndef LOG_PGET_MPF_ENTER
#define LOG_PGET_MPF_ENTER(mpfid, p_blk)
#endif /* LOG_PGET_MPF_ENTER */

#ifndef LOG_PGET_MPF_LEAVE
#define LOG_PGET_MPF_LEAVE(ercd, blk)
#endif /* LOG_PGET_MPF_LEAVE */

#ifndef LOG_TGET_MPF_ENTER
#define LOG_TGET_MPF_ENTER(mpfid, p_blk, tmout)
#endif /* LOG_TGET_MPF_ENTER */

#ifndef LOG_TGET_MPF_LEAVE
#define LOG_TGET_MPF_LEAVE(ercd, blk)
#endif /* LOG_TGET_MPF_LEAVE */

#ifndef LOG_REL_MPF_ENTER
#define LOG_REL_MPF_ENTER(mpfid, blk)
#endif /* LOG_REL_MPF_ENTER */

#ifndef LOG_REL_MPF_LEAVE
#define LOG_REL_MPF_LEAVE(ercd)
#endif /* LOG_REL_MPF_LEAVE */

#ifndef LOG_INI_MPF_ENTER
#define LOG_INI_MPF_ENTER(mpfid)
#endif /* LOG_INI_MPF_ENTER */

#ifndef LOG_INI_MPF_LEAVE
#define LOG_INI_MPF_LEAVE(ercd)
#endif /* LOG_INI_MPF_LEAVE */

#ifndef LOG_REF_MPF_ENTER
#define LOG_REF_MPF_ENTER(mpfid, pk_rmpf)
#endif /* LOG_REF_MPF_ENTER */

#ifndef LOG_REF_MPF_LEAVE
#define LOG_REF_MPF_LEAVE(ercd, pk_rmpf)
#endif /* LOG_REF_MPF_LEAVE */

/*
 *   total number of memory pool
 */
#define tnum_mpf	((uint_t)(tmax_mpfid - TMIN_MPFID + 1))

/*
 * get memory pool control block from memory pool id
 */
#define INDEX_MPF(mpfid)	((uint_t)((mpfid) - TMIN_MPFID))
#define get_mpfcb(mpfid)	(&(mpfcb_table[INDEX_MPF(mpfid)]))

/*
 *  special index definition
 */
#define INDEX_NULL		(~0U)		/* last in empty block list */
#define INDEX_ALLOC		(~1U)		/* already allocated block */

/* 
 *  memory pool initialization
 */
#ifdef TOPPERS_mpfini

void
initialize_mempfix(void)
{
	uint_t	i;
	MPFCB	*p_mpfcb;

	for (p_mpfcb = mpfcb_table, i = 0; i < tnum_mpf; p_mpfcb++, i++) {
		queue_initialize(&(p_mpfcb->wait_queue));
		p_mpfcb->p_mpfinib = &(mpfinib_table[i]);
		p_mpfcb->fblkcnt = p_mpfcb->p_mpfinib->blkcnt;
		p_mpfcb->unused = 0U;
		p_mpfcb->freelist = INDEX_NULL;
	}
}

#endif /* TOPPERS_mpfini */

/*
 *  get memory block from memory pool
 */
#ifdef TOPPERS_mpfget

void
get_mpf_block(MPFCB *p_mpfcb, void **p_blk)
{
	uint_t	blkidx;

	if (p_mpfcb->freelist != INDEX_NULL) {
		blkidx = p_mpfcb->freelist;
		p_mpfcb->freelist = (p_mpfcb->p_mpfinib->p_mpfmb + blkidx)->next;
	}
	else {
		blkidx = p_mpfcb->unused;
		p_mpfcb->unused++;
	}
	*p_blk = (void *)((char *)(p_mpfcb->p_mpfinib->mpf)
								+ p_mpfcb->p_mpfinib->blksz * blkidx);
	p_mpfcb->fblkcnt--;
	(p_mpfcb->p_mpfinib->p_mpfmb + blkidx)->next = INDEX_ALLOC;
}

#endif /* TOPPERS_mpfget */

/*
 *  get memory block from memory pool through id
 */
#ifdef TOPPERS_get_mpf

ER
get_mpf(ID mpfid, void **p_blk)
{
	MPFCB	*p_mpfcb;
	WINFO_MPF winfo_mpf;
	ER		ercd;

	LOG_GET_MPF_ENTER(mpfid, p_blk);
	CHECK_DISPATCH();
	CHECK_MPFID(mpfid);
	p_mpfcb = get_mpfcb(mpfid);

	t_lock_cpu();
	if (p_mpfcb->fblkcnt > 0) {
		get_mpf_block(p_mpfcb, p_blk);
		ercd = E_OK;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_MPF);
		wobj_make_wait((WOBJCB *) p_mpfcb, (WINFO_WOBJ *) &winfo_mpf);
		dispatch();
		ercd = winfo_mpf.winfo.wercd;
		if (ercd == E_OK) {
			*p_blk = winfo_mpf.blk;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_GET_MPF_LEAVE(ercd, *p_blk);
	return(ercd);
}

#endif /* TOPPERS_get_mpf */

/*
 *  get memory block by polling
 */
#ifdef TOPPERS_pget_mpf

ER
pget_mpf(ID mpfid, void **p_blk)
{
	MPFCB	*p_mpfcb;
	ER		ercd;

	LOG_PGET_MPF_ENTER(mpfid, p_blk);
	CHECK_TSKCTX_UNL();
	CHECK_MPFID(mpfid);
	p_mpfcb = get_mpfcb(mpfid);

	t_lock_cpu();
	if (p_mpfcb->fblkcnt > 0) {
		get_mpf_block(p_mpfcb, p_blk);
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

  error_exit:
	LOG_PGET_MPF_LEAVE(ercd, *p_blk);
	return(ercd);
}

#endif /* TOPPERS_pget_mpf */

/*
 *  get memory block with timeout
 */
#ifdef TOPPERS_tget_mpf

ER
tget_mpf(ID mpfid, void **p_blk, TMO tmout)
{
	MPFCB	*p_mpfcb;
	WINFO_MPF winfo_mpf;
	TMEVTB	tmevtb;
	ER		ercd;

	LOG_TGET_MPF_ENTER(mpfid, p_blk, tmout);
	CHECK_DISPATCH();
	CHECK_MPFID(mpfid);
	CHECK_TMOUT(tmout);
	p_mpfcb = get_mpfcb(mpfid);

	t_lock_cpu();
	if (p_mpfcb->fblkcnt > 0) {
		get_mpf_block(p_mpfcb, p_blk);
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_MPF);
		wobj_make_wait_tmout((WOBJCB *) p_mpfcb, (WINFO_WOBJ *) &winfo_mpf,
														&tmevtb, tmout);
		dispatch();
		ercd = winfo_mpf.winfo.wercd;
		if (ercd == E_OK) {
			*p_blk = winfo_mpf.blk;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_TGET_MPF_LEAVE(ercd, *p_blk);
	return(ercd);
}

#endif /* TOPPERS_tget_mpf */

/*
 *  release memory block
 */
#ifdef TOPPERS_rel_mpf

ER
rel_mpf(ID mpfid, void *blk)
{
	MPFCB	*p_mpfcb;
	SIZE	blkoffset;
	uint_t	blkidx;
	TCB		*p_tcb;
	ER		ercd;
    
	LOG_REL_MPF_ENTER(mpfid, blk);
	CHECK_TSKCTX_UNL();
	CHECK_MPFID(mpfid);
	p_mpfcb = get_mpfcb(mpfid);
	CHECK_PAR(p_mpfcb->p_mpfinib->mpf <= blk);
	blkoffset = ((char *) blk) - (char *)(p_mpfcb->p_mpfinib->mpf);
	CHECK_PAR(blkoffset % p_mpfcb->p_mpfinib->blksz == 0U);
	CHECK_PAR(blkoffset / p_mpfcb->p_mpfinib->blksz < p_mpfcb->unused);
	blkidx = (uint_t)(blkoffset / p_mpfcb->p_mpfinib->blksz);
	CHECK_PAR((p_mpfcb->p_mpfinib->p_mpfmb + blkidx)->next == INDEX_ALLOC);

	t_lock_cpu();
	if (!queue_empty(&(p_mpfcb->wait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_mpfcb->wait_queue));
		((WINFO_MPF *)(p_tcb->p_winfo))->blk = blk;
		if (wait_complete(p_tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		p_mpfcb->fblkcnt++;
		(p_mpfcb->p_mpfinib->p_mpfmb + blkidx)->next = p_mpfcb->freelist;
		p_mpfcb->freelist = blkidx;
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_REL_MPF_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_rel_mpf */

/*
 *  re-init memory pool
 */
#ifdef TOPPERS_ini_mpf

ER
ini_mpf(ID mpfid)
{
	MPFCB	*p_mpfcb;
	bool_t	dspreq;
	ER		ercd;
    
	LOG_INI_MPF_ENTER(mpfid);
	CHECK_TSKCTX_UNL();
	CHECK_MPFID(mpfid);
	p_mpfcb = get_mpfcb(mpfid);

	t_lock_cpu();
	dspreq = init_wait_queue(&(p_mpfcb->wait_queue));
	p_mpfcb->fblkcnt = p_mpfcb->p_mpfinib->blkcnt;
	p_mpfcb->unused = 0U;
	p_mpfcb->freelist = INDEX_NULL;
	if (dspreq) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_INI_MPF_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_mpf */

/*
 *  refer the staus of memory pool
 */
#ifdef TOPPERS_ref_mpf

ER
ref_mpf(ID mpfid, T_RMPF *pk_rmpf)
{
	MPFCB	*p_mpfcb;
	ER		ercd;
    
	LOG_REF_MPF_ENTER(mpfid, pk_rmpf);
	CHECK_TSKCTX_UNL();
	CHECK_MPFID(mpfid);
	p_mpfcb = get_mpfcb(mpfid);

	t_lock_cpu();
	pk_rmpf->wtskid = wait_tskid(&(p_mpfcb->wait_queue));
	pk_rmpf->fblkcnt = p_mpfcb->fblkcnt;
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_REF_MPF_LEAVE(ercd, pk_rmpf);
	return(ercd);
}

#endif /* TOPPERS_ref_mpf */
