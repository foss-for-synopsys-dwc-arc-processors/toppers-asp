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
 * core-dependent part in assemble language (for arc)
 */

#define TOPPERS_MACRO_ONLY
#define TOPPERS_ASM_MACRO
#define UINT_C(val)		(val)		/* macro to create uint_t constant */
#define ULONG_C(val)	(val)		/* macro to create ulong_t constant */
#include "kernel_impl.h"
#include "offset.h"
#include "arc_gcc/common/arc.h"

/* Note on the LD/ST addr modes with addr reg wback
 *
 * LD.a same as LD.aw
 *
 * LD.a    reg1, [reg2, x]  => Pre Incr
 *      Eff Addr for load = [reg2 + x]
 *
 * LD.ab   reg1, [reg2, x]  => Post Incr
 *      Eff Addr for load = [reg2]
 */

.macro PUSH reg
	st.a	\reg, [sp, -4]
.endm

.macro PUSHAX aux
	lr	r9, [\aux]
	PUSH	r9
.endm

.macro POP reg
	ld.ab	\reg, [sp, 4]
.endm

.macro POPAX aux
	POP	r9
	sr	r9, [\aux]
.endm

/*--------------------------------------------------------------
 * Helpers to save/restore callee-saved regs:
 * used by several macros below
 *-------------------------------------------------------------*/
.macro SAVE_CALLEE_REGS
	PUSH	r13
	PUSH	r14
	PUSH	r15
	PUSH	r16
	PUSH	r17
	PUSH	r18
	PUSH	r19
	PUSH	r20
	PUSH	r21
	PUSH	r22
	PUSH	r23
	PUSH	r24
	PUSH	r25
.endm

.macro RESTORE_CALLEE_REGS
	POP	r25
	POP	r24
	POP	r23
	POP	r22
	POP	r21
	POP	r20
	POP	r19
	POP	r18
	POP	r17
	POP	r16
	POP	r15
	POP	r14
	POP	r13
.endm

.macro SAVE_LP_REGS
	PUSHAX	ARC_REG_LP_START
	PUSHAX	ARC_REG_LP_END
	PUSH	r60
.endm

.macro RESTORE_LP_REGS
	POP	r9
/* must not use the LP_COUNT register(r60) as the destination of multi-cycle instruction */		
	mov	r60, r9
	POPAX	ARC_REG_LP_END
	POPAX	ARC_REG_LP_START
.endm

.macro SAVE_R0_TO_R12
	PUSH	r0
	PUSH	r1
	PUSH	r2
	PUSH	r3
	PUSH	r4
	PUSH	r5
	PUSH	r6
	PUSH	r7
	PUSH	r8
	PUSH	r9
	PUSH	r10
	PUSH	r11
	PUSH	r12
.endm

.macro RESTORE_R0_TO_R12
	POP	r12
	POP	r11
	POP	r10
	POP	r9
	POP	r8
	POP	r7
	POP	r6
	POP	r5
	POP	r4
	POP	r3
	POP	r2
	POP	r1
	POP	r0
.endm

.macro SAVE_CODE_DENSITY
	PUSHAX	ARC_REG_JLI_BASE
	PUSHAX	ARC_REG_LDI_BASE
	PUSHAX	ARC_REG_EI_BASE
.endm

.macro RESTORE_CODE_DENSITY
	POPAX	ARC_REG_EI_BASE
	POPAX	ARC_REG_LDI_BASE
	POPAX	ARC_REG_JLI_BASE
.endm



/* todo: check the contents of NON_SCRATCH_REGS in debug */
.macro SAVE_NONSCRATCH_REGS
/* r0-r12 are saved by caller function */
/* some bits in status32 should be saved */
	;PUSH	ARC_REG_STATUS32
	PUSH	r26
	PUSH	r27
/* r28 is sp will be stored in task control block */
	;PUSH	r29	/* ilink reg */
	;PUSH	r30	/* general purpose */
	PUSH	blink
#ifdef USE_CODE_DENSITY	/* complier won't use these regs here */
	;SAVE_CODE_DENSITY
