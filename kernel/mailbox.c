/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2008 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: mailbox.c 748 2008-03-07 17:18:06Z hiro $
 */

/*
 *	mailbox function
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "mailbox.h"

/*
 *  default definition of trace macro
 */
#ifndef LOG_SND_MBX_ENTER
#define LOG_SND_MBX_ENTER(mbxid, pk_msg)
#endif /* LOG_SND_MBX_ENTER */

#ifndef LOG_SND_MBX_LEAVE
#define LOG_SND_MBX_LEAVE(ercd)
#endif /* LOG_SND_MBX_LEAVE */

#ifndef LOG_RCV_MBX_ENTER
#define LOG_RCV_MBX_ENTER(mbxid, ppk_msg)
#endif /* LOG_RCV_MBX_ENTER */

#ifndef LOG_RCV_MBX_LEAVE
#define LOG_RCV_MBX_LEAVE(ercd, pk_msg)
#endif /* LOG_RCV_MBX_LEAVE */

#ifndef LOG_PRCV_MBX_ENTER
#define LOG_PRCV_MBX_ENTER(mbxid, ppk_msg)
#endif /* LOG_PRCV_MBX_ENTER */

#ifndef LOG_PRCV_MBX_LEAVE
#define LOG_PRCV_MBX_LEAVE(ercd, pk_msg)
#endif /* LOG_PRCV_MBX_LEAVE */

#ifndef LOG_TRCV_MBX_ENTER
#define LOG_TRCV_MBX_ENTER(mbxid, ppk_msg, tmout)
#endif /* LOG_TRCV_MBX_ENTER */

#ifndef LOG_TRCV_MBX_LEAVE
#define LOG_TRCV_MBX_LEAVE(ercd, pk_msg)
#endif /* LOG_TRCV_MBX_LEAVE */

#ifndef LOG_INI_MBX_ENTER
#define LOG_INI_MBX_ENTER(mbxid)
#endif /* LOG_INI_MBX_ENTER */

#ifndef LOG_INI_MBX_LEAVE
#define LOG_INI_MBX_LEAVE(ercd)
#endif /* LOG_INI_MBX_LEAVE */

#ifndef LOG_REF_MBX_ENTER
#define LOG_REF_MBX_ENTER(mbxid, pk_rmbx)
#endif /* LOG_REF_MBX_ENTER */

#ifndef LOG_REF_MBX_LEAVE
#define LOG_REF_MBX_LEAVE(ercd, pk_rmbx)
#endif /* LOG_REF_MBX_LEAVE */

/*
 *  total number of mailboxs
 */
#define tnum_mbx	((uint_t)(tmax_mbxid - TMIN_MBXID + 1))

/*
 *   get mailbox control block for mailbox ID
 */
#define INDEX_MBX(mbxid)	((uint_t)((mbxid) - TMIN_MBXID))
#define get_mbxcb(mbxid)	(&(mbxcb_table[INDEX_MBX(mbxid)]))

/* 
 *  mailbox initialization
 */
#ifdef TOPPERS_mbxini

void
initialize_mailbox(void)
{
	uint_t	i;
	MBXCB	*p_mbxcb;

	for (p_mbxcb = mbxcb_table, i = 0; i < tnum_mbx; p_mbxcb++, i++) {
		queue_initialize(&(p_mbxcb->wait_queue));
		p_mbxcb->p_mbxinib = &(mbxinib_table[i]);
		p_mbxcb->pk_head = NULL;
	}
}

#endif /* TOPPERS_mbxini */

/*
 *  get message priority
 */
#define	MSGPRI(pk_msg)	(((T_MSG_PRI *) pk_msg)->msgpri)

/*
 *  enqueue the message into priority meassage queue
 */
Inline void
enqueue_msg_pri(T_MSG **ppk_prevmsg_next, T_MSG *pk_msg)
{
	T_MSG	*pk_nextmsg;

	while ((pk_nextmsg = *ppk_prevmsg_next) != NULL) {
		if (MSGPRI(pk_nextmsg) > MSGPRI(pk_msg)) {
			break;
		}
		ppk_prevmsg_next = &(pk_nextmsg->pk_next);
	}
	pk_msg->pk_next = pk_nextmsg;
	*ppk_prevmsg_next = pk_msg;
}

/*
 *  send message into mailbox
 */
#ifdef TOPPERS_snd_mbx

