/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2008 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: serial.c 1176 2008-07-01 10:24:46Z ertl-hiro $
 */

/*
 *		serial interface driver
 */

#include <kernel.h>
#include <t_syslog.h>
#include "target_syssvc.h"
#include "target_serial.h"
#include "serial.h"
#include "kernel_cfg.h"

/*
 *  default size of buffer and other definitions
 */
#ifndef SERIAL_RCV_BUFSZ1
#define	SERIAL_RCV_BUFSZ1	256			/* receive buffer size of port1 */
#endif /* SERIAL_RCV_BUFSZ1 */

#ifndef SERIAL_SND_BUFSZ1
#define	SERIAL_SND_BUFSZ1	256			/* send buffer size of port1 */
#endif /* SERIAL_SND_BUFSZ1 */

static char_t	rcv_buffer1[SERIAL_RCV_BUFSZ1];
static char_t	snd_buffer1[SERIAL_SND_BUFSZ1];

#if TNUM_PORT >= 2						/* definitions for port2 */

#ifndef SERIAL_RCV_BUFSZ2
#define	SERIAL_RCV_BUFSZ2	256			/* receive buffer size of port2 */
#endif /* SERIAL_RCV_BUFSZ2 */

#ifndef SERIAL_SND_BUFSZ2
#define	SERIAL_SND_BUFSZ2	256			/* send buffer size of port2 */
#endif /* SERIAL_SND_BUFSZ2 */

static char_t	rcv_buffer2[SERIAL_RCV_BUFSZ2];
static char_t	snd_buffer2[SERIAL_SND_BUFSZ2];

#endif /* TNUM_PORT >= 2 */

#if TNUM_PORT >= 3						/* definitions for port3 */

#ifndef SERIAL_RCV_BUFSZ3
#define	SERIAL_RCV_BUFSZ3	256			/* receive buffer size of port3 */
#endif /* SERIAL_RCV_BUFSZ3 */

#ifndef SERIAL_SND_BUFSZ3
#define	SERIAL_SND_BUFSZ3	256			/* send buffer size of port3 */
#endif /* SERIAL_SND_BUFSZ3 */

static char_t	rcv_buffer3[SERIAL_RCV_BUFSZ3];
static char_t	snd_buffer3[SERIAL_SND_BUFSZ3];

#endif /* TNUM_PORT >= 3 */


#if TNUM_PORT >= 4						/* definitions for port4 */

#ifndef SERIAL_RCV_BUFSZ4
#define	SERIAL_RCV_BUFSZ4	256			/* receive buffer size of port4 */
#endif /* SERIAL_RCV_BUFSZ4 */

#ifndef SERIAL_SND_BUFSZ4
#define	SERIAL_SND_BUFSZ4	256			/* send buffer size of port4 */
#endif /* SERIAL_SND_BUFSZ4 */

static char_t	rcv_buffer4[SERIAL_RCV_BUFSZ4];
static char_t	snd_buffer4[SERIAL_SND_BUFSZ4];

#endif /* TNUM_PORT >= 4 */
/*
 *  constants and macros related to flow control
 */
#define	FC_STOP			'\023'		/* Ctrl-S */
#define	FC_START		'\021'		/* Ctrl-Q */

#define BUFCNT_STOP(bufsz)		((bufsz) * 3 / 4)	/* the upper size limit of sending FC_STOP*/
#define BUFCNT_START(bufsz)		((bufsz) / 2)		/* the lower size limit of sending FC_START*/

/*
 *  serial port initialization block
 */
typedef struct serial_port_initialization_block {
	ID		rcv_semid;		/* the semaphore ID for receiving */
	ID		snd_semid;		/* the semaphore ID for sending */
	uint_t	rcv_bufsz;		/* receive buffer size */
	char_t	*rcv_buffer;	/* receive buffer */
	uint_t	snd_bufsz;		/* send buffer size */
	char_t	*snd_buffer;	/* send buffer */
} SPINIB;

