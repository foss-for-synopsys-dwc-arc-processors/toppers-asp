/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2008 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: serial.h 1176 2008-07-01 10:24:46Z ertl-hiro $
 */

/*
 *		serial interface driver
 */

#ifndef TOPPERS_SERIAL_H
#define TOPPERS_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  refer packet for serial interface driver
 */
typedef struct {
	uint_t		reacnt;			/* count of chars in receive buffer*/
	uint_t		wricnt;			/* count of chars  in send buffer */
} T_SERIAL_RPOR;

/*
 *  serial interface driver initialization routine
 */
extern void		serial_initialize(intptr_t exinf) throw();

/*
 *  get one char that is not sent from serial port send buffer
 */
extern bool_t	serial_get_chr(ID portid, char_t *p_c) throw();

/*
 * serial interface driver service calls
 */
extern ER		serial_opn_por(ID portid) throw();
extern ER		serial_cls_por(ID portid) throw();
extern ER_UINT	serial_rea_dat(ID portid, char_t *buf, uint_t len) throw();
extern ER_UINT	serial_wri_dat(ID portid, const char_t *buf, uint_t len)
																	throw();
extern ER		serial_ctl_por(ID portid, uint_t ioctl) throw();
extern ER		serial_ref_por(ID portid, T_SERIAL_RPOR *pk_rpor) throw();

/*
 *  definitons for serial interface driver io control
 *
 */
#define	IOCTL_NULL	0U			/* NULL */
#define	IOCTL_ECHO	0x0001U		/* enable echo back */
#define	IOCTL_CRLF	0x0010U		/* add CR before LF */
#define	IOCTL_FCSND	0x0100U		/* enable send flow control */
#define	IOCTL_FCANY	0x0200U		/* restart to send whatever char is received */
#define	IOCTL_FCRCV	0x0400U		/* enable receive flow control */

#ifdef __cplusplus
}
#endif

#endif /* TOPPERS_SERIAL_H */
