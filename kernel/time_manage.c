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
 *  $Id: time_manage.c 1747 2010-02-11 18:24:19Z ertl-hiro $
 */


/*
 *	system time tick management function
 */

#include "kernel_impl.h"
#include "check.h"
#include "time_event.h"

/*
 *  default definition of trace macro
 */
#ifndef LOG_GET_TIM_ENTER
#define LOG_GET_TIM_ENTER(p_systim)
#endif /* LOG_GET_TIM_ENTER */

#ifndef LOG_GET_TIM_LEAVE
#define LOG_GET_TIM_LEAVE(ercd, systim)
#endif /* LOG_GET_TIM_LEAVE */

#ifndef LOG_GET_UTM_ENTER
#define LOG_GET_UTM_ENTER(p_sysutm)
#endif /* LOG_GET_UTM_ENTER */

#ifndef LOG_GET_UTM_LEAVE
#define LOG_GET_UTM_LEAVE(ercd, sysutm)
#endif /* LOG_GET_UTM_LEAVE */

/*
 *  get system time tick
 */
#ifdef TOPPERS_get_tim

ER
get_tim(SYSTIM *p_systim)
{
	ER		ercd;

	LOG_GET_TIM_ENTER(p_systim);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	*p_systim = current_time;
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_GET_TIM_LEAVE(ercd, *p_systim);
	return(ercd);
}

#endif /* TOPPERS_get_tim */

/*
 *  get the time tick of performance evaluation timer
 */
#ifdef TOPPERS_get_utm
#ifdef TOPPERS_SUPPORT_GET_UTM
#ifndef OMIT_GET_UTM
#include "target_timer.h"

ER
get_utm(SYSUTM *p_sysutm)
{
	SYSUTM	utime;
	SYSTIM	time;
#if TIC_DENO != 1
	uint_t	subtime;
#endif /* TIC_DENO != 1 */
	CLOCK	clock1, clock2;
	bool_t	ireq;
	SIL_PRE_LOC;

	LOG_GET_UTM_ENTER(p_sysutm);

	SIL_LOC_INT();
	time = next_time;
#if TIC_DENO != 1
	subtime = next_subtime;
#endif /* TIC_DENO != 1 */
	clock1 = target_timer_get_current();
	ireq = target_timer_probe_int();
	clock2 = target_timer_get_current();
	SIL_UNL_INT();

	utime = ((SYSUTM) time) * 1000U;
#if TIC_DENO != 1
	utime += subtime * 1000U / TIC_DENO;
#endif /* TIC_DENO != 1 */

	if (!ireq || clock1 > clock2) {
		utime -= TIC_NUME * 1000U / TIC_DENO;
	}
	utime += TO_USEC(clock1);
	*p_sysutm = utime;

	LOG_GET_UTM_LEAVE(E_OK, *p_sysutm);
	return(E_OK);
}

#endif /* OMIT_GET_UTM */
#endif /* TOPPERS_SUPPORT_GET_UTM */
#endif /* TOPPERS_get_utm */