static const SPINIB spinib_table[TNUM_PORT] = {
	{ SERIAL_RCV_SEM1, SERIAL_SND_SEM1,
	  SERIAL_RCV_BUFSZ1, rcv_buffer1,
	  SERIAL_SND_BUFSZ1, snd_buffer1 },
#if TNUM_PORT >= 2
	{ SERIAL_RCV_SEM2, SERIAL_SND_SEM2,
	  SERIAL_RCV_BUFSZ2, rcv_buffer2,
	  SERIAL_SND_BUFSZ2, snd_buffer2 },
#endif /* TNUM_PORT >= 2 */
#if TNUM_PORT >= 3
	{ SERIAL_RCV_SEM3, SERIAL_SND_SEM3,
	  SERIAL_RCV_BUFSZ3, rcv_buffer3,
	  SERIAL_SND_BUFSZ3, snd_buffer3 },
#endif /* TNUM_PORT >= 3 */
#if TNUM_PORT >= 4
	{ SERIAL_RCV_SEM4, SERIAL_SND_SEM4,
	  SERIAL_RCV_BUFSZ4, rcv_buffer4,
	  SERIAL_SND_BUFSZ4, snd_buffer4 },
#endif /* TNUM_PORT >= 4 */
};

/*
 *  serial port control block
 */
typedef struct serial_port_control_block {
	const SPINIB *p_spinib;		/* serial port initialization block */
	SIOPCB	*p_siopcb;			/* serial IO port control block  */
	bool_t	openflag;			/* the open flag */
	bool_t	errorflag;			/* the error flag */
	uint_t	ioctl;				/*  io contorl setting*/

	uint_t	rcv_read_ptr;		/* receive buffer read pointer */
	uint_t	rcv_write_ptr;		/* receive buffer write pointer */
	uint_t	rcv_count;			/* the number of characters in receive buffer */
	char_t	rcv_fc_chr;			/* flow control char(START/STOP) */
	bool_t	rcv_stopped;		/* receive stop flag */

	uint_t	snd_read_ptr;		/* send buffer read pointer */
	uint_t	snd_write_ptr;		/* send buffer write pointer  */
	uint_t	snd_count;			/* the number of characters in send buffer */
	bool_t	snd_stopped;		/* send stop flag */
} SPCB;

static SPCB	spcb_table[TNUM_PORT];

/*
 *  macros for getting serial port control block from serial port id
 */
#define INDEX_PORT(portid)	((uint_t)((portid) - 1))
#define get_spcb(portid)	(&(spcb_table[INDEX_PORT(portid)]))

/*
 *  increment the pointer
 */
#define INC_PTR(ptr, bufsz)		{ if (++(ptr) == (bufsz)) { (ptr) = 0; }}

/*
 *  service call macro
 *
 *  check the return value of service call (exp), if it is error (<0),
 *  set ercd with ercd_exp, and goto error_exit.
 */
#define SVC(exp, ercd_exp) \
				{ if ((exp) < 0) { ercd = (ercd_exp); goto error_exit; }}

/*
 *  generate E_SYS error
 */
static ER
gen_ercd_sys(SPCB *p_spcb)
{
	p_spcb->errorflag = true;
	return(E_SYS);
}

/*
 * 	generate error from the return value of service call which makes the
 * 	calling task into waiting state
 */
static ER
gen_ercd_wait(ER rercd, SPCB *p_spcb)
{
	switch (MERCD(rercd)) {
	case E_RLWAI:
	case E_DLT:
		return(rercd);
	default:
		p_spcb->errorflag = true;
		return(E_SYS);
	}
}

/*
 *  serial interface driver initialization routine
 */
void
serial_initialize(intptr_t exinf)
{
	uint_t	i;
	SPCB	*p_spcb;

	for (p_spcb = spcb_table, i = 0; i < TNUM_PORT; p_spcb++, i++) {
		p_spcb->p_spinib = &(spinib_table[i]);
		p_spcb->openflag = false;
	}
}

/*
 *  open a serial port (service call)
 */
