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
