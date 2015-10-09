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
 *  @(#) $Id: interrupt.c 2018 2010-12-31 13:43:05Z ertl-hiro $
 */

/*
 *		interrupt management function
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "interrupt.h"

/*
 *  default definition of trace macro
 */
#ifndef LOG_ISR_ENTER
#define LOG_ISR_ENTER(intno)
#endif /* LOG_ISR_ENTER */

#ifndef LOG_ISR_LEAVE
#define LOG_ISR_LEAVE(intno)
#endif /* LOG_ISR_LEAVE */

#ifndef LOG_ACRE_ISR_ENTER
#define LOG_ACRE_ISR_ENTER(pk_cisr)
#endif /* LOG_ACRE_ISR_ENTER */

#ifndef LOG_ACRE_ISR_LEAVE
#define LOG_ACRE_ISR_LEAVE(ercd)
#endif /* LOG_ACRE_ISR_LEAVE */

#ifndef LOG_DEL_ISR_ENTER
#define LOG_DEL_ISR_ENTER(isrid)
#endif /* LOG_DEL_ISR_ENTER */

#ifndef LOG_DEL_ISR_LEAVE
#define LOG_DEL_ISR_LEAVE(ercd)
#endif /* LOG_DEL_ISR_LEAVE */

#ifndef LOG_DIS_INT_ENTER
#define LOG_DIS_INT_ENTER(intno)
#endif /* LOG_DIS_INT_ENTER */

#ifndef LOG_DIS_INT_LEAVE
#define LOG_DIS_INT_LEAVE(ercd)
#endif /* LOG_DIS_INT_LEAVE */

#ifndef LOG_ENA_INT_ENTER
#define LOG_ENA_INT_ENTER(intno)
#endif /* LOG_ENA_INT_ENTER */

#ifndef LOG_ENA_INT_LEAVE
#define LOG_ENA_INT_LEAVE(ercd)
#endif /* LOG_ENA_INT_LEAVE */

#ifndef LOG_CHG_IPM_ENTER
#define LOG_CHG_IPM_ENTER(intpri)
#endif /* LOG_CHG_IPM_ENTER */

#ifndef LOG_CHG_IPM_LEAVE
#define LOG_CHG_IPM_LEAVE(ercd)
#endif /* LOG_CHG_IPM_LEAVE */

#ifndef LOG_GET_IPM_ENTER
#define LOG_GET_IPM_ENTER(p_intpri)
#endif /* LOG_GET_IPM_ENTER */

#ifndef LOG_GET_IPM_LEAVE
#define LOG_GET_IPM_LEAVE(ercd, intpri)
#endif /* LOG_GET_IPM_LEAVE */

/*
 *  number of interrupt service routine
 */
#define tnum_isr	((uint_t)(tmax_isrid - TMIN_SEMID + 1) + tnum_sisr)

/*
 *  macro to get interrupt ID of interrupt service routine
 */
#define INDEX_ISR(isrid)	((uint_t)((isrid) - TMIN_ISRID) + tnum_sisr)
#define get_isrcb(isrid)	(&(isrcb_table[INDEX_ISR(isrid)]))

/*
 *  enqueue the interrupt service routine
 */
Inline void
enqueue_isr(QUEUE *p_isr_queue, ISRCB *p_isrcb)
{
	QUEUE	*p_entry;
	PRI		isrpri = p_isrcb->p_isrinib->isrpri;

	for (p_entry = p_isr_queue->p_next; p_entry != p_isr_queue;
											p_entry = p_entry->p_next) {
		if (isrpri < ((ISRCB *) p_entry)->p_isrinib->isrpri) {
			break;
		}
	}
	queue_insert_prev(p_entry, &(p_isrcb->isr_queue));
}

#ifdef TOPPERS_isrini

/*
 *  list of free interrupt service routine control blocks
 */
QUEUE	free_isrcb;

/*
 *  initial the interrupt service routine
 */
void
initialize_isr(void)
{
	uint_t	i, j;
	ISRCB	*p_isrcb;
	ISRINIB	*p_isrinib;

	for (i = 0; i < tnum_isr_queue; i++) {
		queue_initialize(&(isr_queue_table[i]));
	}
	for (p_isrcb = isrcb_table, i = 0; i < tnum_sisr; p_isrcb++, i++) {
		p_isrcb->p_isrinib = &(sisrinib_table[i]);
		enqueue_isr(p_isrcb->p_isrinib->p_isr_queue, p_isrcb);
	}
	queue_initialize(&free_isrcb);
	for (j = 0; i < tnum_isr; p_isrcb++, i++, j++) {
		p_isrinib = &(aisrinib_table[j]);
		p_isrinib->isratr = TA_NOEXS;
		p_isrcb->p_isrinib = ((const ISRINIB *) p_isrinib);
		queue_insert_prev(&free_isrcb, &(p_isrcb->isr_queue));
	}
}

#endif /* TOPPERS_isrini */

/*
 *  call the interrupt service routine
 */
#ifdef TOPPERS_isrcal

void
call_isr(QUEUE *p_isr_queue)
{
	QUEUE	*p_queue;
	ISRINIB	*p_isrinib;
	PRI		saved_ipm;

	saved_ipm = i_get_ipm();
	for (p_queue = p_isr_queue->p_next; p_queue != p_isr_queue;
											p_queue = p_queue->p_next) {
		p_isrinib = (ISRINIB *)(((ISRCB *) p_queue)->p_isrinib);
		LOG_ISR_ENTER(p_isrinib->intno);
		(*(p_isrinib->isr))(p_isrinib->exinf);
		LOG_ISR_LEAVE(p_isrinib->intno);

		if (p_queue->p_next != p_isr_queue) {
			/* return to the state before call of the ISR */
			if (i_sense_lock()) {
				i_unlock_cpu();
			}
			i_set_ipm(saved_ipm);
		}
	}
}

#endif /* TOPPERS_isrcal */

/*
 *  search the queue of the interrupt service routine
 */
Inline QUEUE *
search_isr_queue(INTNO intno)
{
	int_t	left, right, i;

	if (tnum_isr_queue == 0) {
		return(NULL);
	}

	left = 0;
	right = tnum_isr_queue - 1;
	while (left < right) {
		i = (left + right + 1) / 2;
		if (intno < isr_queue_list[i].intno) {
			right = i - 1;
		}
		else {
			left = i;
		}
	}
	if (isr_queue_list[left].intno == intno) {
		return(isr_queue_list[left].p_isr_queue);
	}
	else {
		return(NULL);
	}
}

/*
 * create interrupt service routine
 */
#ifdef TOPPERS_acre_isr

ER_UINT
acre_isr(const T_CISR *pk_cisr)
{
	ISRCB	*p_isrcb;
	ISRINIB	*p_isrinib;
	QUEUE	*p_isr_queue;
	ER		ercd;

	LOG_ACRE_ISR_ENTER(pk_cisr);
	CHECK_TSKCTX_UNL();
	CHECK_RSATR(pk_cisr->isratr, TARGET_ISRATR);
	CHECK_INTNO_CREISR(pk_cisr->intno);
	CHECK_ALIGN_FUNC(pk_cisr->isr);
	CHECK_NONNULL_FUNC(pk_cisr->isr);
	CHECK_ISRPRI(pk_cisr->isrpri);

	p_isr_queue = search_isr_queue(pk_cisr->intno);
	CHECK_PAR(p_isr_queue != NULL);

	t_lock_cpu();
	if (queue_empty(&free_isrcb)) {
		ercd = E_NOID;
	}
	else {
		p_isrcb = ((ISRCB *) queue_delete_next(&free_isrcb));
		p_isrinib = (ISRINIB *)(p_isrcb->p_isrinib);
		p_isrinib->isratr = pk_cisr->isratr;
		p_isrinib->exinf = pk_cisr->exinf;
		p_isrinib->intno = pk_cisr->intno;
		p_isrinib->p_isr_queue = p_isr_queue;
		p_isrinib->isr = pk_cisr->isr;
		p_isrinib->isrpri = pk_cisr->isrpri;
		enqueue_isr(p_isr_queue, p_isrcb);
		ercd = ISRID(p_isrcb);
	}
	t_unlock_cpu();

  error_exit:
	LOG_ACRE_ISR_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_acre_isr */

/*
 * delete interrupt service routine
 */
#ifdef TOPPERS_del_isr

ER
del_isr(ID isrid)
{
	ISRCB	*p_isrcb;
	ISRINIB	*p_isrinib;
	ER		ercd;

	LOG_DEL_ISR_ENTER(isrid);
	CHECK_TSKCTX_UNL();
	CHECK_ISRID(isrid);
	p_isrcb = get_isrcb(isrid);

	t_lock_cpu();
	if (p_isrcb->p_isrinib->isratr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else {
		queue_delete(&(p_isrcb->isr_queue));
		p_isrinib = (ISRINIB *)(p_isrcb->p_isrinib);
		p_isrinib->isratr = TA_NOEXS;
		queue_insert_prev(&free_isrcb, &(p_isrcb->isr_queue));
	}
	t_unlock_cpu();

  error_exit:
	LOG_DEL_ISR_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_del_isr */

/*
 *  interrupt management initialization
 */
#ifdef TOPPERS_intini
#ifndef OMIT_INITIALIZE_INTERRUPT

void
initialize_interrupt(void)
{
	uint_t			i;
	const INHINIB	*p_inhinib;
	const INTINIB	*p_intinib;

	for (p_inhinib = inhinib_table, i = 0; i < tnum_inhno; p_inhinib++, i++) {
		x_define_inh(p_inhinib->inhno, p_inhinib->int_entry);
	}
	for (p_intinib = intinib_table, i = 0; i < tnum_intno; p_intinib++, i++) {
		x_config_int(p_intinib->intno, p_intinib->intatr, p_intinib->intpri);
	}
}

#endif /* OMIT_INITIALIZE_INTERRUPT */
#endif /* TOPPERS_intini */

/*
 * disable interrupt
 */
#ifdef TOPPERS_dis_int
#ifdef TOPPERS_SUPPORT_DIS_INT

ER
dis_int(INTNO intno)
{
	bool_t	locked;
	ER		ercd;

	LOG_DIS_INT_ENTER(intno);
	CHECK_TSKCTX();
	CHECK_INTNO_DISINT(intno);

	locked = t_sense_lock();
	if (!locked) {
		t_lock_cpu();
	}
	if (t_disable_int(intno)) {
		ercd = E_OK;
	}
	else {
		ercd = E_OBJ;
	}
	if (!locked) {
		t_unlock_cpu();
	}

  error_exit:
	LOG_DIS_INT_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_SUPPORT_DIS_INT */
#endif /* TOPPERS_dis_int */

/*
 *  enable interrupt
 */
#ifdef TOPPERS_ena_int
#ifdef TOPPERS_SUPPORT_ENA_INT

ER
ena_int(INTNO intno)
{
	bool_t	locked;
	ER		ercd;

	LOG_ENA_INT_ENTER(intno);
	CHECK_TSKCTX();
	CHECK_INTNO_DISINT(intno);

	locked = t_sense_lock();
	if (!locked) {
		t_lock_cpu();
	}
	if (t_enable_int(intno)) {
		ercd = E_OK;
	}
	else {
		ercd = E_OBJ;
	}
	if (!locked) {
		t_unlock_cpu();
	}

  error_exit:
	LOG_ENA_INT_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_SUPPORT_ENA_INT */
#endif /* TOPPERS_ena_int */

/*
 *  change interrupt priority mask
 */
#ifdef TOPPERS_chg_ipm

ER
chg_ipm(PRI intpri)
{
	ER		ercd;

	LOG_CHG_IPM_ENTER(intpri);
	CHECK_TSKCTX_UNL();
	CHECK_INTPRI_CHGIPM(intpri);

	t_lock_cpu();
	t_set_ipm(intpri);
	if (intpri == TIPM_ENAALL) {
		ipmflg = true;
		if (!disdsp) {
			dspflg = true;
			if (p_runtsk != p_schedtsk) {
				dispatch();
			}
		}
		if (p_runtsk->enatex && p_runtsk->texptn != 0U) {
			call_texrtn();
		}
	}
	else {
		ipmflg = false;
		dspflg = false;
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_CHG_IPM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_chg_ipm */

/*
 *  refer the interrupt priority mask
 */
#ifdef TOPPERS_get_ipm

ER
get_ipm(PRI *p_intpri)
{
	ER		ercd;

	LOG_GET_IPM_ENTER(p_intpri);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	*p_intpri = t_get_ipm();
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_GET_IPM_LEAVE(ercd, *p_intpri);
	return(ercd);
}

#endif /* TOPPERS_get_ipm */