ER
serial_opn_por(ID portid)
{
	SPCB	*p_spcb;
	ER		ercd;

	if (sns_dpn()) {				/* check context */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);				/* check port number */
	}
	p_spcb = get_spcb(portid);

	SVC(dis_dsp(), gen_ercd_sys(p_spcb));
	if (p_spcb->openflag) {			/* check whether it is already open */
		ercd = E_OBJ;
	}
	else {
		/*
		 *  variables initialization
		 */
		p_spcb->ioctl = (IOCTL_ECHO | IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV);

		p_spcb->rcv_read_ptr = p_spcb->rcv_write_ptr = 0U;
		p_spcb->rcv_count = 0U;
		p_spcb->rcv_fc_chr = '\0';
		p_spcb->rcv_stopped = false;

		p_spcb->snd_read_ptr = p_spcb->snd_write_ptr = 0U;
		p_spcb->snd_count = 0U;
		p_spcb->snd_stopped = false;

		/*
		 *  after this, interrupts are disabled．
		 */
		if (loc_cpu() < 0) {
			ercd = E_SYS;
			goto error_exit_enadsp;
		}

		/*
		 *  hardware dependent open
		 */
		p_spcb->p_siopcb = sio_opn_por(portid, (intptr_t) p_spcb);

		/*
		 *  enable receive callback routine．
		 */
		sio_ena_cbr(p_spcb->p_siopcb, SIO_RDY_RCV);
		p_spcb->openflag = true;
		p_spcb->errorflag = false;

		if (unl_cpu() < 0) {
			p_spcb->errorflag = true;
			ercd = E_SYS;
			goto error_exit_enadsp;
		}
		ercd = E_OK;
	}

  error_exit_enadsp:
	SVC(ena_dsp(), gen_ercd_sys(p_spcb));

  error_exit:
	return(ercd);
}

/*
 *  close the serial port (service call)
 */
ER
serial_cls_por(ID portid)
{
	SPCB	*p_spcb;
	ER		ercd;
	bool_t	eflag = false;

	if (sns_dpn()) {				/* check context */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);				/* check port number */
	}
	p_spcb = get_spcb(portid);

	SVC(dis_dsp(), gen_ercd_sys(p_spcb));
	if (!(p_spcb->openflag)) {		/* check whether it is already closed */
		ercd = E_OBJ;
	}
	else {
		/*
		 *  hardware-dependent close
		 */
		if (loc_cpu() < 0) {
			eflag = true;
		}
		sio_cls_por(p_spcb->p_siopcb);
		p_spcb->openflag = false;
		if (unl_cpu() < 0) {
			eflag = true;
		}

		/*
		 *  re-initialize semaphores
		 */
		if (ini_sem(p_spcb->p_spinib->snd_semid) < 0) {
			eflag = true;
		}
		if (ini_sem(p_spcb->p_spinib->rcv_semid) < 0) {
			eflag = true;
		}

		/*
		 *  set error code
		 */
		if (eflag) {
			ercd = gen_ercd_sys(p_spcb);
		}
		else {
			ercd = E_OK;
		}
	}
	SVC(ena_dsp(), gen_ercd_sys(p_spcb));

  error_exit:
	return(ercd);
}

/*
 *  send one char through the serial port
 *
 *  send one char (c) through the serial port specified by p_spcb.
 *  if the char is successfully put into the send register, return ture.
 *  if not, enable the send callback routine (enable interrupt) and
 *  return false. this function is called when cpu is locked. 
 */
Inline bool_t
serial_snd_chr(SPCB *p_spcb, char_t c)
{
	if (sio_snd_chr(p_spcb->p_siopcb, c)) {
		return(true);
	}
	else {
		sio_ena_cbr(p_spcb->p_siopcb, SIO_RDY_SND);
		return(false);
	}
}

/*
 * send one char through the serial port
 */
