/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2008 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: tool_stddef.h 819 2008-03-18 12:58:50Z hiro $
 */

/*
 *		tool chain dependent part of t_stddef.h (for GCC)
 */

#ifndef TOPPERS_TOOL_STDDEF_H
#define TOPPERS_TOOL_STDDEF_H

/*
 *  macro definitions of compiler extend function
 */
#ifndef __cplusplus					/* C++ supports inline */
#if __STDC_VERSION__ < 199901L		/* C99 supports inline  */
#define inline	__inline__			/* inline function */
#endif /* __STDC_VERSION__ < 199901L */
#endif /* __cplusplus */

#define Inline	static __inline__	/* inline function */

#ifndef __cplusplus					/* C++ supports asm */
#define asm		__asm__				/* inline asm */
#endif /* __cplusplus */

#define Asm		__asm__ volatile	/* inline asm (no optimization) */

#define NoReturn	__attribute__((__noreturn__))
									/* function without return */

/*
 *  utilize the standard header files of tool chain 
 *
 *  the definition of NULL is in stddef.h, INT_MAX, INT_MIN, UINT_MAX, LONG_MAX, 
 *  LONG_MIN, ULONG_MAX, CHAR_BIT come from limits.h.
 *
 *  for C++/EC++, these header files may not be the standard header
 *  files.(for most tool chain, these header files are supported)
 *
 */
#ifndef TOPPERS_MACRO_ONLY
#include <stddef.h>
#include <limits.h>
#endif /* TOPPERS_MACRO_ONLY */

/*
 *  alternative definitions for stdint.h
 *
 *  If there is no stdint.h in tool chain, use the following definitions
 *
 *  TOPPERS_STDINT_TYPE1: char/short/int/long long are 
 *                        8/16/32/64 bits long ¡¤the length of pointer
 *                        is the same as long
 */
#ifdef TOPPERS_STDINT_TYPE1

/*
 *  compiler dependent data type
 */
#ifndef TOPPERS_MACRO_ONLY

typedef signed char			int8_t;		/* 8 bit signed integer */
typedef unsigned char		uint8_t;	/* 8 bit unsigned integer */

typedef signed short		int16_t;	/* 16 bit signed integer */
typedef unsigned short		uint16_t;	/* 16 bit unsigned integer*/

typedef signed int			int32_t;	/* 32 bit signed integer*/
typedef unsigned int		uint32_t;	/* 32 bit unsigned integer */

typedef signed long long	int64_t;	/* 64 bit signed integer */
typedef unsigned long long	uint64_t;	/* 64 bit unsigned integer */

typedef int8_t				int_least8_t;	/* at least 8 bit signed integer */
typedef uint8_t				uint_least8_t;	/* at least 8 bit unsigned integer */

typedef long				intptr_t;	/* the signed integer which can hold pointer */
typedef unsigned long		uintptr_t;	/* the unsigned integer which can hold pointer */

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  the macros to generate the compiler dependent data type
 */
#ifndef INT8_C
#define INT8_C(val)			(val)
#endif /* INT8_C */

#ifndef UINT8_C
#define UINT8_C(val)		(val ## U)
#endif /* UINT8_C */

#ifndef INT16_C
#define INT16_C(val)		(val)
#endif /* INT16_C */

#ifndef UINT16_C
#define UINT16_C(val)		(val ## U)
#endif /* UINT16_C */

#ifndef INT32_C
#define INT32_C(val)		(val)
#endif /* INT32_C */

#ifndef UINT32_C
#define UINT32_C(val)		(val ## U)
#endif /* UINT32_C */

#ifndef INT64_C
#define INT64_C(val)		(val ## LL)
#endif /* INT64_C */

#ifndef UINT64_C
#define UINT64_C(val)		(val ## ULL)
#endif /* UINT64_C */

/*
 *  the max and min value of compiler dependent data type
 */
#define INT8_MAX			SCHAR_MAX
#define INT8_MIN			SCHAR_MIN
#define UINT8_MAX			UCHAR_MAX

#define INT16_MAX			SHRT_MAX
#define INT16_MIN			SHRT_MIN
#define UINT16_MAX			USHRT_MAX

#define INT32_MAX			INT_MAX
#define INT32_MIN			INT_MIN
#define UINT32_MAX			UINT_MAX

#define INT64_MAX			LLONG_MAX
#define INT64_MIN			LLONG_MIN
#define UINT64_MAX			ULLONG_MAX

#define INT_LEAST8_MAX		INT8_MAX
#define INT_LEAST8_MIN		INT8_MIN
#define UINT_LEAST8_MAX		INT8_MAX

#endif /* TOPPERS_STDINT_TYPE1 */

/*
 *  definitions for float 
 *
 *  TOPPERS_STDFLOAT_TYPE1: float is IEEE754 compatible
 *							double is double float
 */
#ifdef TOPPERS_STDFLOAT_TYPE1
#ifndef TOPPERS_MACRO_ONLY

typedef float		float32_t;			/* IEEE754 compatible float */
typedef double		double64_t;			/* IEEE754 compatible double float */

#endif /* TOPPERS_MACRO_ONLY */

#define FLOAT32_MIN		1.17549435e-38F
#define FLOAT32_MAX		3.40282347e+38F
#define DOUBLE64_MIN	2.2250738585072014e-308
#define DOUBLE64_MAX	1.7976931348623157e+308

#endif /* TOPPERS_STDFLOAT_TYPE1 */
#endif /* TOPPERS_TOOL_STDDEF_H */
