/* ------------------------------------------
 * Copyright (c) 2015, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */


/*
 *  serial IO device (SIO) driver (for emstartkit)
 */

#ifndef TOPPERS_TARGET_SERIAL_H
#define TOPPERS_TARGET_SERIAL_H

#include "emstartkit.h"


/*
 *  the number of sio port
 */
#define TNUM_SIOP	2	/* the number of supported sio port */

/*
 * the sio interrupt used by system log 
 */
#define INHNO_SIO0        INTNO_UART0  	/* handler no*/
#define INTNO_SIO0        INTNO_UART0   /* intno */
#define INTPRI_SIO0       -1		/* interrupt priority */
#define INTATR_SIO0       0U		/* interrupt attribute */

#define INHNO_SIO1        INTNO_UART1  	/* handler no*/
#define INTNO_SIO1        INTNO_UART1   /* intno */
#define INTPRI_SIO1       -1		/* interrupt priority */
#define INTATR_SIO1       0U		/* interrupt attribute */


/* DW APB UART bit definitions */

/* IER */
#define IER_DATA_AVAIL       (0x01)
#define IER_XMIT_EMPTY       (0x02)
#define IER_LINE_STATUS      (0x04)
#define IER_MDM_STATUS       (0x08)
#define IER_PTIME            (0x80)

/* IIR */
/* IIR READ */
#define IIR_IP               (0x01)
#define IIR_MASK             (0x0E)
#define IIR_READ_FIFO_ENABLE (0xC0)

/* Possible interrupt IIR_MASK values */
#define IIR_MDM_STATUS       (0x00)
#define IIR_XMIT_EMPTY       (0x02)
#define IIR_DATA_AVAIL       (0x04)
#define IIR_LINE_STATUS      (0x06)
#define IIR_RX_TIMEOUT       (0x0C)
#define IIR_INT_ID_MASK      (0x0f)

/* IIR WRITE */
#define IIR_FIFO_ENABLE      (0x01)
#define IIR_RCVR_FIFO_RESET  (0x02)
#define IIR_XMIT_FIFO_RESET  (0x04)
#define IIR_DMA_MODE_SELECT  (0x08)
#define IIR_RCV_TRIGGER_MASK (0xC0)

/* Values for IIR receive trigger */
#define IIR_TRIGGER_LEVEL_1_CHAR         (0x00)
#define IIR_TRIGGER_LEVEL_1_4_FULL       (0x40)
#define IIR_TRIGGER_LEVEL_1_2_FULL       (0x80)
#define IIR_TRIGGER_LEVEL_2_LESS_FULL    (0xC0)


/* MCR */
#define MCR_DTR              (0x01)
#define MCR_RTS              (0x02)
#define MCR_IRQ_ENABLE       (0x08)
#define MCR_LOOPBACK         (0x10)
#define MCR_AFCE             (0x20)
#define MCR_SIRE             (0x40)

/* LSR */
#define LSR_DR               (0x01)
#define LSR_OVERRUN          (0x02)
#define LSR_PARITYERR        (0x04)
#define LSR_FRAMEERR         (0x08)
#define LSR_BREAKRCVD        (0x10)
#define LSR_TXD_EMPTY        (0x20)
#define LSR_TX_STATUS        (0x40)
#define LSR_RX_FIFOERR       (0x80)

/* MSR */
#define MSR_DCTS             (0x01)
#define MSR_DDSR             (0x02)
#define MSR_TERI             (0x04)
#define MSR_DDCD             (0x08)
#define MSR_CTS              (0x10)
#define MSR_DSR              (0x20)
#define MSR_RIC              (0x40)
#define MSR_DCD              (0x80)

/* FCR */
#define FCR_FEN              (0x01)
#define FCR_RFR              (0x02)
#define FCR_TFR              (0x04)
#define FCR_DMS              (0x08)
#define FCR_RTL              (0xC0)

/* USR */
#define USR_BUSY             (0x01)
#define USR_TFNF             (0x02)
#define USR_TFE              (0x04)
#define USR_RFNE             (0x08)
#define USR_RFF              (0x10)

/* SFE */
#define SFE_SHADOW_FIFO_ENABLE          (0x01)

/* SRR */
#define SRR_UR               (0x01)
#define SRR_RFR              (0x02)
#define SRR_XFR              (0x04)

/* SRT */
#define SRT_TRIGGER_LEVEL_1_CHAR        (0x00)
#define SRT_TRIGGER_LEVEL_1_4_FULL      (0x01)
#define SRT_TRIGGER_LEVEL_1_2_FULL      (0x02)
#define SRT_TRIGGER_LEVEL_2_LESS_FULL   (0x03)

/* STET*/
#define STET_FIFO_EMPTY        (0x00)
#define STET_2_CHARS_IN_FIFO   (0x01)
#define STET_1_4_FULL          (0x02)
#define STET_1_2_FULL          (0x03)


#define UART_CONFIG   (LCR_1_STOP_BIT | \
		LCR_PARITY_NONE | \
		LCR_WORD_LEN8)