#endif
	;SAVE_LP_REGS
	SAVE_CALLEE_REGS
.endm

.macro RESTORE_NONSCRATCH_REGS
	RESTORE_CALLEE_REGS
	;RESTORE_LP_REGS
#ifdef	USE_CODE_DENSITY
	;RESTORE_CODE_DENSITY
#endif
	POP	blink
	;POP	r30
	;POP	r29
	POP	r27
	POP	r26
/* some bits in status32 should be restored */
	;POP	r9	/* status32 cannot be written by sr */
	;kflag	r9
.endm

.macro SAVE_INT_EXC_REGS
	SAVE_R0_TO_R12
	PUSH	r26
	PUSH	r27
/* r28 is sp will be stored later */
	PUSH	r29	/* ilink reg */
	PUSH	r30	/* general purpose */
	PUSH	blink

#ifdef USE_CODE_DENSITY
	SAVE_CODE_DENSITY
#endif
	SAVE_LP_REGS
.endm

.macro RESTORE_INT_EXC_REGS
	RESTORE_LP_REGS
#ifdef	USE_CODE_DENSITY
	RESTORE_CODE_DENSITY
#endif
	POP	blink
	POP	r30
	POP	r29
	POP	r27
	POP	r26

	RESTORE_R0_TO_R12
.endm


/*
 *  task dispatcher
 *
 */
	.text
	.align 4
	.global dispatch
dispatch:
/*  
 *  the pre-conditions of this routine are task context, CPU is
 *  locked, dispatch is enabled, and all interrupt priorities are
 *  unmasked.
 *		
 */
	SAVE_NONSCRATCH_REGS		/* save callee save registers */
	ld	r0, [@p_runtsk]
	st	sp, [r0, TCB_sp]	/* save stack pointer */
	mov	r1, @dispatch_r
	st	r1, [r0, TCB_pc]	/* save return address */
	b	dispatcher

/* return routine when task dispatch happend in task context */
dispatch_r:
	RESTORE_NONSCRATCH_REGS		/* recover registers */
	/*
	 * whether to call the task exception routine
	 * as it is jumped from dispatch, TCB is already in r1
	 */
	ldb	r0, [r1, TCB_enatex]
	/* whether task exception is enabled */
	bbit0	r0, TCB_enatex_mask, dispatch_r_1	
	ld	r0, [r1,TCB_texptn]	/* if texptn is 0, return */
	cmp	r0, 0                
	beq	dispatch_r_1          
	ld	r1, [@ipmflg]		/* if ipmflg is false, return */
	btst	r1,r1
	bne	call_texrtn		/* call the task exception routine */
dispatch_r_1:
	j	[blink]


/*
 *  start dispatch (cpu_support.S)
 */
	.global start_dispatch
	.align 4
start_dispatch:
/*  
 *  this routine is called in the non-task conext during the startup of the kernel
 *  , and all the interrupts are locked.
 *
 *  when the dispatcher is called, the cpu is locked, all interrupt
 *  priority mask are cleared, no nest exception (CPU exception/interrupt).
 *  In target_initialize, all interrupt priority mask should be cleared, cpu should be
 *  locked, the interrupts outside the kernel such as fiq can be
 *  enabled. As the disdsp is initialized to be false, task dispatch
 *  is enabled.  
 */    
	clri
	mov	r0, 0                           
	st	r0, [@excpt_nest_count]
	b	dispatcher_0

/*
 *  exit and dispatch
 */
	.global exit_and_dispatch
	.align 4
exit_and_dispatch:
/*  go to dispatcher */    

/*
 *  dispatcher
 */
dispatcher:
/*
 *  before dispatcher is called, task context | cpu locked | dispatch enabled | all interrupt priority masks are cleared 
 *  should be satisfied. In this routine, the processor will jump
 *  into the entry of next to run task
 *
 *  i.e. kernel mode, IRQ disabled, disdsp is false, dspflg is true.
 */
