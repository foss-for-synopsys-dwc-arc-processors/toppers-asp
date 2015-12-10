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
