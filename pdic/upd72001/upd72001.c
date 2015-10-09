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
 *		(a) The above copyright notice, this use conditions, and the
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
 *  @(#) $Id: upd72001.c 873 2008-04-11 10:32:26Z hiro $
 */

/*
 *		simple SIO driver for uPD72001
 */

#include <sil.h>
#include "target_syssvc.h"
#include "upd72001.h"

/*
 *  the time interval to access registers (unit: nse)
 *
 *  200 is a magic value
 */
#define	UPD72001_DELAY	200U

/*
 *  register number of uPD72001
 */
#define	UPD72001_CR0	0x00U		/* control register */
#define	UPD72001_CR1	0x01U
#define	UPD72001_CR2	0x02U
#define	UPD72001_CR3	0x03U
#define	UPD72001_CR4	0x04U
#define	UPD72001_CR5	0x05U
#define	UPD72001_CR10	0x0aU
#define	UPD72001_CR12	0x0cU
#define	UPD72001_CR14	0x0eU
#define	UPD72001_CR15	0x0fU

#define	UPD72001_SR0	0x00U		/* status register */

/*
 *  setting value of control register
 */
#define CR_RESET	0x18U		/* port rest */

#define CR0_EOI		0x38U		/* EOI（End of Interrupt）*/

#define CR1_DOWN	0x00U		/* disable interrupts */
#define CR1_RECV	0x10U		/* enable receive interrupt */
#define CR1_SEND	0x02U		/* enable send interrupt */

#define CR3_DEF		0xc1U		/* 8bit data, receive enabled */
#define CR4_DEF		0x44U		/* 1 stop bit, no parity */
#define CR5_DEF		0xeaU		/* 8bit data, send enabled */

#define CR10_DEF	0x00U		/* NRZ */
#define CR14_DEF	0x07U		/* baud rate generator enabled */
#define CR15_DEF	0x56U		/* use baud rate generator */

#define SR0_RECV	0x01U		/* ready to receive */
#define SR0_SEND	0x04U		/* ready to send  */

/*
 *  sio port initialization block
 */
typedef struct sio_port_initialization_block {
	void		*data;			/* data register address */
	void		*ctrl;			/* control register address*/

	uint8_t		cr3_def;		/* CR3 (receive data bit)*/
	uint8_t		cr4_def;		/* CR4 (stop bit, parity）*/
	uint8_t		cr5_def;		/* CR5 (send data bits）*/
	uint8_t		brg1_def;		/* the upper value of baudrate */
	uint8_t		brg2_def;		/* the lower value of baudrate */
} SIOPINIB;

/*
 *  sio port control block
 */
struct sio_port_control_block {
	const SIOPINIB *p_siopinib;	/* pointer to sio init block  */
	intptr_t	exinf;			/* extend information  */
	bool_t		openflag;		/* open flag  */
	uint8_t		cr1;			/* CR1 (interrupt control)*/
	bool_t		getready;		/* ready to get */
	bool_t		putready;		/* ready to put */
};

/*
 *  sio port initialization block table
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{ (void *) TADR_UPD72001_DATAA, (void *) TADR_UPD72001_CTRLA,
					CR3_DEF, CR4_DEF, CR5_DEF, BRG1_DEF, BRG2_DEF },
	{ (void *) TADR_UPD72001_DATAB, (void *) TADR_UPD72001_CTRLB,
					CR3_DEF, CR4_DEF, CR5_DEF, BRG1_DEF, BRG2_DEF }
};

/*
 *  sio control block table
 */
SIOPCB	siopcb_table[TNUM_SIOP];

/*
 *  macro of getting sio port control block from sio port id
 */
#define INDEX_SIOP(siopid)	((uint_t)((siopid) - 1))
#define get_siopcb(siopid)	(&(siopcb_table[INDEX_SIOP(siopid)]))

/*
 *  read device register
 */
Inline uint8_t
upd72001_read_reg(void *addr)
{
	uint8_t	val;

	val = upd72001_reb_reg(addr);
	sil_dly_nse(UPD72001_DELAY);
	return(val);
}

Inline void
upd72001_write_reg(void *addr, uint8_t val)
{
	upd72001_wrb_reg(addr, val);
	sil_dly_nse(UPD72001_DELAY);
}

Inline uint8_t
upd72001_read_ctrl(void *addr, uint8_t reg)
{
	upd72001_write_reg(addr, reg);
	return(upd72001_read_reg(addr));
}

Inline void
upd72001_write_ctrl(void *addr, uint8_t reg, uint8_t val)
{
	upd72001_write_reg(addr, reg);
	upd72001_write_reg(addr, val);
}

Inline void
upd72001_write_brg(void *addr, uint8_t reg, uint8_t val,
									uint8_t brg2, uint8_t brg1)
{
	upd72001_write_reg(addr, reg);
	upd72001_write_reg(addr, val);
	upd72001_write_reg(addr, brg2);
	upd72001_write_reg(addr, brg1);
	(void) upd72001_read_reg(addr);		/* dummy read */
}

/*
 *  get status (read SR0)
 *
 */
static void
upd72001_get_stat(SIOPCB *p_siopcb)
{
	uint8_t	sr0;

	sr0 = upd72001_read_ctrl(p_siopcb->p_siopinib->ctrl, UPD72001_SR0);
	if ((sr0 & SR0_RECV) != 0) {
		p_siopcb->getready = true;
	}
	if ((sr0 & SR0_SEND) != 0) {
		p_siopcb->putready = true;
	}
}

/*
 *  ready to get?
 */
Inline bool_t
upd72001_getready(SIOPCB *p_siopcb)
{
	upd72001_get_stat(p_siopcb);
	return(p_siopcb->getready);
}

/*
 *   ready to put?
 */
Inline bool_t
upd72001_putready(SIOPCB *p_siopcb)
{
	upd72001_get_stat(p_siopcb);
	return(p_siopcb->putready);
}

/*
 *  get received char
 */
Inline char_t
upd72001_getchar(SIOPCB *p_siopcb)
{
	p_siopcb->getready = false;
	return((char_t) upd72001_read_reg(p_siopcb->p_siopinib->data));
}

/*
 *  write the char
 */
Inline void
upd72001_putchar(SIOPCB *p_siopcb, char_t c)
{
	p_siopcb->putready = false;
	upd72001_write_reg(p_siopcb->p_siopinib->data, (uint8_t) c);
}

/*
 *  do EOI
 */
Inline void
upd72001_eoi(void)
{
	upd72001_write_ctrl((void *) TADR_UPD72001_CTRLA, UPD72001_CR0, CR0_EOI);
}

/*
 *  initialize sio driver
 */
void
upd72001_initialize(void)
{
	SIOPCB	*p_siopcb;
	uint_t	i;

	/*
	 * initialize sio port control block
	 */
	for (p_siopcb = siopcb_table, i = 0; i < TNUM_SIOP; p_siopcb++, i++) {
		p_siopcb->p_siopinib = &(siopinib_table[i]);
		p_siopcb->openflag = false;
	}
}

/*
 *  already open ?
 */
bool_t
upd72001_openflag(void)
{
	return(siopcb_table[0].openflag || siopcb_table[1].openflag);
}

/*
 *  open sio port
 */
SIOPCB *
upd72001_opn_por(ID siopid, intptr_t exinf)
{
	SIOPCB		*p_siopcb;
	const SIOPINIB	*p_siopinib;

	p_siopcb = get_siopcb(siopid);
	p_siopinib = p_siopcb->p_siopinib;

	upd72001_write_reg(p_siopinib->ctrl, CR_RESET);
	if (!upd72001_openflag()) {
		upd72001_write_ctrl((void *) TADR_UPD72001_CTRLA, UPD72001_CR2, 0x18);
		upd72001_write_ctrl((void *) TADR_UPD72001_CTRLB, UPD72001_CR2, 0x00);
	}
	p_siopcb->cr1 = CR1_DOWN;
	upd72001_write_ctrl(p_siopinib->ctrl, UPD72001_CR1, p_siopcb->cr1);
	upd72001_write_ctrl(p_siopinib->ctrl, UPD72001_CR4, p_siopinib->cr4_def);
	upd72001_write_brg(p_siopinib->ctrl, UPD72001_CR12, 0x01U,
							p_siopinib->brg2_def, p_siopinib->brg1_def);
	upd72001_write_brg(p_siopinib->ctrl, UPD72001_CR12, 0x02U,
							p_siopinib->brg2_def, p_siopinib->brg1_def);
	upd72001_write_ctrl(p_siopinib->ctrl, UPD72001_CR15, CR15_DEF);
	upd72001_write_ctrl(p_siopinib->ctrl, UPD72001_CR14, CR14_DEF);
	upd72001_write_ctrl(p_siopinib->ctrl, UPD72001_CR10, CR10_DEF);
	upd72001_write_ctrl(p_siopinib->ctrl, UPD72001_CR3, p_siopinib->cr3_def);
	upd72001_write_ctrl(p_siopinib->ctrl, UPD72001_CR5, p_siopinib->cr5_def);
	p_siopcb->exinf = exinf;
	p_siopcb->getready = p_siopcb->putready = false;
	p_siopcb->openflag = true;
	return(p_siopcb);
}

/*
 *  close sio port
 */
void
upd72001_cls_por(SIOPCB *p_siopcb)
{
	upd72001_write_ctrl(p_siopcb->p_siopinib->ctrl, UPD72001_CR1, CR1_DOWN);
	p_siopcb->openflag = false;
}

/*
 *  sio port send char
 */
bool_t
upd72001_snd_chr(SIOPCB *p_siopcb, char_t c)
{
	if (upd72001_putready(p_siopcb)) {
		upd72001_putchar(p_siopcb, c);
		return(true);
	}
	return(false);
}

/*
 *  sio port receive char
 */
int_t
upd72001_rcv_chr(SIOPCB *p_siopcb)
{
	if (upd72001_getready(p_siopcb)) {
		return((int_t)(uint8_t) upd72001_getchar(p_siopcb));
	}
	return(-1);
}

/*
 *  enable callback routine of sio port
 */
void
upd72001_ena_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	uint8_t	cr1_bit = 0;

	switch (cbrtn) {
	case SIO_RDY_SND:
		cr1_bit = CR1_SEND;
		break;
	case SIO_RDY_RCV:
		cr1_bit = CR1_RECV;
		break;
	}
	p_siopcb->cr1 |= cr1_bit;
	upd72001_write_ctrl(p_siopcb->p_siopinib->ctrl,
									UPD72001_CR1, p_siopcb->cr1);
}

