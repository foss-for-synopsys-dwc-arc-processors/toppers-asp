/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: exception.c 2010 2010-12-31 12:44:33Z ertl-hiro $
 */

/*
 *		CPU exception management
 */

#include "kernel_impl.h"
#include "task.h"
#include "exception.h"

/*
 * default definition of trace macro
 */
#ifndef LOG_XSNS_DPN_ENTER
#define LOG_XSNS_DPN_ENTER(p_excinf)
#endif /* LOG_XSNS_DPN_ENTER */

#ifndef LOG_XSNS_DPN_LEAVE
#define LOG_XSNS_DPN_LEAVE(state)
#endif /* LOG_XSNS_DPN_LEAVE */

#ifndef LOG_XSNS_XPN_ENTER
#define LOG_XSNS_XPN_ENTER(p_excinf)
#endif /* LOG_XSNS_XPN_ENTER */

#ifndef LOG_XSNS_XPN_LEAVE
#define LOG_XSNS_XPN_LEAVE(state)
#endif /* LOG_XSNS_XPN_LEAVE */

/* 
 *  initialization of cpu exception management
 */
#ifdef TOPPERS_excini
#ifndef OMIT_INITIALIZE_EXCEPTION

void
initialize_exception(void)
{
	uint_t			i;
	const EXCINIB	*p_excinib;

	for (p_excinib = excinib_table, i = 0; i < tnum_excno; p_excinib++, i++) {
		x_define_exc(p_excinib->excno, p_excinib->exc_entry);
	}
}

#endif /* OMIT_INITIALIZE_EXCEPTION */
#endif /* TOPPERS_excini */

/*
 *  refer the context when exceptions happened
 */

/*
 *  check whether in the dispatch reserved status (dispatch cannot be done)
 *  when exceptions happened.
 *
 *  dipatch reserved status: 
 *  	1. int disable  or 
 *  	2. cpu lock or 
 *  	3. dispatch disable
 *  	4. int priority mask is not zero (in interrupt handler)
 *
 *
 *  becasue the value of disdsp will not change in cpu exception handler,
 *  no need to save it.
 */
#ifdef TOPPERS_xsns_dpn

bool_t
xsns_dpn(void *p_excinf)
{
	bool_t	state;

	LOG_XSNS_DPN_ENTER(p_excinf);
	state = (exc_sense_intmask(p_excinf) && !disdsp) ? false : true;
	LOG_XSNS_DPN_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_xsns_dpn */

/*
 *  check whether task exception handling is disabled
 *  flase: task exception handling is enabled
 *  true: taske exception handling is disabled
 *
 *  becasue the value of ebatex will not change in cpu exception handler,
 *  no need to save it.бе
 */
#ifdef TOPPERS_xsns_xpn

bool_t
xsns_xpn(void *p_excinf)
{
	bool_t	state;

	LOG_XSNS_XPN_ENTER(p_excinf);
	state = (exc_sense_intmask(p_excinf) && p_runtsk->enatex) ? false : true;
	LOG_XSNS_XPN_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_xsns_xpn */
