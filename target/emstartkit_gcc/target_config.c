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

/*
 * chip dependent module (for emstartkit)
 */

#include "kernel_impl.h"
#include <sil.h>
#include "target_serial.h"
#include <stdlib.h>

/*
 *  target dependent initialization
 */
void
target_initialize(void)
{
	/* in emstartkit, clock nand mem are already initialized when FPGA is configured */

	/*
	 *  to use target_fput_log, initialize UART
	 */
	emstartkit_uart_init((DW_APB_UART_STRUCT_PTR)REGBASE_UART1,
			LCR_WORD_LEN8 | LCR_1_STOP_BIT | LCR_PARITY_NONE, 115200);

	/*
	 *  core dependent initialization
	 */
	core_initialize();

}

/*
 *  target dependent exit processing
 */
void
target_exit(void)
{
	extern void    software_term_hook(void);
	void (*volatile fp)(void) = software_term_hook;

	/*
	 *  use volatile to omit optimization.
	 */
	if (fp != 0) {
		(*fp)();
	}

	/*
	 *  mask out all interrupts
	 */


	/*
	 *  core dependent termination processing
	 */
	core_terminate();

	/*
	 *  tool chain dependent termination processing
	 */
	while (1);
}

/*
 *  the low level output for syslog
 */
void
target_fput_log(char_t c)
{
	if (c == '\n') {
		emstartkit_putc((DW_APB_UART_STRUCT_PTR)REGBASE_UART1, '\r');
	}
	emstartkit_putc((DW_APB_UART_STRUCT_PTR)REGBASE_UART1, c);
}


/*
 *  the main entry for the initialization routine provided by the tool
 *  chain. sta_ker() will be called to start the kernel. 
 */
int
main(void)
{
	extern void software_init_hook(void);
	void (*volatile fp)(void) = software_init_hook;

	/*
	 *  call software_init_hook if not 0.
	 *
	 *  If it's necessary to do some software enviroment initialization,
	 *  such as libray, use software_init_hook.
	 *
	 *  use volatile to omit optimization.
	 */
	if (fp != 0) {
		(*fp)();
	}

	/*
	 * start kernel
	 */
	sta_ker();

	return 1;
}