#define UART_CLOCK	50000000U

#define UART_BAUD(baud)	(UART_CLOCK / (baud *16))
#define UART_DLL(baud)  (UART_BAUD(baud) & 0xff)
#define UART_DLH(baud)  ((UART_BAUD(baud) >> 8) & 0xff)

/* LCR */
#define LCR_WORD_LEN_MASK    (0x03)
#define LCR_STOP_BIT_MASK    (0x04)
#define LCR_PARITY_MASK      (0x18)
#define LCR_STICK_PARITY     (0x20)
#define LCR_BREAK            (0x40)
#define LCR_DLAB             (0x80)

/* Word length values */
#define LCR_WORD_LEN5        (0x00)
#define LCR_WORD_LEN6        (0x01)
#define LCR_WORD_LEN7        (0x02)
#define LCR_WORD_LEN8        (0x03)

/* stop bit values */
#define LCR_1_STOP_BIT       (0x00)
#define LCR_2_STOP_BIT       (0x04)

/* Parity bit values */
#define LCR_PARITY_NONE      (0x00)
#define LCR_PARITY_ODD       (0x08)
#define LCR_PARITY_EVEN      (0x18)



#ifndef TOPPERS_MACRO_ONLY

/* designware uart regs */
typedef volatile struct dw_apb_uart_struct
{
     uint32_t DATA;                  /* data in/out and DLL */
     uint32_t IER;                   /* Interrupt enable register and DLH */
     uint32_t IIR;                   /* Interrupt Id register */
     uint32_t LCR;                   /* Line control Register */
     uint32_t MCR;                   /* Modem control register */
     uint32_t LSR;                   /* Line Status Register */
     uint32_t MSR;                   /* Modem status Register */
     uint32_t SCRATCHPAD;            /* Uart scratchpad register */
     uint32_t LPDLL;                 /* Low Power Divisor Latch (Low) Register */
     uint32_t LPDLH;                 /* Low Power Divisor Latch (High) Register */
     uint32_t RES1[2];               /* Reserved */
     uint32_t SHR[16];               /* Shadow data register */
     uint32_t FAR;                   /* FIFO Access register */
     uint32_t TFR;                   /* Transmit FIFO Read */
     uint32_t RFW;                   /* Receive FIFO write */
     uint32_t USR;                   /* UART status register */
     uint32_t TFL;                   /* Transmit FIFO level */
     uint32_t RFL;                   /* Receive FIFO level */
     uint32_t SRR;                   /* Software reset register */
     uint32_t SRTS;                  /* Shadow request to send */
     uint32_t SBCR;                  /* Shadow break control */
     uint32_t SDMAM;                 /* Shadow DMA mode */
     uint32_t SFE;                   /* Shadow FIFO enable */
     uint32_t SRT;                   /* Shadow RCVR Trigger */
     uint32_t STET;                  /* Shadow TX empty register */
     uint32_t HTX;                   /* Halt TX */
     uint32_t DMASA;                 /* DMA Software ACK */
     uint32_t RES2[18];              /* Reserved */
     uint32_t CPR;                   /* Camponent parameter register */
     uint32_t UCV;                   /* UART Component Version */
     uint32_t CTR;                   /* Component typw register */
} DW_APB_UART_STRUCT, *DW_APB_UART_STRUCT_PTR;

/*
 *  sio port control block
 */
typedef struct sio_port_control_block	SIOPCB;

/*
 *  the id no. of call back routine
 */
#define SIO_RDY_SND	1U		/* ready to send callback */
#define SIO_RDY_RCV	2U		/* ready to receive callback */
#define UART_PARA_OFFSET 24
#define UART_PARA_BAUD_MASK	0xfffff	/* baudrate mask, 20 bits */

/*
 * the initialization for the log output at kernel startup (in usart.h)
 */
extern void emstartkit_uart_init(DW_APB_UART_STRUCT_PTR regs, uint32_t para, uint32_t baud);

/*
 *  the polling output of uart
 */
extern void emstartkit_putc(DW_APB_UART_STRUCT_PTR regs, char_t c);

/*
 *  initialize sio driver
 */
extern void sio_initialize(intptr_t exinf);

/*
 *  open sio port
 */
extern SIOPCB *sio_opn_por(ID siopid, intptr_t exinf);

/*
 *  close sio port
 */
extern void sio_cls_por(SIOPCB *p_siopcb);

/*
 *  sio port interrupt handler
 */
extern void sio_handler(void);

/*
 *  sio port send char
 */
extern bool_t sio_snd_chr(SIOPCB *siopcb, char_t c);

/*
 *  sio port receive char
 */
extern int_t sio_rcv_chr(SIOPCB *siopcb);

/*
 *  enable callback routine of sio port
 */
extern void sio_ena_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  disable callback routine of sio port
 */
extern void sio_dis_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  ready to send callback in interrupt
 */
extern void sio_irdy_snd(intptr_t exinf);

/*
 *  ready to receive callback in interrupt
 */
extern void sio_irdy_rcv(intptr_t exinf);


#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_TARGET_SERIAL_H */
