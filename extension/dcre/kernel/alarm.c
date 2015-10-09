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
 *  @(#) $Id: alarm.c 1966 2010-11-20 07:23:56Z ertl-hiro $
 */

/*
 *		alarm handler function
 */

#include "kernel_impl.h"
#include "check.h"
#include "alarm.h"

/*
 *  default definition of trace macro
 */
#ifndef LOG_ALM_ENTER
#define LOG_ALM_ENTER(p_almcb)
#endif /* LOG_ALM_ENTER */

#ifndef LOG_ALM_LEAVE
#define LOG_ALM_LEAVE(p_almcb)
#endif /* LOG_ALM_LEAVE */

#ifndef LOG_ACRE_ALM_ENTER
#define LOG_ACRE_ALM_ENTER(pk_calm)
#endif /* LOG_ACRE_ALM_ENTER */

#ifndef LOG_ACRE_ALM_LEAVE
#define LOG_ACRE_ALM_LEAVE(ercd)
#endif /* LOG_ACRE_ALM_LEAVE */

#ifndef LOG_DEL_ALM_ENTER
#define LOG_DEL_ALM_ENTER(almid)
#endif /* LOG_DEL_ALM_ENTER */

#ifndef LOG_DEL_ALM_LEAVE
#define LOG_DEL_ALM_LEAVE(ercd)
#endif /* LOG_DEL_ALM_LEAVE */

#ifndef LOG_STA_ALM_ENTER
#define LOG_STA_ALM_ENTER(almid, almtim)
#endif /* LOG_STA_ALM_ENTER */

#ifndef LOG_STA_ALM_LEAVE
#define LOG_STA_ALM_LEAVE(ercd)
#endif /* LOG_STA_ALM_LEAVE */

#ifndef LOG_ISTA_ALM_ENTER
#define LOG_ISTA_ALM_ENTER(almid, almtim)
#endif /* LOG_ISTA_ALM_ENTER */

#ifndef LOG_ISTA_ALM_LEAVE
#define LOG_ISTA_ALM_LEAVE(ercd)
#endif /* LOG_ISTA_ALM_LEAVE */

#ifndef LOG_STP_ALM_ENTER
#define LOG_STP_ALM_ENTER(almid)
#endif /* LOG_STP_ALM_ENTER */

#ifndef LOG_STP_ALM_LEAVE
#define LOG_STP_ALM_LEAVE(ercd)
#endif /* LOG_STP_ALM_LEAVE */

#ifndef LOG_ISTP_ALM_ENTER
#define LOG_ISTP_ALM_ENTER(almid)
#endif /* LOG_ISTP_ALM_ENTER */

#ifndef LOG_ISTP_ALM_LEAVE
#define LOG_ISTP_ALM_LEAVE(ercd)
#endif /* LOG_ISTP_ALM_LEAVE */

#ifndef LOG_REF_ALM_ENTER
#define LOG_REF_ALM_ENTER(almid, pk_ralm)
#endif /* LOG_REF_ALM_ENTER */

#ifndef LOG_REF_ALM_LEAVE
#define LOG_REF_ALM_LEAVE(ercd, pk_ralm)
#endif /* LOG_REF_ALM_LEAVE */

/*
 *  total number of alarm handlers
 */
#define tnum_alm	((uint_t)(tmax_almid - TMIN_ALMID + 1))
#define tnum_salm	((uint_t)(tmax_salmid - TMIN_ALMID + 1))

/*
 *  get alarm handler control block for alarm handler ID
 */
#define INDEX_ALM(almid)	((uint_t)((almid) - TMIN_ALMID))
#define get_almcb(almid)	(&(almcb_table[INDEX_ALM(almid)]))

#ifdef TOPPERS_almini

/*
 *  list of alarm handler control block which is not used
 *
 *  there is no place for queue entry in the alarm handler control
 *  block, the time event block (tmevtb) is used as queue entry.
 */
QUEUE	free_almcb;

/*
 *  initialize alarm handler
 */
void
initialize_alarm(void)
{
	uint_t	i, j;
	ALMCB	*p_almcb;
	ALMINIB	*p_alminib;

	for (p_almcb = almcb_table, i = 0; i < tnum_salm; p_almcb++, i++) {
		p_almcb->p_alminib = &(alminib_table[i]);
		p_almcb->almsta = false;
	}
	queue_initialize(&free_almcb);
	for (j = 0; i < tnum_alm; p_almcb++, i++, j++) {
		p_alminib = &(aalminib_table[j]);
		p_alminib->almatr = TA_NOEXS;
		p_almcb->p_alminib = ((const ALMINIB *) p_alminib);
		queue_insert_prev(&free_almcb, ((QUEUE *) &(p_almcb->tmevtb)));
	}
}

#endif /* TOPPERS_almini */

/*
 *  create alarm handler
 */
#ifdef TOPPERS_acre_alm

ER_UINT
acre_alm(const T_CALM *pk_calm)
{
	ALMCB	*p_almcb;
	ALMINIB	*p_alminib;
	ER		ercd;

	LOG_ACRE_ALM_ENTER(pk_calm);
	CHECK_TSKCTX_UNL();
	CHECK_RSATR(pk_calm->almatr, TA_NULL);
	CHECK_ALIGN_FUNC(pk_calm->almhdr);
	CHECK_NONNULL_FUNC(pk_calm->almhdr);

	t_lock_cpu();
	if (queue_empty(&free_almcb)) {
		ercd = E_NOID;
	}
	else {
		p_almcb = ((ALMCB *)(((char *) queue_delete_next(&free_almcb))
												- offsetof(ALMCB, tmevtb)));
		p_alminib = (ALMINIB *)(p_almcb->p_alminib);
		p_alminib->almatr = pk_calm->almatr;
		p_alminib->exinf = pk_calm->exinf;
		p_alminib->almhdr = pk_calm->almhdr;

		p_almcb->almsta = false;
		ercd = ALMID(p_almcb);
	}
	t_unlock_cpu();

  error_exit:
	LOG_ACRE_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_acre_alm */

/*
 *  delete alarm handler
 */
#ifdef TOPPERS_del_alm

ER
del_alm(ID almid)
{
	ALMCB	*p_almcb;
	ALMINIB	*p_alminib;
	ER		ercd;

	LOG_DEL_ALM_ENTER(almid);
	CHECK_TSKCTX_UNL();
	CHECK_ALMID(almid);
	p_almcb = get_almcb(almid);

	t_lock_cpu();
	if (p_almcb->p_alminib->almatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (ALMID(p_almcb) > tmax_salmid) {
		if (p_almcb->almsta) {
			p_almcb->almsta = false;
			tmevtb_dequeue(&(p_almcb->tmevtb));
		}

		p_alminib = (ALMINIB *)(p_almcb->p_alminib);
		p_alminib->almatr = TA_NOEXS;
		queue_insert_prev(&free_almcb, ((QUEUE *) &(p_almcb->tmevtb)));
		ercd = E_OK;
	}
	else {
		ercd = E_OBJ;
	}
	t_unlock_cpu();

  error_exit:
	LOG_DEL_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_del_alm */

/*
 * start an alarm hander
 * the unit of almtim is system tick
 */
#ifdef TOPPERS_sta_alm

ER
sta_alm(ID almid, RELTIM almtim)
{
	ALMCB	*p_almcb;
	ER		ercd;

	LOG_STA_ALM_ENTER(almid, almtim);
	CHECK_TSKCTX_UNL();
	CHECK_ALMID(almid);
	CHECK_PAR(almtim <= TMAX_RELTIM);
	p_almcb = get_almcb(almid);

	t_lock_cpu();
	if (p_almcb->p_alminib->almatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_almcb->almsta) {
		tmevtb_dequeue(&(p_almcb->tmevtb));
	}
	else {
		p_almcb->almsta = true;
	}
	tmevtb_enqueue(&(p_almcb->tmevtb), almtim,
								(CBACK) call_almhdr, (void *) p_almcb);
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_STA_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sta_alm */

/*
 *  start an alarm handler (call in non-task context, such as interrupts, exceptions)
 */
#ifdef TOPPERS_ista_alm

ER
ista_alm(ID almid, RELTIM almtim)
{
	ALMCB	*p_almcb;
	ER		ercd;

	LOG_ISTA_ALM_ENTER(almid, almtim);
	CHECK_INTCTX_UNL();
	CHECK_ALMID(almid);
	CHECK_PAR(almtim <= TMAX_RELTIM);
	p_almcb = get_almcb(almid);

	i_lock_cpu();
	if (p_almcb->p_alminib->almatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_almcb->almsta) {
		tmevtb_dequeue(&(p_almcb->tmevtb));
	}
	else {
		p_almcb->almsta = true;
	}
	tmevtb_enqueue(&(p_almcb->tmevtb), almtim,
								(CBACK) call_almhdr, (void *) p_almcb);
	ercd = E_OK;
	i_unlock_cpu();

  error_exit:
	LOG_ISTA_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ista_alm */

/*
 *  stop an alarm handler
 */
#ifdef TOPPERS_stp_alm

ER
stp_alm(ID almid)
{
	ALMCB	*p_almcb;
	ER		ercd;

	LOG_STP_ALM_ENTER(almid);
	CHECK_TSKCTX_UNL();
	CHECK_ALMID(almid);
	p_almcb = get_almcb(almid);

	t_lock_cpu();
	if (p_almcb->p_alminib->almatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_almcb->almsta) {
		p_almcb->almsta = false;
		tmevtb_dequeue(&(p_almcb->tmevtb));
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_STP_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_stp_alm */

/*
 *  stop an alarm handler (call in non-task context)
 */
#ifdef TOPPERS_istp_alm

ER
istp_alm(ID almid)
{
	ALMCB	*p_almcb;
	ER		ercd;

	LOG_ISTP_ALM_ENTER(almid);
	CHECK_INTCTX_UNL();
	CHECK_ALMID(almid);
	p_almcb = get_almcb(almid);

	i_lock_cpu();
	if (p_almcb->p_alminib->almatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_almcb->almsta) {
		p_almcb->almsta = false;
		tmevtb_dequeue(&(p_almcb->tmevtb));
	}
	ercd = E_OK;
	i_unlock_cpu();

  error_exit:
	LOG_ISTP_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_istp_alm */

/*
 *  refer the status of alarm handler
 */
#ifdef TOPPERS_ref_alm

ER
ref_alm(ID almid, T_RALM *pk_ralm)
{
	ALMCB	*p_almcb;
	ER		ercd;

	LOG_REF_ALM_ENTER(almid, pk_ralm);
	CHECK_TSKCTX_UNL();
	CHECK_ALMID(almid);
	p_almcb = get_almcb(almid);

	t_lock_cpu();
	if (p_almcb->p_alminib->almatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else {
		if (p_almcb->almsta) {
			pk_ralm->almstat = TALM_STA;
			pk_ralm->lefttim = tmevt_lefttim(&(p_almcb->tmevtb));
		}
		else {
			pk_ralm->almstat = TALM_STP;
		}
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_REF_ALM_LEAVE(ercd, pk_ralm);
	return(ercd);
}

#endif /* TOPPERS_ref_alm */

/*
 *  alarm handler callback routine
 *  this function is called  in signal_time(time_event.c)
 *  in ASP kernel, the alarm handlers are processed in the interrupt handler
 *  of system time tick.
 */
#ifdef TOPPERS_almcal

void
call_almhdr(ALMCB *p_almcb)
{
	PRI		saved_ipm;

	/*
	 *  stop the alarm handler
	 */
	p_almcb->almsta = false;

	/*
	 *  alarm handler should be called in cpu unlock status
	 */
	saved_ipm = i_get_ipm();
	i_unlock_cpu();

	LOG_ALM_ENTER(p_almcb);
	(*((ALMHDR)(p_almcb->p_alminib->almhdr)))(p_almcb->p_alminib->exinf);
	LOG_ALM_LEAVE(p_almcb);

	if (!i_sense_lock()) {
		i_lock_cpu();
	}
	i_set_ipm(saved_ipm);
}

#endif /* TOPPERS_almcal */