#ifdef LOG_DSP_ENTER
	ld	r0, [@p_runtsk]        
	bl	log_dsp_enter
#endif /* LOG_DSP_ENTER */
dispatcher_0:
	ld	r1, [@p_schedtsk]  
	st	r1, [@p_runtsk]        
	cmp	r1, 0		/* if p_runtsk is NULL, jump to dispatcher_1 */
	beq	dispatcher_1             
	ld	sp, [r1, TCB_sp]	/* recover task stack */
#ifdef LOG_DSP_LEAVE
	mov	r0, r1		/* p_runtsk is the parameter of log_dsp_leave */
	PUSH	r1		/* as r1 is scratch register, is used later, save it */
	bl	log_dsp_leave
	POP	r1
#endif /* LOG_DSP_LEAVE */
	ld	r0, [r1, TCB_pc]	/* get return address  */    
	j	[r0]
/* when p_runtsk is NULL */
dispatcher_1:
/*
 * unlock the CPU, and be ready to switch to non-task context
 */
/* switch to non-task context */
	ld	sp, [@_kernel_istkpt]    
dispatcher_2:
/*
*  enable all interrupts, run in non-task context to wait for
*  interrupts.
*  
*  the switch to no-task context one hand is to solve the problem
*  of which stack the interrupt processing should use, the other
*  hand is to prevent task dispatch in the interrupt handlers.  
*  
*  the processings of enable interrupts and wait for interrupts
*  must be atomic. If not, at the moment that interrupts are
*  enabled, the interrupts may come out, no matter whether a task
*  dispatch request exists, the processor will wait for interrupts 
*  after return. So even if a task dispatch request exists, no real
*  task dispatch will happen.
*
*  when the processor is waiting for interrupts, p_runtsk must be
*  NULL (=0). If not, when iget_tid is called in interrupt
*  handlers, there is a conflict.
*
*  dependent on target, the processor may get into low power mode
*  when it is waiting for interrupts. If TOPPERS_CUSTOM_IDLE is
*  defined,  the assemble macro, toppers_asm_custom_idle, defined
*  by the target will be called. In this macro, r0, r1, r2, r3, sp
*  cannot be used before stored in stack.
*
*/
	mov	r1, 1
	st   r1, [@excpt_nest_count]	/* excpt_nest_count = 1 */

#ifdef TOPPERS_CUSTOM_IDLE
	toppers_asm_custom_idle	/* in this macro, interrupt should be enabled */
#else
	seti		/* enable interrupt */
	sleep
	clri		/* disable interrupt */
#endif /* TOPPERS_CUSTOM_IDLE */
	ld	r4, [@reqflg]        /* if reqflg is false, jump to dispatcher_2 */
	cmp	r4, 0          
	beq	dispatcher_2
	mov	r0, 0                 
	st	r0, [@reqflg]        /* reqflg = 0 */
	st	r0, [@excpt_nest_count]        /* excpt_nest_count = 0 */
	b	dispatcher_0


/*
 *  kernel exit routine
 *
 *  switch to the mode and stack of no-task context
 */
	.global call_exit_kernel
	.align 4
call_exit_kernel:
	clri	/* lock cpu */ 
	ld	sp, [@_kernel_istkpt]
	j	exit_kernel


/*
 *  task startup routine
 *
 *  As this routine is called in dispatcher¡¤TCB address is in r1 (see dispatcher_0)
 */
	.text
	.global start_r
	.align 4
start_r:
	seti	/* unlock cpu */
	mov	blink, @ext_tsk		/* set return address */
	ld	r2, [r1, TCB_p_tinib]	/* load p_runtsk->p_tinib into r2  */
	ld	r0, [r2, TINIB_exinf]	/* load exinf into r0  */
	ld	r1, [r2, TINIB_task]	/* jump to task entry */
	j	[r1] 


	.global core_exc_entry	/* entry for exception handling */
	.align 4
core_exc_entry:
	/* interrups are disabled in exception */
	SAVE_INT_EXC_REGS

