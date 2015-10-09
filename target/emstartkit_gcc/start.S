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
