/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: t_stddef.h 1513 2009-04-22 07:35:20Z ertl-hiro $
 */

/*
 *		TOPPERS common head file
 *
 *  This head file should be included all the source files related to
 *  TOPPERS, including kernels and applications. The common data types,
 *  constants and macros of TOPPERS are included in this head file.
 *
 *  When this head file is included in assembly language source file,
 *  the macro TOPPERS_MACRO_ONLY should be defined. Then, the
 *  definitions that are not Marcos will be excluded.
 *
 *  No files are required to be included before this head file.
 */

#ifndef TOPPERS_T_STDDEF_H
#define TOPPERS_T_STDDEF_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  target-dependent part
 */
#include "target_stddef.h"

/*
 *  throw definitions of throw for C and EC++
 */
#if !defined(__cplusplus) || defined(__embedded_cplusplus)
#define throw()
#endif

#ifndef TOPPERS_MACRO_ONLY

/*
 *  the default target-dependent definitions of data types and macros
 */
#ifndef TOPPERS_bool
#define TOPPERS_bool	int				/* boolean */
#endif /* TOPPERS_bool */

#ifndef TOPPERS_size
#define TOPPERS_size	uintptr_t		/* size */
#endif /* TOPPERS_size */

#ifndef TOPPERS_fp
struct TOPPERS_dummy_t { int TOPPERS_dummy_field; };
typedef	void			(*TOPPERS_fp_t)(struct TOPPERS_dummy_t);
#define TOPPERS_fp		TOPPERS_fp_t	/* the program starting address */
#endif /* TOPPERS_fp */

/*
 *  common data types
 */
typedef TOPPERS_bool	bool_t;		/* boolean */
typedef unsigned char	char_t;		/* unsigned character */

typedef signed int		int_t;		/* signed integer */
typedef unsigned int	uint_t;		/* unsigned integer*/

typedef signed long		long_t;		/* signed long integer */
typedef unsigned long	ulong_t;	/* unsigned long integer */

typedef int_t			FN;			/* function code */
typedef	int_t			ER;			/* error code */
typedef	int_t			ID;			/* object ID number */
typedef	uint_t			ATR;		/* attributes of an object */
typedef	uint_t			STAT;		/* object state */
typedef	uint_t			MODE;		/* service call operational mode */
typedef	int_t			PRI;		/* priority */
typedef	TOPPERS_size	SIZE;		/* memory area size */

typedef	int_t			TMO;		/* timeout*/
typedef	uint_t			RELTIM;		/* relative time */
typedef	ulong_t			SYSTIM;		/* system time */
typedef	ulong_t			SYSUTM;		/* performance evaluation system time */

typedef	TOPPERS_fp		FP;			/* the program starting address */

typedef	int_t			ER_BOOL;	/* boolean or error code */
typedef	int_t			ER_ID;		/* error code or ID number */
typedef	int_t			ER_UINT;	/* unsigned integer or error code */

typedef uintptr_t		MB_T;		/* Data type to ensure the management area */

typedef	uint32_t		ACPTN;		/* access pattern */
typedef	struct acvct {				/* access vector */
	ACPTN		acptn1;				/* access pattern of normal operation 1 */
	ACPTN		acptn2;				/* access pattern of normal operation 2 */
	ACPTN		acptn3;				/* access pattern of management operation */
	ACPTN		acptn4;				/* access pattern of reference operation */
} ACVCT;

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  macro to make an integer constant
 */
#ifndef UINT_C
#define UINT_C(val)		(val ## U)		/* macro to create a constant of uint_t  */
#endif /* UINT_C */

#ifndef ULONG_C
#define ULONG_C(val)	(val ## UL)		/* macro to create a constant of ulong_t */
#endif /* ULONG_C */

/*
 *  normal constants
 */
#ifndef NULL
#define NULL		0			/* invalid pointer */
#endif /* NULL */

#ifndef true
#define true		1			/* true */
#endif /* true */

#ifndef false
#define false		0			/* false */
#endif /* false */

#define E_OK		0			/* ok */

/*
 *  the main error code
 */
