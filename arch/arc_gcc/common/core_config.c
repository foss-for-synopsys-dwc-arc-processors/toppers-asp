/* Copyright (C) 
 * 2014 - Wayne Ren
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/*
 *      core-dependent module (for ARC)
 */
#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "cache.h"

/*
 *  context reference variable
 */
uint32_t excpt_nest_count;

/*
 *  core-dependent initialize
 */
void
core_initialize()
{
/*
 *  when kernel is starting, the kernel is in non-task context,
 *  so set excpt_netst_count to 1
 */ 
#if defined(ARC_HAS_I_CACHE) || defined(ARC_HAS_D_CACHE)
	arc_cache_init();
#endif
	excpt_nest_count = 1;
/* all interrupts are unmasked */
	core_set_ipm(IIPM_ENAALL);
}

/*
 *  core-dependent terminate
 */
void
core_terminate(void)
{

}

/*
 *  config interrupt attribute
 *
 *  for ASP, use assert to report parameter error.
 *  for FI4 kernel, the error should be returned.
 *
 */
void
core_config_int(INTNO intno, ATR intatr, PRI intpri)
{
	assert(VALID_INTNO(intno));
	assert(TMIN_INTPRI <= intpri && intpri <= TMAX_INTPRI);
	
	/* set attribute and pri */
	write_aux_reg(ARC_REG_IRQ_SELECT, intno);
	write_aux_reg(ARC_REG_IRQ_PRIORITY, intpri - TMIN_INTPRI);
	if ((intatr & TA_EDGE) != 0U) {
		write_aux_reg(ARC_REG_IRQ_TRIGGLE, 1);
	} else {
		write_aux_reg(ARC_REG_IRQ_TRIGGLE, 0);
	}

	if ((intatr & TA_ENAINT) != 0U){
		core_enable_int(intno);
	} else {
		core_disable_int(intno);
	}    
}


/*
 *  install a CPU exception handler into the CPU exception vector
 */ 
void x_install_exc(EXCNO excno, FP exchdr) 
{
#if 0
	if (exchdr != NULL)
		exc_tbl[excno] = exchdr;
#endif
}


/*
 *  refer the status of system when CPU exceptions happened
 */
bool_t exc_sense_intmask(void *p_excinf)
{
	uint32_t status = 0;
	bool_t	int_lock;
	int	pri_mask;

	status = *((uint32_t *)p_excinf + P_EXCINF_OFFSET_ERSTATUS);
	
	if ((status & STATUS_IE_MASK) && (status & (~STATUS_AE_MASK))) {
		int_lock = false;
	} else {
		int_lock = true;
	}
	pri_mask = (status >> 1) & 0xf;
	pri_mask = pri_mask + TMIN_INTPRI;

	if (!int_lock && (pri_mask == IIPM_ENAALL) && (excpt_nest_count > 0)) {
		return true;
	} else {
		return false;
	}
}


/*
 *  output the status of CPU exceptions
 *
 *  this function is called in CPU exception handlers to output the
 *  status of CPU exceptions pointed by p_excinf.
 */
#ifdef SUPPORT_XLOG_SYS

void
xlog_sys(void *p_excinf)
{    
}

#endif /* SUPPORT_XLOG_SYS */

#ifndef OMIT_DEFAULT_EXC_HANDLER
/*
 * default CPU exception handler
 * p_exinf is stack pointer
 * r0-r12, r26-r31, lp, erbta, erstatus, eret are in stack
 */
void
default_exc_handler(void *p_excinf) {
	syslog_0(LOG_EMERG, "Unregistered Exception occurs.");
	ext_ker();
}


void default_int_handler(void *p_excinf) {
	syslog_0(LOG_EMERG, "Unregistered interrupt handler occurs");
}
#endif /* OMIT_DEFAULT_EXC_HANDLER */
