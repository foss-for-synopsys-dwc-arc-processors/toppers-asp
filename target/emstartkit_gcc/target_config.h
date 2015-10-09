/* Copyright (C) 
 * 2013 - Wayne Ren
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
 *  board dependent module (for emstartkit)
 *
 *  the include file of the chip dependent part of the kernel.
 */

#ifndef TOPPERS_TARGET_CONFIG_H
#define TOPPERS_TARGET_CONFIG_H

/*
 *  board hardware resource definition
 */
#include "emstartkit.h"
 
/*
 *  trace log related definition
 */
#ifdef TOPPERS_ENABLE_TRACE
#include "logtrace/trace_config.h"
#endif /* TOPPERS_ENABLE_TRACE */


/*
 *  whether there are vector routines in the kernel.
 */
#define VECTOR_KERNEL

/* no need to init data section */
#define TOPPERS_OMIT_DATA_INIT

/*
 *  the start address of vector if there are no vector 
 *  routines in the kernel.
 */ 
#define VECTOR_START  0x00

/*
 * the instruction to wait interrupts
 */
#define ASM_TARGET_WAIT_INTERRUPT nop

/*
 *  default size of non-task context stack
 */
#define DEFAULT_ISTKSZ      0x1000U   /* 4KB */
                                                   
/*
 *  definitions about slight delay (maybe in the target dependent part
 *  of SIL)
 */
#define SIL_DLY_TIM1    339
#define SIL_DLY_TIM2    158

/*
 *  definitions about interrupt handler no.
 */ 
#define TMIN_INHNO 16U
#define TMAX_INHNO 25U
#define TNUM_INH   10U

/*
 *  definitions about interrupt no.
 */ 
#define TMIN_INTNO 16U
#define TMAX_INTNO 25U
#define TNUM_INT   10U

#ifndef TOPPERS_MACRO_ONLY

/*
 *  check whether the intno is valid
 *
 */
#define VALID_INTNO(intno) (TMIN_INTNO <= (intno) && (intno) <= TMAX_INTNO)
#define VALID_INTNO_DISINT(intno)	VALID_INTNO(intno)
#define VALID_INTNO_CFGINT(intno)	VALID_INTNO(intno)
#define VALID_INTNO_ATTISR(intno)	VALID_INTNO(intno)

/*
 *  define interrupt handler
 */
Inline void
x_define_inh(INHNO inhno, FP int_entry)
{
	/* no need to implement this, as interrupt handler is set in
	 * static configuration
	 */ 
}

/*
 *  to generate the entry of interrupt handler
 *
 */
#define INT_ENTRY(inhno, inthdr)    inthdr
#define INTHDR_ENTRY(inhno, inhno_num, inthdr) extern void inthdr(void);

/*
 *  implementation of TOPPERS standard interrupt model
 */

/*
 * the disable status of all interrupts
 */

#endif /* TOPPERS_MACRO_ONLY */  

#define INDEX_IPM(ipm)  (-(ipm))

/*
 *  core dependent module (for ARC)
 */
#include "arc_gcc/common/core_config.h"


#ifndef TOPPERS_MACRO_ONLY


Inline void
x_set_ipm(PRI intpri)
{
	core_set_ipm(intpri);
}

#define t_set_ipm(intpri) x_set_ipm(intpri)
#define i_set_ipm(intpri) x_set_ipm(intpri)

/*
 *  refer the current interrupt priority mask value
 *
 *  return ipm
 */
Inline PRI
x_get_ipm(void)
{
	return core_get_ipm();
}

#define t_get_ipm() x_get_ipm()
#define i_get_ipm() x_get_ipm()


Inline bool_t
x_disable_int(INTNO intno)
{
	return core_disable_int(intno);
}

#define t_disable_int(intno)  x_disable_int(intno)
#define i_disable_int(intno)  t_disable_int(intno)



Inline bool_t
x_enable_int(INTNO intno)
{
	return core_enable_int(intno);
}

#define t_enable_int(intno) x_enable_int(intno)
#define i_enable_int(intno) x_enable_int(intno)

/*
 * clear interrupt request
 */
Inline void
x_clear_int(INTNO intno)
{
   
}

#define t_clear_int(intno) x_clear_int(intno) 
#define i_clear_int(intno) x_clear_int(intno) 


/*
 *  probe interrupt
 */
Inline bool_t
x_probe_int(INTNO intno)
{	
    return core_probe_int(intno);
}

#define t_probe_int(intno) x_probe_int(intno)
#define i_probe_int(intno) x_probe_int(intno)

/*
 *  config interrupt attribute
 *
 */
Inline void x_config_int(INTNO intno, ATR intatr, PRI intpri)
{
	core_config_int(intno, intatr, intpri);
}

/*
 *  the necessory IRC processing before interrupt handled is called.
 *
 */
Inline void
i_begin_int(INTNO intno)
{
    
}

/*
 *  the necessory IRC processing after interrupt handled is called.
 *
 */
Inline void
i_end_int(INTNO intno)
{
    
}

/*
 *  target dependent system initialization
 */
extern void target_initialize(void);

/*
 *  target dependent exit processing.
 *
 *  called when the system exits.
 */
extern void target_exit(void) NoReturn;

#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_TARGET_CONFIG_H */
