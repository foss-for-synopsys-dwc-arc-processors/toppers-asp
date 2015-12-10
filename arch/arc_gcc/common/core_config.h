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
 *      core-dependent module (for arc)
 *
 *  this head file should not be included directly in the files except
 *  target_config.h (or the files included in target_config.h). 
 */

#ifndef TOPPERS_CORE_CONFIG_H
#define TOPPERS_CORE_CONFIG_H

/*
 *  arc common definitions
 */
#include "arc.h"

/*
 *  definitions of error check in pass3
 */
#define CHECK_STKSZ_ALIGN	8	/* the align of stack size */
#define CHECK_FUNC_ALIGN	4	/* the align of function address */
#define CHECK_FUNC_NONNULL		/* check whether the function address is NULL */
#define CHECK_STACK_ALIGN	4	/* the align of stack */
#define CHECK_STACK_NONNULL		/* check whether the stack is NULL */
#define CHECK_MPF_ALIGN		4	/* the align of fixed-sized memory pool */
#define CHECK_MPF_NONNULL		/* check whether the fixed-sized memory pool is NULL */
#define CHECK_MB_ALIGN		4	/* the align of management block */

#define IIPM_ENAALL		-1

/*
 *  the number of exceptions
 */  
#define TNUM_EXCH         16

#ifndef TOPPERS_MACRO_ONLY

/*
 ******************************************************************
 *      Inline ASM macros to read/write AUX Regs
 *      Essentially invocation of lr/sr insns from "C"
 */

#if 0

#define read_aux_reg(reg)	__builtin_arc_lr(reg)

/* gcc builtin sr needs reg param to be long immediate */
#define write_aux_reg(reg_immed, val)		\
		__builtin_arc_sr((unsigned int)val, reg_immed)

#else

#define read_aux_reg(reg)		\
({					\
	unsigned int __ret;		\
	__asm__ __volatile__(		\
	"	lr    %0, [%1]"		\
	: "=r"(__ret)			\
	: "i"(reg));			\
	__ret;				\
})

/*
 * Aux Reg address is specified as long immediate by caller
 * e.g.
 *    write_aux_reg(0x69, some_val);
 * This generates tightest code.
 */
#define write_aux_reg(reg_imm, val)	\
({					\
	__asm__ __volatile__(		\
	"	sr   %0, [%1]	\n"	\
	:				\
	: "ir"(val), "i"(reg_imm));	\
})

#endif


/*
 *  the initial value of non-task context stack
 */
#define TOPPERS_ISTKPT(istk, istksz) ((STK_T *)((char_t *)(istk) + (istksz)))


/*
 *  the definition of task context block
 */
typedef struct task_context_block {
    void  *sp;       /*  stack pointer */
    FP    pc;        /*  program counter */
} TSKCTXB;


/*
 *  the implementation of TOPPERS standard interrupt processing model
 *
 *  IRQ but not FIQ is managed by the kernel.
 *
 *  In the ARC core-dependent, the management of CPU lock 
 *  basic interrupt management  (STATUS32, AUX_REG_IRQ_* regs) are 
 *  implemented. Extension parts should to be implemented in the
 *  target-dependent part.
 * 
 */

/*
 *  context reference variable
 */
extern uint32_t excpt_nest_count; /* the nest count of exceptions (interrupt/CPU exception) */

/*
 *  refer the context
 *  
 */
Inline bool_t
sense_context(void)
{
    return (excpt_nest_count > 0U);
}

/*
 *  lock the CPU
 */
Inline void
x_lock_cpu(void)
{
	Asm("clri"); 
    /* data in memory maybe modified in critical section */
	Asm("":::"memory");
}

#define t_lock_cpu()   x_lock_cpu()
#define i_lock_cpu()   x_lock_cpu()

/*
 *  unlock the CPU
 */
Inline void
x_unlock_cpu(void)
{
    /* data in memory maybe modified in critical section */
	Asm("":::"memory");
	Asm("seti");
}

#define t_unlock_cpu() x_unlock_cpu()
#define i_unlock_cpu() x_unlock_cpu()

/*
 *  refer the CPU lock state
 */
Inline bool_t
x_sense_lock(void)
{
	uint32_t status = 0;

	status = read_aux_reg(ARC_REG_STATUS32);
	if (status & STATUS_IE_MASK) {
		return false;
	} else {
		return true;
	}
}

