/* Copyright (C) 
 * 2013 - Wayne Ren
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/*
 *  serial IO device (SIO) driver (for emstartkit)
 */

#include <kernel.h>
#include <t_syslog.h>
#include "emstartkit.h"
#include "target_serial.h"


#if TNUM_SIOP == 0
#error  "TNUM_SIOP should not be 0"
#endif

/*
 *  sio port initialization block
 */
typedef struct sio_port_initialization_block 
{
    DW_APB_UART_STRUCT_PTR regs;
    uint32_t intno;
}
SIOPINIB;

/*
 *  sio port control block
 */
struct sio_port_control_block 
{
    const SIOPINIB	*p_siopinib;		/* pointer to sio init block */
    intptr_t	exinf;				/* extend information */
    bool_t	openflag;			/* open flag */
    bool_t	sendflag;			/* send flag  */
    bool_t	getready;			/* ready to get */
    bool_t	putready;			/* ready to put */
};

/*
 * sio port initialization block table
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{(DW_APB_UART_STRUCT_PTR)REGBASE_UART0, INTNO_UART0},
#if TNUM_SIOP > 1
	{(DW_APB_UART_STRUCT_PTR)REGBASE_UART1, INTNO_UART1},
#endif
};

/*
 *  sio control block table
 */
SIOPCB	siopcb_table[TNUM_SIOP];

/*
 *  macro of getting sio port control block from sio port id
 */
#define INDEX_SIOP(siopid)  (siopid)
#define get_siopcb(siopid)  (&(siopcb_table[INDEX_SIOP(siopid)]))
#define get_siopinib(siopid) (&(siopinib_table[INDEX_SIOP(siopid)]))

/*
 * ready to get?
 */ 
Inline bool_t
uart_getready(DW_APB_UART_STRUCT_PTR regs)
{
	return ((regs->USR & USR_RFNE) != 0);
}

/*
 * ready to put?
 */
Inline bool_t
uart_putready(DW_APB_UART_STRUCT_PTR regs)
{
	return ((regs->USR & USR_TFNF) != 0);
}

/*
 *  get received char
 */
Inline uint8_t
uart_getchar(DW_APB_UART_STRUCT_PTR regs)
{
	return regs->DATA;
}

/*
 *  write the char
 */
Inline void
uart_putchar(DW_APB_UART_STRUCT_PTR regs, uint8_t c)
{
	regs->DATA = c;
}

/*
 * enable send interrupt
 */
Inline void
uart_enable_send(DW_APB_UART_STRUCT_PTR regs)
{
	regs->IER |= IER_XMIT_EMPTY;
}

/*
 *  disable send interrupt
 */
Inline void
uart_disable_send(DW_APB_UART_STRUCT_PTR regs)
{
	regs->IER &= ~IER_XMIT_EMPTY;
}


/*
 *  enable receive interrupt
 */
Inline void
uart_enable_rcv(DW_APB_UART_STRUCT_PTR regs)
{
	regs->IER |= IER_DATA_AVAIL;
}

/*
 *  disable receive interrupt
 */
Inline void
uart_disable_rcv(DW_APB_UART_STRUCT_PTR regs)
{
	regs->IER &= ~IER_DATA_AVAIL;
}


/*
 * init uart through portid 
 */ 
void emstartkit_uart_init(DW_APB_UART_STRUCT_PTR regs, uint32_t para, uint32_t baud) {
	
	regs->MCR = 0;
	regs->IIR = IIR_RCVR_FIFO_RESET | IIR_XMIT_FIFO_RESET;
	regs->IIR = IIR_FIFO_ENABLE;  
	regs->LCR = LCR_DLAB;
	/* DLL is shared with DATA */
	regs->DATA = UART_DLL(baud);
	/* DLH is shared with IER */
	regs->IER = UART_DLH(baud);
	regs->LCR = 0;
	regs->LCR = para & 0xff;
	regs->IER = 0;	/* no interrupt enable first */
}


/*
 *  the polling output of uart
 */
void
emstartkit_putc(DW_APB_UART_STRUCT_PTR regs, char_t c)
{
	while (!uart_putready(regs));
	uart_putchar(regs, c);
}

