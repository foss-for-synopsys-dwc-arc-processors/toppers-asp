/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
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
 *  $Id: itron.h 1762 2010-02-18 02:24:39Z ertl-hiro $
 */

/*
 *		 constants, and macros defined in ITRON specification
 *
 *  The common data types, constants, and macros defined in ITRON
 *  specification but not included in TOPPERS common head file are
 *  included in this head file. This head file is assumed to be included
 *  by applications that require to be compatible with ITRON
 *  specification.
 *
 *  When this head file is included in assembly language source file,
 *  the macro TOPPERS_MACRO_ONLY should be defined. Then, the
 *  definitions that are not marcos will be excluded.
 */

#ifndef TOPPERS_ITRON_H
#define TOPPERS_ITRON_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  TOPPERS common head file
 */
#include "t_stddef.h"

/*
 *  common data types of ITRON specification
 */
#ifndef TOPPERS_MACRO_ONLY

#ifdef INT8_MAX
typedef	int8_t			B;			/* 8-bit signed integer */
#endif /* INT8_MAX */

#ifdef UINT8_MAX
typedef	uint8_t			UB;			/* 8-bit unsigned integer */
typedef	uint8_t			VB;			/* 8-bit value with unknown data type */
#endif /* UINT8_MAX */

typedef	int16_t			H;			/* 16-bit signed integer */
typedef	uint16_t		UH;			/* 16-bit unsigned integer */
typedef	uint16_t		VH;			/* 16-bit value with unknown data type */

typedef	int32_t			W;			/* 32-bit signed integer */
typedef	uint32_t		UW;			/* 32-bit unsigned integer */
typedef	uint32_t		VW;			/* 32-bit value with unknown data type */

#ifdef INT64_MAX
typedef	int64_t			D;			/* 64-bit signed integer */
#endif /* INT64_MAX */

#ifdef UINT64_MAX
typedef	uint64_t		UD;			/* 64-bit unsigned integer */
typedef	uint64_t		VD;			/* 64-bit value with unknown data type */
#endif /* UINT64_MAX */

typedef	void			*VP;		/* pointer to an unknown data type */

typedef int_t			INT;		/* signed integer for the processor */
typedef uint_t			UINT;		/* unsigned integer for the processor*/

typedef bool_t			BOOL;		/* boolean */

typedef	intptr_t		VP_INT;		/* pointer to an unknown data type, or  a signed
									   integer for the processor */

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  common constants of ITRON specification 
 */
#define	TRUE		true			/* true */
#define	FALSE		false			/* false */

/*
 *  definitions of object attributes
 */
#define TA_HLNG			UINT_C(0x00)	/* high-level language interface */
#define TA_TFIFO		UINT_C(0x00)	/* FIFO queue of tasks in order */
#define TA_MFIFO		UINT_C(0x00)	/* FIFO order message queue */
#define TA_WSGL			UINT_C(0x00)	/* Only one task is waiting */

/*
 *  the maximum number of nested suspend
 */
#define TMAX_SUSCNT		UINT_C(1)		/* maximum number of nested suspend request */

/*
 * force to resume the task
 */
#define frsm_tsk(tskid)		rsm_tsk(tskid)

#ifdef __cplusplus
}
#endif

#endif /* TOPPERS_ITRON_H */