ER
snd_mbx(ID mbxid, T_MSG *pk_msg)
{
	MBXCB	*p_mbxcb;
	TCB		*p_tcb;
	ER		ercd;
    
	LOG_SND_MBX_ENTER(mbxid, pk_msg);
	CHECK_TSKCTX_UNL();
	CHECK_MBXID(mbxid);
	p_mbxcb = get_mbxcb(mbxid);
	CHECK_PAR((p_mbxcb->p_mbxinib->mbxatr & TA_MPRI) == 0U
				|| (TMIN_MPRI <= MSGPRI(pk_msg)
					&& MSGPRI(pk_msg) <= p_mbxcb->p_mbxinib->maxmpri));

	t_lock_cpu();
	if (!queue_empty(&(p_mbxcb->wait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_mbxcb->wait_queue));
		((WINFO_MBX *)(p_tcb->p_winfo))->pk_msg = pk_msg;
		if (wait_complete(p_tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if ((p_mbxcb->p_mbxinib->mbxatr & TA_MPRI) != 0U) {
		enqueue_msg_pri(&(p_mbxcb->pk_head), pk_msg);
		ercd = E_OK;
	}
	else {
		pk_msg->pk_next = NULL;
		if (p_mbxcb->pk_head != NULL) {
			p_mbxcb->pk_last->pk_next = pk_msg;
		}
		else {
			p_mbxcb->pk_head = pk_msg;
		}
		p_mbxcb->pk_last = pk_msg;
		ercd = E_OK;
	}
	t_unlock_cpu();

  error_exit:
	LOG_SND_MBX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_snd_mbx */

/*
 *  receive message from mailbox
 */
#ifdef TOPPERS_rcv_mbx

ER
rcv_mbx(ID mbxid, T_MSG **ppk_msg)
{
	MBXCB	*p_mbxcb;
	WINFO_MBX winfo_mbx;
	ER		ercd;
    
	LOG_RCV_MBX_ENTER(mbxid, ppk_msg);
	CHECK_DISPATCH();
	CHECK_MBXID(mbxid);
	p_mbxcb = get_mbxcb(mbxid);
    
	t_lock_cpu();
	if (p_mbxcb->pk_head != NULL) {
		*ppk_msg = p_mbxcb->pk_head;
		p_mbxcb->pk_head = (*ppk_msg)->pk_next;
		ercd = E_OK;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_MBX);
		wobj_make_wait((WOBJCB *) p_mbxcb, (WINFO_WOBJ *) &winfo_mbx);
		dispatch();
		ercd = winfo_mbx.winfo.wercd;
		if (ercd == E_OK) {
			*ppk_msg = winfo_mbx.pk_msg;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_RCV_MBX_LEAVE(ercd, *ppk_msg);
	return(ercd);
}

#endif /* TOPPERS_rcv_mbx */

/*
 *  receive message from mailbox(polling)
 */
#ifdef TOPPERS_prcv_mbx

ER
prcv_mbx(ID mbxid, T_MSG **ppk_msg)
{
	MBXCB	*p_mbxcb;
	ER		ercd;
    
	LOG_PRCV_MBX_ENTER(mbxid, ppk_msg);
	CHECK_TSKCTX_UNL();
	CHECK_MBXID(mbxid);
	p_mbxcb = get_mbxcb(mbxid);
    
	t_lock_cpu();
	if (p_mbxcb->pk_head != NULL) {
		*ppk_msg = p_mbxcb->pk_head;
		p_mbxcb->pk_head = (*ppk_msg)->pk_next;
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

  error_exit:
	LOG_PRCV_MBX_LEAVE(ercd, *ppk_msg);
	return(ercd);
}

#endif /* TOPPERS_prcv_mbx */

/*
 *  receive message from mailbox(with timeout)
 */
#ifdef TOPPERS_trcv_mbx

ER
trcv_mbx(ID mbxid, T_MSG **ppk_msg, TMO tmout)
{
	MBXCB	*p_mbxcb;
	WINFO_MBX winfo_mbx;
	TMEVTB	tmevtb;
	ER		ercd;
    
	LOG_TRCV_MBX_ENTER(mbxid, ppk_msg, tmout);
	CHECK_DISPATCH();
	CHECK_MBXID(mbxid);
	CHECK_TMOUT(tmout);
	p_mbxcb = get_mbxcb(mbxid);
    
	t_lock_cpu();
	if (p_mbxcb->pk_head != NULL) {
		*ppk_msg = p_mbxcb->pk_head;
		p_mbxcb->pk_head = (*ppk_msg)->pk_next;
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_MBX);
		wobj_make_wait_tmout((WOBJCB *) p_mbxcb, (WINFO_WOBJ *) &winfo_mbx,
														&tmevtb, tmout);
		dispatch();
		ercd = winfo_mbx.winfo.wercd;
		if (ercd == E_OK) {
			*ppk_msg = winfo_mbx.pk_msg;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_TRCV_MBX_LEAVE(ercd, *ppk_msg);
	return(ercd);
}

#endif /* TOPPERS_trcv_mbx */

/*
 *  re-init mailbox
 */
#ifdef TOPPERS_ini_mbx

ER
ini_mbx(ID mbxid)
{
	MBXCB	*p_mbxcb;
	bool_t	dspreq;
	ER		ercd;
    
	LOG_INI_MBX_ENTER(mbxid);
	CHECK_TSKCTX_UNL();
	CHECK_MBXID(mbxid);
	p_mbxcb = get_mbxcb(mbxid);

	t_lock_cpu();
	dspreq = init_wait_queue(&(p_mbxcb->wait_queue));
	p_mbxcb->pk_head = NULL;
	if (dspreq) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_INI_MBX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_mbx */

/*
 *  refer the status of mailbox
 */
#ifdef TOPPERS_ref_mbx

ER
ref_mbx(ID mbxid, T_RMBX *pk_rmbx)
{
	MBXCB	*p_mbxcb;
	ER		ercd;
    
	LOG_REF_MBX_ENTER(mbxid, pk_rmbx);
	CHECK_TSKCTX_UNL();
	CHECK_MBXID(mbxid);
	p_mbxcb = get_mbxcb(mbxid);

	t_lock_cpu();
	pk_rmbx->wtskid = wait_tskid(&(p_mbxcb->wait_queue));
	pk_rmbx->pk_msg = p_mbxcb->pk_head;
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_REF_MBX_LEAVE(ercd, pk_rmbx);
	return(ercd);
}

#endif /* TOPPERS_ref_mbx */