static ER_BOOL
serial_wri_chr(SPCB *p_spcb, char_t c)
{
	bool_t	buffer_full;
	ER		ercd, rercd;

	/*
	 *  send CR before LF．
	 */
	if (c == '\n' && (p_spcb->ioctl & IOCTL_CRLF) != 0U) {
		SVC(rercd = serial_wri_chr(p_spcb, '\r'), rercd);
		if ((bool_t) rercd) {
			SVC(rercd = wai_sem(p_spcb->p_spinib->snd_semid),
										gen_ercd_wait(rercd, p_spcb));
		}
	}

	SVC(loc_cpu(), gen_ercd_sys(p_spcb));
	if (p_spcb->snd_count == 0U && !(p_spcb->snd_stopped)
								&& serial_snd_chr(p_spcb, c)) {
		 
		/*
		 * if the char is successfully put into the send register
		 */
		buffer_full = false;
	}
	else {
		/*
		 *   put the char into send buffer
		 */
		p_spcb->p_spinib->snd_buffer[p_spcb->snd_write_ptr] = c;
		INC_PTR(p_spcb->snd_write_ptr, p_spcb->p_spinib->snd_bufsz);
		p_spcb->snd_count++;
		buffer_full = (p_spcb->snd_count == p_spcb->p_spinib->snd_bufsz);
	}

	SVC(unl_cpu(), gen_ercd_sys(p_spcb));
	ercd = (ER_BOOL) buffer_full;

  error_exit:
	return(ercd);
}

/*
 * send data through serial port (service call)
 */
ER_UINT
serial_wri_dat(ID portid, const char_t *buf, uint_t len)
{
	SPCB	*p_spcb;
	bool_t	buffer_full;
	uint_t	wricnt = 0U;
	ER		ercd, rercd;

	if (sns_dpn()) {				/* check context */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);				/* check  port number */
	}

	p_spcb = get_spcb(portid);
	if (!(p_spcb->openflag)) {		/* check whether it is already open */
		return(E_OBJ);
	}
	if (p_spcb->errorflag) {		/* check whether error exists */
		return(E_SYS);
	}

	buffer_full = true;				/* first loop, wai_sem */
	while (wricnt < len) {
		if (buffer_full) {
			SVC(rercd = wai_sem(p_spcb->p_spinib->snd_semid),
										gen_ercd_wait(rercd, p_spcb));
		}
		SVC(rercd = serial_wri_chr(p_spcb, *buf++), rercd);
		wricnt++;
		buffer_full = (bool_t) rercd;
	}
	if (!buffer_full) {
		SVC(sig_sem(p_spcb->p_spinib->snd_semid), gen_ercd_sys(p_spcb));
	}
	ercd = E_OK;

  error_exit:
	return(wricnt > 0U ? (ER_UINT) wricnt : ercd);
}

/*
 *  receive one char from serial port
 */
static bool_t
serial_rea_chr(SPCB *p_spcb, char_t *p_c)
{
	bool_t	buffer_empty;
	ER		ercd;

	SVC(loc_cpu(), gen_ercd_sys(p_spcb));

	/*
	 *  get one char from serial port
	 */
	*p_c = p_spcb->p_spinib->rcv_buffer[p_spcb->rcv_read_ptr];
	INC_PTR(p_spcb->rcv_read_ptr, p_spcb->p_spinib->rcv_bufsz);
	p_spcb->rcv_count--;
	buffer_empty = (p_spcb->rcv_count == 0U);

	/*
	 *  send START?
	 */
	if (p_spcb->rcv_stopped && p_spcb->rcv_count
								<= BUFCNT_START(p_spcb->p_spinib->rcv_bufsz)) {
		if (!serial_snd_chr(p_spcb, FC_START)) {
			p_spcb->rcv_fc_chr = FC_START;
		}
		p_spcb->rcv_stopped = false;
	}

	SVC(unl_cpu(), gen_ercd_sys(p_spcb));
	ercd = (ER_BOOL) buffer_empty;

  error_exit:
	return(ercd);
}

/*
 *  receive data from the serial port (service call)
 */
