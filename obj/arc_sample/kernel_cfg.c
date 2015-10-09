/* kernel_cfg.c */
#include "kernel/kernel_int.h"
#include "kernel_cfg.h"

#ifndef TOPPERS_EMPTY_LABEL
#define TOPPERS_EMPTY_LABEL(x,y) x y[0]
#endif

#if TKERNEL_PRID != 0x07u
#error The kernel does not match this configuration file.
#endif

/*
 *  Include Directives (#include)
 */

#include "target_timer.h"
#include "syssvc/syslog.h"
#include "syssvc/banner.h"
#include "target_syssvc.h"
#include "target_serial.h"
#include "syssvc/serial.h"
#include "syssvc/logtask.h"
#include "sample1.h"

/*
 *  Default Definitions of Trace Log Macros
 */

#ifndef LOG_ISR_ENTER
#define LOG_ISR_ENTER(intno)
#endif /* LOG_ISR_ENTER */

#ifndef LOG_ISR_LEAVE
#define LOG_ISR_LEAVE(intno)
#endif /* LOG_ISR_LEAVE */

/*
 *  Task Management Functions
 */

const ID _kernel_tmax_tskid = (TMIN_TSKID + TNUM_TSKID - 1);

static STK_T _kernel_stack_LOGTASK[COUNT_STK_T(LOGTASK_STACK_SIZE)];
static STK_T _kernel_stack_TASK1[COUNT_STK_T(STACK_SIZE)];
static STK_T _kernel_stack_TASK2[COUNT_STK_T(STACK_SIZE)];
static STK_T _kernel_stack_TASK3[COUNT_STK_T(STACK_SIZE)];
static STK_T _kernel_stack_MAIN_TASK[COUNT_STK_T(STACK_SIZE)];

const TINIB _kernel_tinib_table[TNUM_TSKID] = {
	{ (TA_ACT), (intptr_t)(LOGTASK_PORTID), ((TASK)(logtask_main)), INT_PRIORITY(LOGTASK_PRIORITY), ROUND_STK_T(LOGTASK_STACK_SIZE), _kernel_stack_LOGTASK, (TA_NULL), (NULL) },
	{ (TA_NULL), (intptr_t)(1), ((TASK)(task)), INT_PRIORITY(MID_PRIORITY), ROUND_STK_T(STACK_SIZE), _kernel_stack_TASK1, (TA_NULL), (tex_routine) },
	{ (TA_NULL), (intptr_t)(2), ((TASK)(task)), INT_PRIORITY(MID_PRIORITY), ROUND_STK_T(STACK_SIZE), _kernel_stack_TASK2, (TA_NULL), (tex_routine) },
	{ (TA_NULL), (intptr_t)(3), ((TASK)(task)), INT_PRIORITY(MID_PRIORITY), ROUND_STK_T(STACK_SIZE), _kernel_stack_TASK3, (TA_NULL), (tex_routine) },
	{ (TA_ACT), (intptr_t)(0), ((TASK)(main_task)), INT_PRIORITY(MAIN_PRIORITY), ROUND_STK_T(STACK_SIZE), _kernel_stack_MAIN_TASK, (TA_NULL), (NULL) }
};

TCB _kernel_tcb_table[TNUM_TSKID];

const ID _kernel_torder_table[TNUM_TSKID] = {
	LOGTASK, TASK1, TASK2, TASK3, MAIN_TASK
};

/*
 *  Semaphore Functions
 */

const ID _kernel_tmax_semid = (TMIN_SEMID + TNUM_SEMID - 1);

const SEMINIB _kernel_seminib_table[TNUM_SEMID] = {
	{ (TA_TPRI), (0), (1) },
	{ (TA_TPRI), (1), (1) },
	{ (TA_TPRI), (0), (1) },
	{ (TA_TPRI), (1), (1) }
};

SEMCB _kernel_semcb_table[TNUM_SEMID];

/*
 *  Eventflag Functions
 */

const ID _kernel_tmax_flgid = (TMIN_FLGID + TNUM_FLGID - 1);

TOPPERS_EMPTY_LABEL(const FLGINIB, _kernel_flginib_table);
TOPPERS_EMPTY_LABEL(FLGCB, _kernel_flgcb_table);

/*
 *  Dataqueue Functions
 */

const ID _kernel_tmax_dtqid = (TMIN_DTQID + TNUM_DTQID - 1);

TOPPERS_EMPTY_LABEL(const DTQINIB, _kernel_dtqinib_table);
TOPPERS_EMPTY_LABEL(DTQCB, _kernel_dtqcb_table);

