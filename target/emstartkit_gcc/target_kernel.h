/* ------------------------------------------
 * Copyright (c) 2015, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */

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
