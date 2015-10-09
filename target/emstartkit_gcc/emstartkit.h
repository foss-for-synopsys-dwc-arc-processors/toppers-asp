/* Copyright (C) 
 * 2014 - Wayne Ren
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


#ifndef TOPPERS_EMSTARTKIT_H
#define TOPPERS_EMSTARTKIT_H

#include <sil.h>

/*
 * the macro of getting bit pattern of IRT from intno
 */
#define INTNO_BITPAT(intno) (1U << intno)


#define CPU_CLOCK		25000000
#define PERIPHERAL_CLOCK	25000000

#define	REG_FILE_0		0xF0000000U
#define	REG_FILE_1		0xF0001000U	// not implemented on this board
#define	REGBASE_GPIO0		0xF0002000U
#define	REGBASE_GPIO1		0xF0003000U	// not implemented on this board
#define	REGBASE_I2C0		0xF0004000U
#define	REGBASE_I2C1		0xF0005000U	// not implemented on this board
#define REGBASE_SPI_MASTER	0xF0006000U	// SPI Master
#define REGBASE_SPI_SLAVE	0xF0007000U	// SPI Slave
#define	REGBASE_UART0		0xF0008000U // UART0 is connected to PMOD
#define	REGBASE_UART1		0xF0009000U	// UART1 is USB-UART£¬ use UART1 as default



#define INTNO_TIMER0		16
#define INTNO_TIMER1		17
#define INTNO_I2C0		19
#define INTNO_SPI_MASTER	21
#define INTNO_SPI_SLAVE		22
#define INTNO_UART0		23
#define INTNO_UART1		24

#ifndef TOPPERS_MACRO_ONLY

#endif /* TOPPPERS_MACRO_ONLY */


#endif /* TOPPERS_EMSTARTKIT_H */