ER_UINT
serial_rea_dat(ID portid, char_t *buf, uint_t len)
{
	SPCB	*p_spcb;
	bool_t	buffer_empty;
	uint_t	reacnt = 0U;
	char_t	c = '\0';		/* initialize the variable to suppress compiler warning */
	ER		ercd, rercd;

	if (sns_dpn()) {				/* check context */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);				/* check port number */
	}

	p_spcb = get_spcb(portid);
	if (!(p_spcb->openflag)) {		/* check whether it is already open */
		return(E_OBJ);
	}
	if (p_spcb->errorflag) {		/* check whether error exists */
		return(E_SYS);
	}

	buffer_empty = true;			/* first loop, wai_sem */
	while (reacnt < len) {
		if (buffer_empty) {
			SVC(rercd = wai_sem(p_spcb->p_spinib->rcv_semid),
										gen_ercd_wait(rercd, p_spcb));
		}
		SVC(rercd = serial_rea_chr(p_spcb, &c), rercd);
		*buf++ = c;
		reacnt++;
		buffer_empty = (bool_t) rercd;

		/*
		 * echo back processing
		 */
		if ((p_spcb->ioctl & IOCTL_ECHO) != 0U) {
			SVC(rercd = wai_sem(p_spcb->p_spinib->snd_semid),
										gen_ercd_wait(rercd, p_spcb));
			SVC(rercd = serial_wri_chr(p_spcb, c), rercd);
			if (!((bool_t) rercd)) {
				SVC(sig_sem(p_spcb->p_spinib->snd_semid),
										gen_ercd_sys(p_spcb));
			}
		}
	}
	if (!buffer_empty) {
		SVC(sig_sem(p_spcb->p_spinib->rcv_semid), gen_ercd_sys(p_spcb));
	}
	ercd = E_OK;

  error_exit:
	return(reacnt > 0U ? (ER_UINT) reacnt : ercd);
}

/*
 * serial port IO control (service call)
 */
ER
serial_ctl_por(ID portid, uint_t ioctl)
{
	SPCB	*p_spcb;

	if (sns_dpn()) {				/* check context */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);				/* check port number */
	}

	p_spcb = get_spcb(portid);
	if (!(p_spcb->openflag)) {		/* check whether it is already open */
		return(E_OBJ);
	}
	if (p_spcb->errorflag) {		/* check whether error exists */
		return(E_SYS);
	}

	p_spcb->ioctl = ioctl;
	return(E_OK);
}

/*
 *  refer serial port's status (service call)
 */
ER
serial_ref_por(ID portid, T_SERIAL_RPOR *pk_rpor)
{
	SPCB	*p_spcb;

	if (sns_dpn()) {				/* check context */
		return(E_CTX);
	}
	if (!(1 <= portid && portid <= TNUM_PORT)) {
		return(E_ID);				/* check the port number */
	}

	p_spcb = get_spcb(portid);
	if (!(p_spcb->openflag)) {		/* check whether it is already open */
		return(E_OBJ);
	}
	if (p_spcb->errorflag) {		/* check whether error exists */
		return(E_SYS);
	}

	pk_rpor->reacnt = p_spcb->rcv_count;
	pk_rpor->wricnt = p_spcb->snd_count;
	return(E_OK);
}

/*
 *  send call back routine when serial port is ready for sending
 *  (called in serial port interrupt handler)
 */
void
sio_irdy_snd(intptr_t exinf)
{
	SPCB	*p_spcb;

	p_spcb = (SPCB *) exinf;
	if (p_spcb->rcv_fc_chr != '\0') {
		/*
		 *  send START/STOP．
		 */
		(void) sio_snd_chr(p_spcb->p_siopcb, p_spcb->rcv_fc_chr);
		p_spcb->rcv_fc_chr = '\0';
	}
	else if (!(p_spcb->snd_stopped) && p_spcb->snd_count > 0U) {
		/*
		 *  get one char from send buffer and send
		 */
		(void) sio_snd_chr(p_spcb->p_siopcb,
					p_spcb->p_spinib->snd_buffer[p_spcb->snd_read_ptr]);
		INC_PTR(p_spcb->snd_read_ptr, p_spcb->p_spinib->snd_bufsz);
		if (p_spcb->snd_count == p_spcb->p_spinib->snd_bufsz) {
			if (isig_sem(p_spcb->p_spinib->snd_semid) < 0) {
				p_spcb->errorflag = true;
			}
		}
		p_spcb->snd_count--;
	}
	else {
		/*
		 *  if the send buffer is empty, disable send callback routine
		 */
		sio_dis_cbr(p_spcb->p_siopcb, SIO_RDY_SND);
	}
}