/*
 *  initialize sio driver
 */
void
sio_initialize(intptr_t exinf)
{
    SIOPCB  *p_siopcb;
    uint_t  i;

    /*
     *  initialize sio port control block
     */
    for (p_siopcb = siopcb_table, i = 0; i < TNUM_SIOP; p_siopcb++, i++) {
        p_siopcb->p_siopinib = &(siopinib_table[i]);
        p_siopcb->openflag = false;
        p_siopcb->sendflag = false;
    }
}


/*
 *  open sio port
 */
SIOPCB *
sio_opn_por(ID siopid, intptr_t exinf)
{
	SIOPCB  *p_siopcb = get_siopcb(siopid);
	
	p_siopcb->exinf = exinf;

	/*
	 *  the device dependent open
	*/
	emstartkit_uart_init(p_siopcb->p_siopinib->regs, 
			LCR_WORD_LEN8 | LCR_1_STOP_BIT | LCR_PARITY_NONE, 115200);
	p_siopcb->getready = p_siopcb->putready = false;
	p_siopcb->openflag = true;

	ena_int(p_siopcb->p_siopinib->intno);

	return p_siopcb;
}

/*
 *  close sio port
 */
void
sio_cls_por(SIOPCB *p_siopcb)
{
    /*
     * device dependent close
     */
	p_siopcb->openflag = false;
    
    /*
     * disable sio port interrupt
     */
	dis_int(p_siopcb->p_siopinib->intno);
}

/*
 * uart interrupt service routine
 */ 
static void dw_apb_uart_int_isr(SIOPCB *p_siopcb) {
    
	uint32_t status = 0;
	uint32_t lsr = 0;
	DW_APB_UART_STRUCT_PTR regs = p_siopcb->p_siopinib->regs;

	status = regs->IIR & IIR_INT_ID_MASK;

	switch (status) {
		case IIR_MDM_STATUS:
			break;
		case IIR_LINE_STATUS:
			/* overrun, parity error, framing error */
			lsr = regs->LSR;
			lsr = lsr;
			break;
		case IIR_XMIT_EMPTY:
			sio_irdy_snd(p_siopcb->exinf);
			break;
		case IIR_RX_TIMEOUT:
		case IIR_DATA_AVAIL:
			/* read all received data */
			do {
				sio_irdy_rcv(p_siopcb->exinf);
			}
			while (uart_getready(regs));
			break;
		default:
			break;
	}
} 

#
/*
 *  sio port interrupt handler
 */
void
sio_handler0(void)
{
	SIOPCB *p_siopcb = &(siopcb_table[0]);

	dw_apb_uart_int_isr(p_siopcb);
}


#if TNUM_SIOP > 1
void
sio_handler1(void)
{
	SIOPCB *p_siopcb = &(siopcb_table[1]);
	
	dw_apb_uart_int_isr(p_siopcb);
}
#endif

/*
 *  sio port send char
 */
bool_t
sio_snd_chr(SIOPCB *siopcb, char_t c)
{    
	if (uart_putready(siopcb->p_siopinib->regs)) {
		uart_putchar(siopcb->p_siopinib->regs, c);
		return true;
	} else {
		return false;
	}
}

/*
 *  sio port receive char
 */
int_t
sio_rcv_chr(SIOPCB *siopcb)
{
	return ((int_t)uart_getchar(siopcb->p_siopinib->regs));
}

/*
 *  enable callback routine of sio port
 */
void
sio_ena_cbr(SIOPCB *siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
        case SIO_RDY_SND:
            uart_enable_send(siopcb->p_siopinib->regs);
            break;
        case SIO_RDY_RCV:
            uart_enable_rcv(siopcb->p_siopinib->regs);
            break;
    }
}

/*
 *  disable callback routine of sio port
 */
void
sio_dis_cbr(SIOPCB *siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
        case SIO_RDY_SND:
            uart_disable_send(siopcb->p_siopinib->regs);
            break;
        case SIO_RDY_RCV:
            uart_disable_rcv(siopcb->p_siopinib->regs);
            break;
    }
}
