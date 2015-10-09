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
 *  the chip dependent part of kernel.h (for emstartkit)
 *
 *  This file is included in kernel.h. Other files should not include it
 *  directly. Before this file is included, t_stddef.h should also be 
 *  included
 */

#ifndef TOPPERS_TARGET_KERNEL_H
#define TOPPERS_TARGET_KERNEL_H

/*
 *  the core dependent common definitions (for ARC)
 */
#include "arc_gcc/common/core_kernel.h"

/*
 *  the supported kernel functions
 */
#define TOPPERS_TARGET_SUPPORT_ENA_INT		/* support ena_int */
#define TOPPERS_TARGET_SUPPORT_DIS_INT		/* support dis_int */
#define TOPPERS_TARGET_SUPPORT_GET_UTM		/* support get_utm */
/*
 *  the range of interrupt priority
 */
#define TMIN_INTPRI        (-2)        /* the min value of interrupt priority (highest priority)*/
#define TMAX_INTPRI        (-1)        /* the max value of interrupt priority (lowest priority)*/

/*
 *  time tick definitions
 */
#define TIC_NUME     1U            /* the numerator of time tick */
#define TIC_DENO     1U            /* the denominator of time tick*/


/* 
 *  trace log related definition
 *  use cycle counter as log timer
 */ 
#define TRACE_GET_TIM()   cycle_counter_get_current()

#endif /* TOPPERS_TARGET_KERNEL_H */