/*
 * receive callback routine when serialport is ready for receving
 */
void
sio_irdy_rcv(intptr_t exinf)
{
	SPCB	*p_spcb;
	char_t	c;

	p_spcb = (SPCB *) exinf;
	c = (char_t) sio_rcv_chr(p_spcb->p_siopcb);
	if ((p_spcb->ioctl & IOCTL_FCSND) != 0U && c == FC_STOP) {
		/*
		 *  stop sending, the char being sent will be sent
		 */
		p_spcb->snd_stopped = true;
	}
	else if (p_spcb->snd_stopped && (c == FC_START
				|| (p_spcb->ioctl & IOCTL_FCANY) != 0U)) {
		/*
		 *  restart sending
		 */
		p_spcb->snd_stopped = false;
		if (p_spcb->snd_count > 0U) {
			c = p_spcb->p_spinib->snd_buffer[p_spcb->snd_read_ptr];
			if (serial_snd_chr(p_spcb, c)) {
				INC_PTR(p_spcb->snd_read_ptr, p_spcb->p_spinib->snd_bufsz);
				if (p_spcb->snd_count == p_spcb->p_spinib->snd_bufsz) {
					if (isig_sem(p_spcb->p_spinib->snd_semid) < 0) {
						p_spcb->errorflag = true;
					}
				}
				p_spcb->snd_count--;
			}
		}
	}
	else if ((p_spcb->ioctl & IOCTL_FCSND) != 0U && c == FC_START) {
		/*
		 * if send flow control is enabled,  START will be discarded.
		 */
	}
	else if (p_spcb->rcv_count == p_spcb->p_spinib->rcv_bufsz) {
		/*
		 * if the receive buffer is full, received char will be discarded.
		 */
	}
	else {
		/*
		 *  put received char into the receive buffer.
		 */
		p_spcb->p_spinib->rcv_buffer[p_spcb->rcv_write_ptr] = c;
		INC_PTR(p_spcb->rcv_write_ptr, p_spcb->p_spinib->rcv_bufsz);
		if (p_spcb->rcv_count == 0U) {
			if (isig_sem(p_spcb->p_spinib->rcv_semid) < 0) {
				p_spcb->errorflag = true;
			}
		}
		p_spcb->rcv_count++;

		/*
		 *  send a STOP before receive buffer is full
		 */
		if ((p_spcb->ioctl & IOCTL_FCRCV) != 0U && !(p_spcb->rcv_stopped)
						&& p_spcb->rcv_count
							>= BUFCNT_STOP(p_spcb->p_spinib->rcv_bufsz)) {
			if (!serial_snd_chr(p_spcb, FC_STOP)) {
				p_spcb->rcv_fc_chr = FC_STOP;
			}
			p_spcb->rcv_stopped = true;
		}
	}
}

/*
 *  get one char that is not sent from serial port send buffer
 */
bool_t
serial_get_chr(ID portid, char_t *p_c)
{
	SPCB	*p_spcb;

	if (1 <= portid && portid <= TNUM_PORT) {	/* check the port number */
		p_spcb = get_spcb(portid);
		if (p_spcb->openflag) {					/* check if is open */
			if (p_spcb->snd_count > 0U) {
				*p_c = p_spcb->p_spinib->snd_buffer[p_spcb->snd_read_ptr];
				INC_PTR(p_spcb->snd_read_ptr, p_spcb->p_spinib->snd_bufsz);
				p_spcb->snd_count--;
				return(true);
			}
		}
	}
	return(false);
}