/*
 *  disable callback routine of sio port
 */
void
upd72001_dis_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	uint8_t	cr1_bit = 0;

	switch (cbrtn) {
	case SIO_RDY_SND:
		cr1_bit = CR1_SEND;
		break;
	case SIO_RDY_RCV:
		cr1_bit = CR1_RECV;
		break;
	}
	p_siopcb->cr1 &= ~cr1_bit;
	upd72001_write_ctrl(p_siopcb->p_siopinib->ctrl,
									UPD72001_CR1, p_siopcb->cr1);
}

/*
 *  sio port interrupt handling
 */
static void
upd72001_isr_siop(SIOPCB *p_siopcb)
{
	if ((p_siopcb->cr1 & CR1_RECV) != 0U && upd72001_getready(p_siopcb)) {
		/*
		 *  call receive callback routine
		 */
		upd72001_irdy_rcv(p_siopcb->exinf);
	}
	if ((p_siopcb->cr1 & CR1_SEND) != 0U && upd72001_putready(p_siopcb)) {
		/*
		 *  call send callback routine
		 */
		upd72001_irdy_snd(p_siopcb->exinf);
	}
}

/*
 *  sio port interrupt service routine
 */
void
upd72001_isr(void)
{
	if (siopcb_table[0].openflag) {
		upd72001_isr_siop(&(siopcb_table[0]));
	}
	if (siopcb_table[1].openflag) {
		upd72001_isr_siop(&(siopcb_table[1]));
	}
	upd72001_eoi();
}