#define E_SYS		(-5)		/* system error */
#define E_NOSPT		(-9)		/* unsupported features */
#define E_RSFN		(-10)		/* reserved function code */
#define E_RSATR		(-11)		/* reserved attribute */
#define E_PAR		(-17)		/* parameter error */
#define E_ID		(-18)		/* invalid ID number */
#define E_CTX		(-25)		/* context error */
#define E_MACV		(-26)		/* memory access violation */
#define E_OACV		(-27)		/* object access violation */
#define E_ILUSE		(-28)		/* illegal service call use */
#define E_NOMEM		(-33)		/* insufficient memory */
#define E_NOID		(-34)		/* no ID number available */
#define E_NORES		(-35)		/* no resource available */
#define E_OBJ		(-41)		/* object state error */
#define E_NOEXS		(-42)		/* non-existent object */
#define E_QOVR		(-43)		/* queue overflow */
#define E_RLWAI		(-49)		/* forced release from waiting */
#define E_TMOUT		(-50)		/* polling failure or timeout */
#define E_DLT		(-51)		/* waiting object deleted */
#define E_CLS		(-52)		/* waiting object state changed */
#define E_WBLK		(-57)		/* non-blocking accepted */

#define E_BOVR		(-58)		/* buffer overflow */

/*
 *  object attributes
 */
#define TA_NULL		UINT_C(0)	/* do not specify an object attribute */

/*
 *  specified time-out
 */
#define TMO_POL		0			/* polling */
#define TMO_FEVR	(-1)		/* wait forever */
#define TMO_NBLK	(-2)		/* non-blocking */

/*
 *  access patterns
 */
#define TACP_KERNEL		(0U)	/* allow access only to the kernel domain */
#define TACP_SHARED		(~0U)	/* allow access from all domains */

/*
 *  macro to retrieve information about the type
 */
#ifndef offsetof
#define	offsetof(structure, field)	((uintptr_t) &(((structure *) 0)->field))
#endif /* offsetof */

#ifndef alignof
#define alignof(type)	offsetof(struct { char field1; type field2; }, field2)
#endif /* alignof */

#ifndef ALIGN_TYPE				/* checking the alignment */
#define ALIGN_TYPE(addr, type) \
						((((uintptr_t)(addr)) & (alignof(type) - 1)) == 0U)
#endif /* ALIGN_TYPE */

/*
 *  assert macro
 */
#undef assert
#ifndef NDEBUG
#define assert(exp)		((void)((exp) ? 0 : (TOPPERS_assert_fail(#exp, \
							__FILE__, __LINE__), TOPPERS_assert_abort(), 0)))
#else /* NDEBUG */
#define assert(exp)		((void) 0)
#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif

/*
 *  macro to generate or decompose an error code
 *
 *  for the following macro definitions, signed integer is two's
 *  complement, the right shift operator (>>) is assumed to make an
 *  arithmetic shift. Otherwise, these macro definitions must be
 *  overridden in target_stddef.h.
 *
 *  For MERCD macro definitions, the main error code is assumed to be
 *  negative. Otherwise, the two definitions don't match each other.
 */
#ifndef ERCD
#define ERCD(mercd, sercd) \
				((ER)((((uint_t) sercd) << 8) | (((uint_t) mercd) & 0xffU)))
#endif /* ERCD */

#ifndef MERCD
#ifdef INT8_MAX
#define MERCD(ercd)		((ER)((int8_t)(ercd)))
#else /* INT8_MAX */
#define MERCD(ercd)		((ER)(((uint_t) ercd) | ~0xffU))
#endif /* INT8_MAX */
#endif /* MERCD */

#ifndef SERCD
#define SERCD(ercd)		((ER)((ercd) >> 8))
#endif /* SERCD */

/*
 *  macro to generate access pattern
 */
#define TACP(domid)		(1U << ((domid) - 1))	/* allow access only to domid */

/*
 *  the maximum value of relative time (RELTIM)
 */
#if UINT_MAX < LONG_MAX
#define TMAX_RELTIM		((RELTIM) UINT_MAX)
#else /* UINT_MAX < LONG_MAX */
#define TMAX_RELTIM		((RELTIM) LONG_MAX)
#endif /* UINT_MAX < LONG_MAX */

#endif /* TOPPERS_T_STDDEF_H */
