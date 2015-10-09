/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2010 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *
 *	The above copyright holders grant permission gratis to use,
 *	duplicate, modify, or redistribute (hereafter called use) this
 *	software (including the one made by modifying this software),
 *	provided that the following four conditions (1) through (4) are
 *	satisfied.
 *
 *	(1) When this software is used in the form of source code, the above
 *    	copyright notice, this use conditions, and the disclaimer shown
 *    	below must be retained in the source code without modification.
 *
 *	(2) When this software is redistributed in the forms usable for the
 *    	development of other software, such as in library form, the above
 *    	copyright notice, this use conditions, and the disclaimer shown
 *    	below must be shown without modification in the document provided
 *    	with the redistributed software, such as the user manual.
 *
 *	(3) When this software is redistributed in the forms unusable for the
 *    	development of other software, such as the case when the software
 *    	is embedded in a piece of equipment, either of the following two
 *   	 conditions must be satisfied:
 *
 *  	(a) The above copyright notice, this use conditions, and the
 *         	disclaimer shown below must be shown without modification in
 *     		the document provided with the redistributed software, such as
 *      	the user manual.
 *
 * 		(b) How the software is to be redistributed must be reported to the
 *     		TOPPERS Project according to the procedure described
 *     		separately.
 *
 *	(4) The above copyright holders and the TOPPERS Project are exempt
 *    	from responsibility for any type of damage directly or indirectly
 *   	caused from the use of this software and are indemnified by any
 *    	users or end users of this software from any and all causes of
 *    	action whatsoever.
 *
 *	THIS SOFTWARE IS PROVIDED "AS IS." THE ABOVE COPYRIGHT HOLDERS AND
 *	THE TOPPERS PROJECT DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES,
 *	INCLUDING, BUT NOT LIMITED TO, ITS APPLICABILITY TO A PARTICULAR
 *	PURPOSE. IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS AND THE
 *	TOPPERS PROJECT BE LIABLE FOR ANY TYPE OF DAMAGE DIRECTLY OR
 *	INDIRECTLY CAUSED FROM THE USE OF THIS SOFTWARE.
 *
 *  @(#) $Id: kernel_impl.h 1966 2010-11-20 07:23:56Z ertl-hiro $
 */

/*
 *		TOPPERS/ASP kernel internal headfile
 *
 * 	this header file should be included in the src files composing the kernel
 *
 *  when included in assemble file, the macro TOPPERS_MACRO_ONLY should be defined
 *  , then the parts in
 *  		#ifndef TOPPERS_MACRO_ONLY
 *  		....
 *  		#endif
 *  will not be included
 *
 */

#ifndef TOPPERS_KERNEL_IMPL_H
#define TOPPERS_KERNEL_IMPL_H

/*
 *  kernel function rename
 */
#include "kernel_rename.h"

/*
 *  common header file shared with application
 */
#include <kernel.h>

/*
 *  include system log function
 */
#include <t_syslog.h>

/*
 *  data type cast
 */
#ifndef CAST
#define CAST(type, val)		((type)(val))
#endif /* CAST */

/*
 *  target-dependent parts
 */
#include "target_config.h"

/*
 *  compile all functions
 */
#ifdef ALLFUNC
#include "allfunc.h"
#endif /* ALLFUNC */

/*
 *  data type for 8-bits bit field.
 * 
 *  the unsigned int data type for 8-bits bit field. As for ANSI C, the data
 *  type for bit field must be int or unsigned int, the default data
 *  type here is unsigned int. Dependent on target, using unsigned char or
 *  unsined short may be more effective.
 *
 */
#ifndef BIT_FIELD_UINT
#define BIT_FIELD_UINT		unsigned int
#endif /* BIT_FIELD_UINT */

/*
 *  data type for 1-bit  bit field
 *
 *  the data type for 1-bit bit field which is used as a bool value.
 *  The default data type is the same as BIT_FIELD_UNIT, using a
 *  different name to show it is a bool value. 
 */
#ifndef BIT_FIELD_BOOL
#define BIT_FIELD_BOOL		BIT_FIELD_UINT
#endif /* BIT_FIELD_BOOL */

/*
 *  the min value of object id
 */
#define TMIN_TSKID		1		/* minimum value of task ID */
#define TMIN_SEMID		1		/* minimum value of semaphore ID*/
#define TMIN_FLGID		1		/* minimum value of eventflag ID */
#define TMIN_DTQID		1		/* minimum value of the data queue ID */
#define TMIN_PDQID		1		/* minimum value of the data queue priority ID */
#define TMIN_MBXID		1		/* minimum value of the mailbox ID */
#define TMIN_MPFID		1		/* minimum value of fixed-size memory pool ID*/
#define TMIN_CYCID		1		/* minimum value of the cyclic handler ID */
#define TMIN_ALMID		1		/* minimum value of the alarm handler ID */
#define TMIN_ISRID		1		/* minimum value of the service routine interrupt ID */

/*
 *  number of priority
 */
#define TNUM_TPRI		(TMAX_TPRI - TMIN_TPRI + 1)
#define TNUM_MPRI		(TMAX_MPRI - TMIN_MPRI + 1)
#define TNUM_INTPRI		(TMAX_INTPRI - TMIN_INTPRI + 1)

/*
 *  attributes to be used internally by the kernel
 */
#define TA_NOEXS		((ATR)(-1))			/* unregister state*/

#ifndef TA_MEMALLOC
#define TA_MEMALLOC		UINT_C(0x8000)		/* secure in the kernel memory area */
#endif /* TA_MEMALLOC */
#ifndef TA_MBALLOC
#define TA_MBALLOC		UINT_C(0x4000)		/* management area in the kernel */
#endif /* TA_MBALLOC */

/*
 *  macro definition for checking default value error of the target definition
 */
#ifndef TARGET_TSKATR
#define TARGET_TSKATR		0U		/* task attributes definition */
#endif /* TARGET_TSKATR */

#ifndef TARGET_ISRATR
#define TARGET_ISRATR		0U		/* ISR attribute definition */
#endif /* TARGET_ISRATR */

/*
 *  function declarations that should not be
 *  included in assemble files
 */
#ifndef TOPPERS_MACRO_ONLY

/*
 *  objects initialization (see kernel_cfg.c)
 */
extern void	initialize_object(void);

/*
 *  call init routine defined in static api (see kernel_cfg.c)
 */
extern void	call_inirtn(void);

/*
 *  call terminate routine defined in static api (see kernel_cfg.c)
 */
extern void	call_terrtn(void);

/*
 *  non-task context stack area (see kernel_cfg.c)
 */
extern const SIZE	istksz;		/* size of non-task context stack(round value) */
extern STK_T *const	istk;		/* pointer to no-task context stack area */
#ifdef TOPPERS_ISTKPT
extern STK_T *const	istkpt;		/* init value of stack pointer */
#endif /* TOPPERS_ISTKPT */

/*
 *   memory space will be allocated by kernel ¡Êkernel_cfg.c¡Ë
 */
extern const SIZE	kmmsz;		/* the size of the memory region */
extern MB_T *const	kmm;		/* the start address of memory region*/

/*
 *  kernel flag(true:kernel is on, false: kernel is off) (see startup.c)
 */
extern bool_t	kerflg;

/*
 *  start kernel (see startup.c)
 */
extern void	sta_ker(void);

/*
 *  exit kernel (see startup.c)
 */
extern void	exit_kernel(void);

/*
 *  space management to allocate kernel memory(startup.c)
 */
extern void initialize_kmm(void);
extern void *kernel_malloc(SIZE size);
extern void kernel_free(void *ptr);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_KERNEL_IMPL_H */
