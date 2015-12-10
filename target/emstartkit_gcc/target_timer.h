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
 *  timer driver (for emstartkit)
 */

#ifndef TOPPERS_TARGET_TIMER_H
#define TOPPERS_TARGET_TIMER_H

#include <sil.h>
#include "emstartkit.h"

/*
 *  timer interrupt related definitions
 */
#define INHNO_TIMER     16	/* interrupt handlerno */
#define INTNO_TIMER     16	/* interrupt no */
#define INTPRI_TIMER    -1	/* interrupt priority */
#define INTATR_TIMER    0U	/* interrupt attribute */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  CLOCK type
 */
typedef uint32_t    CLOCK;

/*
 *  the transfer from timer clock to ms and us.
 *
 */
#define TIMER_CLOCK          25000U
#define TO_CLOCK(nume, deno) (TIMER_CLOCK * (nume) / (deno))
#define TO_USEC(clock)       (((SYSUTM) clock) * 1000U / TIMER_CLOCK)


/*
 *  the max timer period (the unit is CLOCK)
 */
#define MAX_CLOCK        ((CLOCK) 0xffffffU)

/*
 *  check whetehr the current value of timer is the value before timer
 *  interrupt happened
 */
#define GET_TOLERANCE    100U    /* the possible delay (unit is CLOCK)*/
#define BEFORE_IREQ(clock) \
            ((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

/*
 *  initialize the timer
 *
 *  initialize the timer and set the timer period.
 */
extern void    target_timer_initialize(intptr_t exinf);

/*
 *  terminate the timer
 *
 */
extern void    target_timer_terminate(intptr_t exinf);

/*
 *  get the current value of timer
 */
Inline CLOCK
target_timer_get_current(void)
{
    return read_aux_reg(ARC_REG_TIMER0_CNT);
}

/*
 *  probe timer interrupt
 */
Inline bool_t
target_timer_probe_int(void)
{
    return(x_probe_int(INTNO_TIMER));
}

/*
 *  timer interrupt handler
 */
extern void    target_timer_handler(void);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_TARGET_TIMER_H */
