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
 *  @(#) $Id: sil.h 883 2008-04-11 10:45:20Z hiro $
 */

/*
 *		System Interface Layer (SIL)
 *		
 *  The declarations and definitions of service call and macros of
 *  system interface layer are included in this head file.
 * 
 *  When this head file is included in assembly language source file,
 *  the macro TOPPERS_MACRO_ONLY should be defined. Then, the
 *  definitions that are not Marcos will be excluded.
 *
 *  No files are required to be included before this head file.
 */

#ifndef TOPPERS_SIL_H
#define TOPPERS_SIL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *	TOPPERS common data types, constants and macros
 */
#include <t_stddef.h>

/*
 *  target-dependent part
 */
#include "target_sil.h"

#ifndef TOPPERS_MACRO_ONLY

/*
 *  lock all interrupts
 *
 *  SIL_PRE_LOC, SIL_LOC_INT (), SIL_UNL_INT () are available in the
 *  target-dependent part. There are no default definitions.
 */

/*
 *  delay for a while
 */
extern void	sil_dly_nse(ulong_t dlytim) throw();

/*
 *  the endian transfer
 */
#ifndef TOPPERS_SIL_REV_ENDIAN_UINT16
#define	TOPPERS_SIL_REV_ENDIAN_UINT16(data) \
				((((data) & 0xff) << 8) | (((data) >> 8) & 0xff))
#endif /* TOPPERS_SIL_REV_ENDIAN_UINT16 */

#ifndef TOPPERS_SIL_REV_ENDIAN_UINT32
#define	TOPPERS_SIL_REV_ENDIAN_UINT32(data) \
				((((data) & 0xff) << 24) | (((data) & 0xff00) << 8) \
					| (((data) >> 8) & 0xff00) | (((data) >> 24) & 0xff))
#endif /* TOPPERS_SIL_REV_ENDIAN_UINT32 */

/*
 *  memory space access functions
 */
#ifndef TOPPERS_OMIT_SIL_ACCESS

/*
 *  read/write 8-bit data
 */
#ifdef UINT8_MAX

Inline uint8_t
sil_reb_mem(void *mem)
{
	uint8_t	data;

	data = *((volatile uint8_t *) mem);
	return(data);
}

Inline void
sil_wrb_mem(void *mem, uint8_t data)
{
	*((volatile uint8_t *) mem) = data;
}

#endif /* UINT8_MAX */

/*
 *  read/write 16-bit data
 */

Inline uint16_t
sil_reh_mem(void *mem)
{
	uint16_t	data;

	data = *((volatile uint16_t *) mem);
	return(data);
}

Inline void
sil_wrh_mem(void *mem, uint16_t data)
{
	*((volatile uint16_t *) mem) = data;
}

#ifdef SIL_ENDIAN_BIG			/*  for big-endian processor */

#define	sil_reh_bem(mem)		sil_reh_mem(mem)
#define	sil_wrh_bem(mem, data)	sil_wrh_mem(mem, data)

#ifndef TOPPERS_OMIT_SIL_REH_LEM

Inline uint16_t
sil_reh_lem(void *mem)
{
	uint16_t	data;

	data = *((volatile uint16_t *) mem);
	return(TOPPERS_SIL_REV_ENDIAN_UINT16(data));
}

#endif /* TOPPERS_OMIT_SIL_REH_LEM */
#ifndef TOPPERS_OMIT_SIL_WRH_LEM

Inline void
sil_wrh_lem(void *mem, uint16_t data)
{
	*((volatile uint16_t *) mem) = TOPPERS_SIL_REV_ENDIAN_UINT16(data);
}

#endif /* TOPPERS_OMIT_SIL_WRH_LEM */
#else /* SIL_ENDIAN_BIG */		/* for little-endian processor */

#define	sil_reh_lem(mem)		sil_reh_mem(mem)
#define	sil_wrh_lem(mem, data)	sil_wrh_mem(mem, data)

#ifndef TOPPERS_OMIT_SIL_REH_BEM

Inline uint16_t
sil_reh_bem(void *mem)
{
	uint16_t	data;

	data = *((volatile uint16_t *) mem);
	return(TOPPERS_SIL_REV_ENDIAN_UINT16(data));
}

#endif /* TOPPERS_OMIT_SIL_REH_BEM */
#ifndef TOPPERS_OMIT_SIL_WRH_BEM

Inline void
sil_wrh_bem(void *mem, uint16_t data)
{
	*((volatile uint16_t *) mem) = TOPPERS_SIL_REV_ENDIAN_UINT16(data);
}

#endif /* TOPPERS_OMIT_SIL_WRH_BEM */
#endif /* SIL_ENDIAN_BIG */

/*
 *  read/write 32-bit data
 */

Inline uint32_t
sil_rew_mem(void *mem)
{
	uint32_t	data;

	data = *((volatile uint32_t *) mem);
	return(data);
}

Inline void
sil_wrw_mem(void *mem, uint32_t data)
{
	*((volatile uint32_t *) mem) = data;
}

#ifdef SIL_ENDIAN_BIG			/* for big-endian processor */

#define	sil_rew_bem(mem)		sil_rew_mem(mem)
#define	sil_wrw_bem(mem, data)	sil_wrw_mem(mem, data)

#ifndef TOPPERS_OMIT_SIL_REW_LEM

Inline uint32_t
sil_rew_lem(void *mem)
{
	uint32_t	data;

	data = *((volatile uint32_t *) mem);
	return(TOPPERS_SIL_REV_ENDIAN_UINT32(data));
}

#endif /* TOPPERS_OMIT_SIL_REW_LEM */
#ifndef TOPPERS_OMIT_SIL_WRW_LEM

Inline void
sil_wrw_lem(void *mem, uint32_t data)
{
	*((volatile uint32_t *) mem) = TOPPERS_SIL_REV_ENDIAN_UINT32(data);
}

#endif /* TOPPERS_OMIT_SIL_WRW_LEM */
#else /* SIL_ENDIAN_BIG */		/* for little-endian processor */

#define	sil_rew_lem(mem)		sil_rew_mem(mem)
#define	sil_wrw_lem(mem, data)	sil_wrw_mem(mem, data)

#ifndef TOPPERS_OMIT_SIL_REW_BEM

Inline uint32_t
sil_rew_bem(void *mem)
{
	uint32_t	data;

	data = *((volatile uint32_t *) mem);
	return(TOPPERS_SIL_REV_ENDIAN_UINT32(data));
}

#endif /* TOPPERS_OMIT_SIL_REW_BEM */
#ifndef TOPPERS_OMIT_SIL_WRW_BEM

Inline void
sil_wrw_bem(void *mem, uint32_t data)
{
	*((volatile uint32_t *) mem) = TOPPERS_SIL_REV_ENDIAN_UINT32(data);
}

#endif /* TOPPERS_OMIT_SIL_WRW_BEM */
#endif /* SIL_ENDIAN_BIG */
#endif /* TOPPERS_OMIT_SIL_ACCESS */

#endif /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* TOPPERS_SIL_H */
