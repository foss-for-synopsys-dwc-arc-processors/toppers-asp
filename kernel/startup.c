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
 *  @(#) $Id: startup.c 2067 2011-05-05 00:46:29Z ertl-hiro $
 */

/*
 *	kernel startup and exit
 */

#include "kernel_impl.h"
#include "time_event.h"
#include <sil.h>

/*
 *   default definition of trace macro
 */
#ifndef LOG_KER_ENTER
#define LOG_KER_ENTER()
#endif /* LOG_KER_ENTER */

#ifndef LOG_KER_LEAVE
#define LOG_KER_LEAVE()
#endif /* LOG_KER_LEAVE */

#ifndef LOG_EXT_KER_ENTER
#define LOG_EXT_KER_ENTER()
#endif /* LOG_EXT_KER_ENTER */

#ifndef LOG_EXT_KER_LEAVE
#define LOG_EXT_KER_LEAVE(ercd)
#endif /* LOG_EXT_KER_LEAVE */

#ifdef TOPPERS_sta_ker

/*
 *  kernel active flag
 *
 *  this flag should be initialized with false in starup routine
 *  false: inactive
 *  ture: active
 */
bool_t	kerflg = false;

/*
 *  kernel startup
 *  the first function to be executed
 */
void
sta_ker(void)
{
	/*
	 *  target initialization
	 *  essential hardware initialization
	 */
	target_initialize();

	/*
	 *  initialize modules
	 *
	 *  the time event module must be initialized first 
	 */
	initialize_tmevt();
	initialize_object();

	/*
	 *  call init routines defined in static api
	 */ 
	call_inirtn();

	/*
	 *  start kernel
	 */
	kerflg = true;
	LOG_KER_ENTER();
	start_dispatch();
	assert(0);
}

#endif /* TOPPERS_sta_ker */

/*
 *  kernel exit
 */
#ifdef TOPPERS_ext_ker

ER
ext_ker(void)
{
	SIL_PRE_LOC;

	LOG_EXT_KER_ENTER();

	/*
	 *  lock interrupt
	 */
	SIL_LOC_INT();

	/*
	 *  kernel inactive
	 */
	LOG_KER_LEAVE();
	kerflg = false;

	/*
	 *  kernel exit routine
	 *
	 *  switch to non-task context, call exit_kernel
	 */
	call_exit_kernel();

	/*
	 * only against warnings during compilation
	 * actually not possible to be executed
	 */
	SIL_UNL_INT();
	LOG_EXT_KER_LEAVE(E_SYS);
	return(E_SYS);
}

/*
 *  kernel exit routine
 */
void
exit_kernel(void)
{
	/*
	 *  call termination routine defined in static api
	 */
	call_terrtn();

	/*
	 *  target-dependent exit routine
	 */
	target_exit();
	assert(0);
}

#endif /* TOPPERS_ext_ker */