/*
 *  Priority Dataqueue Functions
 */

const ID _kernel_tmax_pdqid = (TMIN_PDQID + TNUM_PDQID - 1);

TOPPERS_EMPTY_LABEL(const PDQINIB, _kernel_pdqinib_table);
TOPPERS_EMPTY_LABEL(PDQCB, _kernel_pdqcb_table);

/*
 *  Mailbox Functions
 */

const ID _kernel_tmax_mbxid = (TMIN_MBXID + TNUM_MBXID - 1);

TOPPERS_EMPTY_LABEL(const MBXINIB, _kernel_mbxinib_table);
TOPPERS_EMPTY_LABEL(MBXCB, _kernel_mbxcb_table);

/*
 *  Fixed-sized Memorypool Functions
 */

const ID _kernel_tmax_mpfid = (TMIN_MPFID + TNUM_MPFID - 1);

TOPPERS_EMPTY_LABEL(const MPFINIB, _kernel_mpfinib_table);
TOPPERS_EMPTY_LABEL(MPFCB, _kernel_mpfcb_table);

/*
 *  Cyclic Handler Functions
 */

const ID _kernel_tmax_cycid = (TMIN_CYCID + TNUM_CYCID - 1);

const CYCINIB _kernel_cycinib_table[TNUM_CYCID] = {
	{ (TA_NULL), (intptr_t)(0), (cyclic_handler), (2000), (0) }
};

CYCCB _kernel_cyccb_table[TNUM_CYCID];

/*
 *  Alarm Handler Functions
 */

const ID _kernel_tmax_almid = (TMIN_ALMID + TNUM_ALMID - 1);

const ALMINIB _kernel_alminib_table[TNUM_ALMID] = {
	{ (TA_NULL), (intptr_t)(0), (alarm_handler) }
};

ALMCB _kernel_almcb_table[TNUM_ALMID];

/*
 *  Interrupt Management Functions
 */


#define TNUM_INHNO	3
const uint_t _kernel_tnum_inhno = TNUM_INHNO;

INTHDR_ENTRY(INHNO_TIMER, 16, target_timer_handler)
INTHDR_ENTRY(INHNO_SIO0, 23, sio_handler0)
INTHDR_ENTRY(INHNO_SIO1, 24, sio_handler1)

const INHINIB _kernel_inhinib_table[TNUM_INHNO] = {
	{ (INHNO_TIMER), (TA_NULL), (FP)(INT_ENTRY(INHNO_TIMER, target_timer_handler)) },
	{ (INHNO_SIO0), (TA_NULL), (FP)(INT_ENTRY(INHNO_SIO0, sio_handler0)) },
	{ (INHNO_SIO1), (TA_NULL), (FP)(INT_ENTRY(INHNO_SIO1, sio_handler1)) }
};

#define TNUM_INTNO	3
const uint_t _kernel_tnum_intno = TNUM_INTNO;

const INTINIB _kernel_intinib_table[TNUM_INTNO] = {
	{ (INTNO_TIMER), (TA_ENAINT|INTATR_TIMER), (INTPRI_TIMER) },
	{ (INTNO_SIO0), (INTATR_SIO0), (INTPRI_SIO0) },
	{ (INTNO_SIO1), (INTATR_SIO1), (INTPRI_SIO1) }
};

/*
 *  CPU Exception Management Functions
 */

/*
 *  Stack Area for Non-task Context
 */

#ifdef DEFAULT_ISTK

#define TOPPERS_ISTKSZ		DEFAULT_ISTKSZ
#define TOPPERS_ISTK		DEFAULT_ISTK

#else /* DEAULT_ISTK */

static STK_T				_kernel_istack[COUNT_STK_T(DEFAULT_ISTKSZ)];
#define TOPPERS_ISTKSZ		ROUND_STK_T(DEFAULT_ISTKSZ)
#define TOPPERS_ISTK		_kernel_istack

#endif /* DEAULT_ISTK */

const SIZE		_kernel_istksz = TOPPERS_ISTKSZ;
STK_T *const	_kernel_istk = TOPPERS_ISTK;

#ifdef TOPPERS_ISTKPT
STK_T *const	_kernel_istkpt = TOPPERS_ISTKPT(TOPPERS_ISTK, TOPPERS_ISTKSZ);
#endif /* TOPPERS_ISTKPT */

/*
 *  Time Event Management
 */

TMEVTN   _kernel_tmevt_heap[TNUM_TSKID + TNUM_CYCID + TNUM_ALMID];

/*
 *  Module Initialization Function
 */

