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
 *  @(#) $Id: upd72001.h 873 2008-04-11 10:32:26Z hiro $
 */

/*
 *			simple SIO driver for uPD72001
 */

#ifndef TOPPERS_UPD72001_H
#define TOPPERS_UPD72001_H

/*
 *   the number of sio port
 */
#define TNUM_SIOP		2		/* the number of supported sio port */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  sio port control block
 */
typedef struct sio_port_control_block	SIOPCB;

/*
 *  the id no. of call back routine
 */
#define SIO_RDY_SND	1U		/* ready to send callback */
#define SIO_RDY_RCV	2U		/* ready to receive callback */

/*
 *  initialize sio driver
 */
extern void		upd72001_initialize(void);

/*
 *  already open?
 */
extern bool_t	upd72001_openflag(void);

/*
 *  open sio port
 */
extern SIOPCB	*upd72001_opn_por(ID siopid, intptr_t exinf);

/*
 *  close sio port
 */
extern void		upd72001_cls_por(SIOPCB *siopcb);

/*
 *  sio port send char
 */
extern bool_t	upd72001_snd_chr(SIOPCB *siopcb, char_t c);

/*
 *  sio port receive char
 */
extern int_t	upd72001_rcv_chr(SIOPCB *siopcb);

/*
 *  enable callback routine of sio port
 */
extern void		upd72001_ena_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  disable callback routine of sio port
 */
extern void		upd72001_dis_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  sio interrupt service routine
 */
extern void		upd72001_isr(void);

/*
 *  ready to send callback in interrupt
 */
extern void		upd72001_irdy_snd(intptr_t exinf);

/*
 *  ready to receive callback in interrupt
 */
extern void		upd72001_irdy_rcv(intptr_t exinf);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_UPD72001_H */