/* save ARC_REG for exception */
	PUSHAX	ARC_REG_ERSTATUS	/* save status32 */
	PUSHAX	ARC_REG_ERRET		/* save return address */
	PUSHAX	ARC_REG_ERBTA
	mov	blink,	sp
	mov	r3, sp			/* as exception handler's 2nd para */

	ld	r0, [@excpt_nest_count]
	add	r1, r0, 1
	st	r1, [@excpt_nest_count]
	cmp	r0, 0
	bne	exc_handler_1
/* chang to interrupt stack if interrupt happened in task context */
	ld	sp, [@_kernel_istkpt]
exc_handler_1:
	PUSH	blink

	lr	r0, [ARC_REG_ECR]
	lsr	r0, r0, 16
	mov	r1, @_kernel_exc_tbl
	ld.as	r2, [r1, r0]
	
	PUSH	r0
#ifdef	LOG_EXC_ENTER
	PUSH	r3
	bl	log_exc_enter
	POP	r3
#endif	/* LOG_EXC_ENTER */
	PUSHAX	ARC_REG_STATUS32
/* recover original system status */
	lr	r1, [ARC_REG_ERSTATUS]
	kflag r1
	mov	r0, r3
	jl	[r2]		/* jump to exception handler */
	
	POP	r0
#ifdef LOG_EXC_LEAVE
	bl	log_exc_leave
#endif /* lOG_EXC_LEAVE */
/* interrupts are not allowed */
ret_exc:
	POP	r0
	kflag	r0
	POP	sp
	mov	r1, @excpt_nest_count
	ld	r0, [r1]
	sub	r0, r0, 1
	cmp	r0, 0
	bne.d	ret_exc_1
	st	r0, [r1]

	ld	r0, [@reqflg]
	cmp	r0, 0
	bne	ret_exc_2
ret_exc_1:	/* return from non-task context */
	POPAX	ARC_REG_ERBTA
	POPAX	ARC_REG_ERRET
	POPAX	ARC_REG_ERSTATUS
	RESTORE_INT_EXC_REGS
	rtie
/* there is a dispatch request */
ret_exc_2:
	/* clear dispatch requst */
	mov	r0, 0
	st	r0, [@reqflg]

	ld	r1, [@p_runtsk]
	ld	r0, [@dspflg]
	cmp	r0, 0
	beq	ret_exc_r_1
	ld	r2, [@p_schedtsk]
	cmp	r1, r2
	beq	ret_exc_r_1
	
	SAVE_CALLEE_REGS		/* save callee save registers */
	st	sp, [r1, TCB_sp]	/* save stack pointer */
	mov	r0, @ret_exc_r
	st	r0, [r1, TCB_pc]	/* save return address */
	b	dispatcher

ret_exc_r:
	RESTORE_CALLEE_REGS		/* recover registers */
ret_exc_r_1:
	/*
	 * whether to call the task exception routine
	 * as it is jumped from dispatch, TCB is already in r1
	 */
	ldb	r0, [r1, TCB_enatex]
	bbit0	r0, TCB_enatex_bit, ret_exc_r_2	/* whether task exception is enabled */
	ld	r0, [r1,TCB_texptn]	/* if texptn is 0, return */
	cmp	r0, 0
	beq	ret_exc_r_2          
	ld	r0, [@ipmflg]           /* if ipmflg is false, return */
	cmp	r0, 0
	blne	call_texrtn           /* call the task exception routine */
ret_exc_r_2:
	POPAX	ARC_REG_ERBTA
	POPAX	ARC_REG_ERRET
	POPAX	ARC_REG_ERSTATUS
	RESTORE_INT_EXC_REGS
	rtie


/* the entry of interrupt */
	.global core_int_entry	/* entry for interrupt handling */
	.align 4
core_int_entry:
	clri	/* disable interrupt */
	/* status32 and PC are stored in stack */
	SAVE_INT_EXC_REGS	/* save scratch regs, this will be affected  */
	mov	blink, sp

	ld	r3, [@excpt_nest_count]
	add	r2, r3, 1
	st	r2, [@excpt_nest_count]
	cmp	r3, 0
	bne	irq_handler_1
