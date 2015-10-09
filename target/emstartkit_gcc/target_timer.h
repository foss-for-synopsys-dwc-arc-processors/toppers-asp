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