void
_kernel_initialize_object(void)
{
	_kernel_initialize_task();
	_kernel_initialize_semaphore();
	_kernel_initialize_cyclic();
	_kernel_initialize_alarm();
	_kernel_initialize_interrupt();
	_kernel_initialize_exception();
}

/*
 *  Initialization Routine
 */

void
_kernel_call_inirtn(void)
{
	((INIRTN)(target_timer_initialize))((intptr_t)(0));
	((INIRTN)(syslog_initialize))((intptr_t)(0));
	((INIRTN)(print_banner))((intptr_t)(0));
	((INIRTN)(sio_initialize))((intptr_t)(0));
	((INIRTN)(serial_initialize))((intptr_t)(0));
}

/*
 *  Termination Routine
 */

void
_kernel_call_terrtn(void)
{
	((TERRTN)(logtask_terminate))((intptr_t)(0));
	((TERRTN)(target_timer_terminate))((intptr_t)(0));
}

/*
 *  Target-dependent Definitions (ARC)
 */


__attribute__ ((section(".vector"))) 
const FP _kernel_vector_table[] =      
 {                                    
	(FP)_start,                 // 0 The reset handler         
 	(FP)(_kernel_core_exc_entry), /* 1 */
 	(FP)(_kernel_core_exc_entry), /* 2 */
 	(FP)(_kernel_core_exc_entry), /* 3 */
 	(FP)(_kernel_core_exc_entry), /* 4 */
 	(FP)(_kernel_core_exc_entry), /* 5 */
 	(FP)(_kernel_core_exc_entry), /* 6 */
 	(FP)(_kernel_core_exc_entry), /* 7 */
 	(FP)(_kernel_core_exc_entry), /* 8 */
 	(FP)(_kernel_core_exc_entry), /* 9 */
 	(FP)(_kernel_core_exc_entry), /* 10 */
 	(FP)(_kernel_core_exc_entry), /* 11 */
 	(FP)(_kernel_core_exc_entry), /* 12 */
 	(FP)(_kernel_core_exc_entry), /* 13 */
 	(FP)(_kernel_core_exc_entry), /* 14 */
 	(FP)(_kernel_core_exc_entry), /* 15 */
 	(FP)(_kernel_core_int_entry), /* 16 */
 	(FP)(_kernel_core_int_entry), /* 17 */
 	(FP)(_kernel_core_int_entry), /* 18 */
 	(FP)(_kernel_core_int_entry), /* 19 */
 	(FP)(_kernel_core_int_entry), /* 20 */
 	(FP)(_kernel_core_int_entry), /* 21 */
 	(FP)(_kernel_core_int_entry), /* 22 */
 	(FP)(_kernel_core_int_entry), /* 23 */
 	(FP)(_kernel_core_int_entry), /* 24 */
 	(FP)(_kernel_core_int_entry), /* 25 */

};


const FP _kernel_exc_tbl[] = {
	(FP)_start,                 // 0 The reset handler         
 	(FP)(_kernel_default_exc_handler), /* 1 */
 	(FP)(_kernel_default_exc_handler), /* 2 */
 	(FP)(_kernel_default_exc_handler), /* 3 */
 	(FP)(_kernel_default_exc_handler), /* 4 */
 	(FP)(_kernel_default_exc_handler), /* 5 */
 	(FP)(_kernel_default_exc_handler), /* 6 */
 	(FP)(_kernel_default_exc_handler), /* 7 */
 	(FP)(_kernel_default_exc_handler), /* 8 */
 	(FP)(cpuexc_handler), /* 9 */
 	(FP)(_kernel_default_exc_handler), /* 10 */
 	(FP)(_kernel_default_exc_handler), /* 11 */
 	(FP)(_kernel_default_exc_handler), /* 12 */
 	(FP)(_kernel_default_exc_handler), /* 13 */
 	(FP)(_kernel_default_exc_handler), /* 14 */
 	(FP)(_kernel_default_exc_handler), /* 15 */
	(FP)(target_timer_handler), /* 16 */
	(FP)(_kernel_default_int_handler), /* 17 */
	(FP)(_kernel_default_int_handler), /* 18 */
	(FP)(_kernel_default_int_handler), /* 19 */
	(FP)(_kernel_default_int_handler), /* 20 */
	(FP)(_kernel_default_int_handler), /* 21 */
	(FP)(_kernel_default_int_handler), /* 22 */
	(FP)(sio_handler0), /* 23 */
	(FP)(sio_handler1), /* 24 */
	(FP)(_kernel_default_int_handler), /* 25 */

};