/* chang to interrupt stack if interrupt happened in task context */
	ld	sp, [@_kernel_istkpt]
irq_handler_1:
	PUSH	blink

	lr	r0, [ARC_REG_IRQ_CAUSE]
	sr	r0, [ARC_REG_IRQ_SELECT]
	lr	r3, [ARC_REG_IRQ_PRIORITY]
	mov	r1, @_kernel_exc_tbl
	ld.as	r2, [r1, r0]	/* r2 = _kernel_exc_tbl + irqno *4 */
	lr	r4, [ARC_REG_IRQ_HINT]
	cmp	r4, r0
	bne.d irq_hint_handled
	xor	r4, r4, r4
	sr	r4, [ARC_REG_IRQ_HINT]
irq_hint_handled:
	PUSH	r3	/* save irq priority */
	PUSH	r0
#ifdef LOG_INH_ENTER
	PUSH	r2
	bl	log_inh_enter
	POP	r2
#endif	/* LOG_INH_ENTER */
	seti	/* enable higher priority interrupt */
	jl	[r2]		/* jump to interrupt handler */
	POP	r0
#ifdef	LOG_INH_LEAVE
	bl	log_inh_leave
#endif
/* no interrupts are allowed from here */
ret_int:
	clri	/* disable interrupt */
	POP	r3	/* irq priority */
	POP	sp
	mov	r1, @excpt_nest_count
	ld	r0, [r1]
	sub	r0, r0, 1
	cmp	r0, 0
	bne.d	ret_int_1
	st	r0, [r1]

	ld	r0, [@reqflg]
	cmp	r0, 0
	bne	ret_int_2
ret_int_1:	/* return from non-task context */
	RESTORE_INT_EXC_REGS
	rtie
/* there is a dispatch request */
ret_int_2:
	/* clear dispatch requst */
	mov	r0, 0
	st	r0, [@reqflg]

	ld	r1, [@p_runtsk]
	ld	r0, [@dspflg]
	cmp	r0, 0
	beq	ret_int_r_1
	ld	r2, [@p_schedtsk]
	cmp	r1, r2
	beq	ret_int_r_1

	lr	r9, [ARC_REG_IRQ_ACT]
	PUSH	r9
	bclr	r9, r9, r3		/* clear realated bits in IRQ_ACT */
	sr	r9, [ARC_REG_IRQ_ACT]				
	SAVE_CALLEE_REGS		/* save callee save registers */
	st	sp, [r1, TCB_sp]	/* save stack pointer */
	mov	r0, @ret_int_r
	st	r0, [r1, TCB_pc]	/* save return address */
	b	dispatcher

ret_int_r:
	RESTORE_CALLEE_REGS		/* recover registers */
ret_int_r_1:
	/*
	 * whether to call the task exception routine
	 * as it is jumped from dispatch, TCB is already in r1
	 */
	ldb	r0, [r1, TCB_enatex]
	bbit0	r0, TCB_enatex_bit, ret_int_r_2	/* whether task exception is enabled */
	ld	r0, [r1,TCB_texptn]	/* if texptn is 0, return */
	cmp	r0, 0
	beq	ret_int_r_2          
	ld	r0, [@ipmflg]           /* if ipmflg is false, return */
	cmp	r0, 0
	jlne	call_texrtn           /* call the task exception routine */
ret_int_r_2:
	POPAX	ARC_REG_IRQ_ACT
	RESTORE_INT_EXC_REGS
	rtie

/*
 *  delay in nano seconds
 */
	.global _sil_dly_nse
	.align	4
_sil_dly_nse:
	sub	r0, r0, SIL_DLY_TIM1
	cmp	r0, 0
	bgt	_sil_dly_nse1
	j	[blink]
_sil_dly_nse1:
	sub	r0, r0, SIL_DLY_TIM2
	cmp	r0, 0
	bgt	_sil_dly_nse1
	j	[blink]