#define t_sense_lock()    x_sense_lock()
#define i_sense_lock()    x_sense_lock()




#define activate_context(p_tcb)                                         \
{                                                                       \
    (p_tcb)->tskctxb.sp = (void *)((char_t *)((p_tcb)->p_tinib->stk)    \
                                        + (p_tcb)->p_tinib->stksz);     \
    (p_tcb)->tskctxb.pc = (FP)start_r;                                  \
}

/*
 *  don't use calltex
 */
#define OMIT_CALLTEX
     
/*
 *  CPU exception handler table
 */
extern const FP exc_tbl[];

/*
 * don't use the standard exception initialization routine in the
 * target-independent part.
 */
#define OMIT_INITIALIZE_EXCEPTION

/*
 * CPU exception initialize
 */
Inline void
initialize_exception(void)
{
}

Inline void 
core_set_ipm(PRI intpri) {

	volatile uint32_t status;
	
	intpri = intpri - TMIN_INTPRI;

	status = read_aux_reg(ARC_REG_STATUS32);

	status = status | ((intpri << 1) & 0x1e);
	/* sr cannot write ARC_REG_STATUS32 */
	Asm("kflag %0"::"ir"(status));
}

Inline PRI
core_get_ipm(void) {
	
	uint32_t status;
	status = read_aux_reg(ARC_REG_STATUS32);
	status = (status >> 1) & 0xf;
	
	return (status + TMIN_INTPRI);
}


Inline bool_t
core_disable_int(INTNO intno)
{
	write_aux_reg(ARC_REG_IRQ_SELECT, intno);
	write_aux_reg(ARC_REG_IRQ_ENABLE, 0);
	
	return true;	
}

Inline bool_t
core_enable_int(INTNO intno)
{
	write_aux_reg(ARC_REG_IRQ_SELECT, intno);
	write_aux_reg(ARC_REG_IRQ_ENABLE, 1);
	
	return true;	
}


Inline bool_t
core_probe_int(INTNO intno)
{
	uint32_t pending;

	write_aux_reg(ARC_REG_IRQ_SELECT, intno);
	pending = read_aux_reg(ARC_REG_IRQ_PENDING);
	
	return (bool_t)pending;	
}

/* function declaration in core_support.S */
/* exception handler entry, in core_support.S */
extern void  core_exc_entry(void);
/* interrupt handler entry, in core_support.S */
extern void  core_int_entry(void); 

/* start entry, in start.S */
extern void _start(void);

/*
 *  task dispatcher
 */

/*
 *  dispatch to the task with the highest precedence (core_support.s)
 *  
 *  dispatch is called by the service calls executed in task context
 *  when CPU is locked, dispatch is enabled and interrupt priority mask
 *  is released. 
 */
extern void dispatch(void);

/*
 *  start dispatch (core_support.s)
 *
 *  start_dispatch is called at the start of the kernel when all
 *  interrupts are disabled.
 */
extern void start_dispatch(void) NoReturn;

/*
 *  exit and dispatch (core_support.s)
 *
 *  exit_and_dispatch is called by ext_tsk in task context
 *  when CPU is locked, dispatch is enabled and interrupt priority mask
 *  is released.
 */
extern void exit_and_dispatch(void) NoReturn;

/*
 *  kernel exit (core_support.s)
 *
 * 	call_exit_kernel is called when the kernel is terminated. It will
 * 	switch to the non-task context and call exit_kernel.
 */
extern void call_exit_kernel(void) NoReturn;

     
/*
 *  the initialization of task context
 *
 *  start_r is called when the task comes into the read state from
 *  dormant state. At this moment, the task stack is not used.
 *
 *  As TCB is not defined here, activate_context is implemented in macro
 *  but not inline function.
 */
extern void    start_r(void);


/*
 *
 *  core-dependent initialize
 */
extern void core_initialize(void);

/*
 *  core-dependent terminate
 */
extern void core_terminate(void);

extern void core_config_int(INTNO intno, ATR intatr, PRI intpri);
/*
 *  install a CPU exception handler into the CPU exception vector
 */ 
extern void x_install_exc(EXCNO excno, FP exchdr);
/*
 *  refer the status of system when CPU exceptions happened
 */
extern bool_t exc_sense_intmask(void *p_excinf);

/* default CPU exception handler */
extern void default_exc_handler(void *p_excinf);

/* default interrupt handler */
extern void default_int_handler(void *p_excinf);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_CORE_CONFIG_H */
