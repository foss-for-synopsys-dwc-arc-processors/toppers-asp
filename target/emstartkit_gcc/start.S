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
 *   start up module for kernel (for arc)
 */
#define    TOPPERS_MACRO_ONLY
#define UINT_C(val)		(val)	/* the macro to make uint_t constant */
#define ULONG_C(val)	(val)	/* the macro to make ulong_t constant */
#include "kernel_impl.h"

	.text
	.align  4
	.global _start
_start: 
	kflag	0
/* init vector table */
	sr @_kernel_vector_table, [ARC_REG_INT_VECT_BASE]
	mov r0, 0
/* interrupt related init */
	sr	r0, [ARC_REG_IRQ_ACT]
	sr	r0, [ARC_REG_IRQ_CTRL]
	sr	r0, [ARC_REG_IRQ_HINT]
/* init gp,fp,sp */
	mov gp, @__SDATA_BEGIN__
	mov fp, 0
	ld sp, [@_kernel_istkpt]
	
	mov r0,	@hardware_init_hook
	cmp r0, 0
	jlne [r0]
#ifndef TOPPERS_OMIT_DATA_INIT
/* init .data section */
	mov r1, @__idata_start
	mov r3, @__idata_end
	cmp r1, r3
	jge start_3
	mov r2, @__data_start
start_2:
	ld.ab r0, [r1, 4]
	st.ab r0, [r2, 4]
	cmp r1, r3
	jle start_2
#endif
start_3:
/* init .bss section */ 
#ifndef TOPPERS_OMIT_BSS_INIT
	mov r1, @__bss_start
	mov r2, @__bss_end
	cmp r1, r2
	jge @start_5
	mov r0, 0
start_4:
	st.ab r0, [r1, 4]
	cmp r1,r2
	jle start_4
#endif
start_5:
/* jump to main function */
	jl	main 	
/* to be done */
