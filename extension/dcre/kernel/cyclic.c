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
 *  @(#) $Id: cyclic.c 1966 2010-11-20 07:23:56Z ertl-hiro $
 */

/*
 *		cyclic handler function
 */

#include "kernel_impl.h"
#include "check.h"
#include "cyclic.h"

/*
 *  default definition of trace macro
 */
#ifndef LOG_CYC_ENTER
#define LOG_CYC_ENTER(p_cyccb)
#endif /* LOG_CYC_ENTER */

#ifndef LOG_CYC_LEAVE
#define LOG_CYC_LEAVE(p_cyccb)
#endif /* LOG_CYC_LEAVE */

#ifndef LOG_ACRE_CYC_ENTER
#define LOG_ACRE_CYC_ENTER(pk_ccyc)
#endif /* LOG_ACRE_CYC_ENTER */

#ifndef LOG_ACRE_CYC_LEAVE
#define LOG_ACRE_CYC_LEAVE(ercd)
#endif /* LOG_ACRE_CYC_LEAVE */

#ifndef LOG_DEL_CYC_ENTER
#define LOG_DEL_CYC_ENTER(cycid)
#endif /* LOG_DEL_CYC_ENTER */

#ifndef LOG_DEL_CYC_LEAVE
#define LOG_DEL_CYC_LEAVE(ercd)
#endif /* LOG_DEL_CYC_LEAVE */

#ifndef LOG_STA_CYC_ENTER
#define LOG_STA_CYC_ENTER(cycid)
#endif /* LOG_STA_CYC_ENTER */

#ifndef LOG_STA_CYC_LEAVE
#define LOG_STA_CYC_LEAVE(ercd)
#endif /* LOG_STA_CYC_LEAVE */

#ifndef LOG_STP_CYC_ENTER
#define LOG_STP_CYC_ENTER(cycid)
#endif /* LOG_STP_CYC_ENTER */

#ifndef LOG_STP_CYC_LEAVE
#define LOG_STP_CYC_LEAVE(ercd)
#endif /* LOG_STP_CYC_LEAVE */

#ifndef LOG_REF_CYC_ENTER
#define LOG_REF_CYC_ENTER(cycid, pk_rcyc)
#endif /* LOG_REF_CYC_ENTER */

#ifndef LOG_REF_CYC_LEAVE
#define LOG_REF_CYC_LEAVE(ercd, pk_rcyc)
#endif /* LOG_REF_CYC_LEAVE */

/*
 *  total number of cyclic handlers
 */
#define tnum_cyc	((uint_t)(tmax_cycid - TMIN_CYCID + 1))
#define tnum_scyc	((uint_t)(tmax_scycid - TMIN_CYCID + 1))

/*
 *  get cyclic handler control block for cyclic handler ID
 */
#define INDEX_CYC(cycid)	((uint_t)((cycid) - TMIN_CYCID))
#define get_cyccb(cycid)	(&(cyccb_table[INDEX_CYC(cycid)]))

/*
 *   register a time event for cyclic handler
 */
Inline void
tmevtb_enqueue_cyc(CYCCB *p_cyccb, EVTTIM evttim)
{
	tmevtb_enqueue_evttim(&(p_cyccb->tmevtb), evttim,
								(CBACK) call_cychdr, (void *) p_cyccb);
	p_cyccb->evttim = evttim;
}

#ifdef TOPPERS_cycini

/*
 *  list of cyclic control block which is not used
 *
 *  there is no place for queue entry in the cyclic handler control
 *  block, the time event block (tmevtb) is used as queue entry.
 */
QUEUE	free_cyccb;

/*
 *  cyclic handler initialization
 */
void
initialize_cyclic(void)
{
	uint_t	i, j;
	CYCCB	*p_cyccb;
	CYCINIB	*p_cycinib;

	for (p_cyccb = cyccb_table, i = 0; i < tnum_scyc; p_cyccb++, i++) {
		p_cyccb->p_cycinib = &(cycinib_table[i]);
		if ((p_cyccb->p_cycinib->cycatr & TA_STA) != 0U) {
			p_cyccb->cycsta = true;
			tmevtb_enqueue_cyc(p_cyccb, (EVTTIM)(p_cyccb->p_cycinib->cycphs));
		}
		else {
			p_cyccb->cycsta = false;
		}
	}
	queue_initialize(&free_cyccb);
	for (j = 0; i < tnum_cyc; p_cyccb++, i++, j++) {
		p_cycinib = &(acycinib_table[j]);
		p_cycinib->cycatr = TA_NOEXS;
		p_cyccb->p_cycinib = ((const CYCINIB *) p_cycinib);
		queue_insert_prev(&free_cyccb, ((QUEUE *) &(p_cyccb->tmevtb)));
	}
}

#endif /* TOPPERS_cycini */

/*
 *  create cyclic handler
 */
#ifdef TOPPERS_acre_cyc

ER_UINT
acre_cyc(const T_CCYC *pk_ccyc)
{
	CYCCB	*p_cyccb;
	CYCINIB	*p_cycinib;
	ER		ercd;

	LOG_ACRE_CYC_ENTER(pk_ccyc);
	CHECK_TSKCTX_UNL();
	CHECK_RSATR(pk_ccyc->cycatr, TA_STA);
	CHECK_ALIGN_FUNC(pk_ccyc->cychdr);
	CHECK_NONNULL_FUNC(pk_ccyc->cychdr);
	CHECK_PAR(0 < pk_ccyc->cyctim && pk_ccyc->cyctim <= TMAX_RELTIM);
	CHECK_PAR(0 <= pk_ccyc->cycphs && pk_ccyc->cycphs <= TMAX_RELTIM);

	t_lock_cpu();
	if (queue_empty(&free_cyccb)) {
		ercd = E_NOID;
	}
	else {
		p_cyccb = ((CYCCB *)(((char *) queue_delete_next(&free_cyccb))
												- offsetof(CYCCB, tmevtb)));
		p_cycinib = (CYCINIB *)(p_cyccb->p_cycinib);
		p_cycinib->cycatr = pk_ccyc->cycatr;
		p_cycinib->exinf = pk_ccyc->exinf;
		p_cycinib->cychdr = pk_ccyc->cychdr;
		p_cycinib->cyctim = pk_ccyc->cyctim;
		p_cycinib->cycphs = pk_ccyc->cycphs;

		if ((p_cyccb->p_cycinib->cycatr & TA_STA) != 0U) {
			p_cyccb->cycsta = true;
			tmevtb_enqueue_cyc(p_cyccb, (EVTTIM)(p_cyccb->p_cycinib->cycphs));
		}
		else {
			p_cyccb->cycsta = false;
		}
		ercd = CYCID(p_cyccb);
	}
	t_unlock_cpu();

  error_exit:
	LOG_ACRE_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_acre_cyc */

/*
 *  delete cyclic handler
 */
#ifdef TOPPERS_del_cyc

ER
del_cyc(ID cycid)
{
	CYCCB	*p_cyccb;
	CYCINIB	*p_cycinib;
	ER		ercd;

	LOG_DEL_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_CYCID(cycid);
	p_cyccb = get_cyccb(cycid);

	t_lock_cpu();
	if (p_cyccb->p_cycinib->cycatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (CYCID(p_cyccb) > tmax_scycid) {
		if (p_cyccb->cycsta) {
			p_cyccb->cycsta = false;
			tmevtb_dequeue(&(p_cyccb->tmevtb));
		}

		p_cycinib = (CYCINIB *)(p_cyccb->p_cycinib);
		p_cycinib->cycatr = TA_NOEXS;
		queue_insert_prev(&free_cyccb, ((QUEUE *) &(p_cyccb->tmevtb)));
		ercd = E_OK;
	}
	else {
		ercd = E_OBJ;
	}
	t_unlock_cpu();

  error_exit:
	LOG_DEL_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_del_cyc */

/*
 *  start a cyclic handler
 */
#ifdef TOPPERS_sta_cyc

ER
sta_cyc(ID cycid)
{
	CYCCB	*p_cyccb;
	ER		ercd;

	LOG_STA_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_CYCID(cycid);
	p_cyccb = get_cyccb(cycid);

	t_lock_cpu();
	if (p_cyccb->p_cycinib->cycatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_cyccb->cycsta) {
		tmevtb_dequeue(&(p_cyccb->tmevtb));
	}
	else {
		p_cyccb->cycsta = true;
	}
	tmevtb_enqueue_cyc(p_cyccb, base_time + p_cyccb->p_cycinib->cycphs);
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_STA_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sta_cyc */

/*
 *  stop a cyclic handler
 */
#ifdef TOPPERS_stp_cyc

ER
stp_cyc(ID cycid)
{
	CYCCB	*p_cyccb;
	ER		ercd;

	LOG_STP_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_CYCID(cycid);
	p_cyccb = get_cyccb(cycid);

	t_lock_cpu();
	if (p_cyccb->p_cycinib->cycatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_cyccb->cycsta) {
		p_cyccb->cycsta = false;
		tmevtb_dequeue(&(p_cyccb->tmevtb));
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_STP_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_stp_cyc */

/*
 *  refer the status of cyclic handler
 */
#ifdef TOPPERS_ref_cyc

ER
ref_cyc(ID cycid, T_RCYC *pk_rcyc)
{
	CYCCB	*p_cyccb;
	ER		ercd;

	LOG_REF_CYC_ENTER(cycid, pk_rcyc);
	CHECK_TSKCTX_UNL();
	CHECK_CYCID(cycid);
	p_cyccb = get_cyccb(cycid);

	t_lock_cpu();
	if (p_cyccb->p_cycinib->cycatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else {
		if (p_cyccb->cycsta) {
			pk_rcyc->cycstat = TCYC_STA;
			pk_rcyc->lefttim = tmevt_lefttim(&(p_cyccb->tmevtb));
		}
		else {
			pk_rcyc->cycstat = TCYC_STP;
		}
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_REF_CYC_LEAVE(ercd, pk_rcyc);
	return(ercd);
}

#endif /* TOPPERS_ref_cyc */

/*
 *  cyclic handler callback routine
 *  this function is called  in signal_time(time_evnt.c).
 *  in ASP kernel, the cyclic handlers are processed in the interrupt handler
 *  of system time tick.
 */
#ifdef TOPPERS_cyccal

void
call_cychdr(CYCCB *p_cyccb)
{
	PRI		saved_ipm;

	/*
	 *  register time event for next run of cyclic handler
	 *
	 *  this routine just calls the cyclic handler specified in p_cyccb one time.
	 *
	 */
	tmevtb_enqueue_cyc(p_cyccb, p_cyccb->evttim + p_cyccb->p_cycinib->cyctim);

	/*
	 *  cyclic handler should be called in cpu unlock status
	 */
	saved_ipm = i_get_ipm();
	i_unlock_cpu();

	LOG_CYC_ENTER(p_cyccb);
	(*((CYCHDR)(p_cyccb->p_cycinib->cychdr)))(p_cyccb->p_cycinib->exinf);
	LOG_CYC_LEAVE(p_cyccb);

	if (!i_sense_lock()) {
		i_lock_cpu();
	}
	i_set_ipm(saved_ipm);
}

#endif /* TOPPERS_cyccal */
