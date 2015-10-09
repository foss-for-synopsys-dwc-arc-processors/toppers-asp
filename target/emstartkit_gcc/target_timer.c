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

#include "kernel_impl.h"
#include "time_event.h"
#include <sil.h>
#include "target_timer.h"


#define TIMER_CTRL_IE		(1 << 0) /* Interupt when Count reachs limit */
#define TIMER_CTRL_NH		(1 << 1) /* Count only when CPU NOT halted */

#define ARC_TIMER_MAX	0xFFFFFFFF


/*
 *  kernel timer initialization
 */
void
target_timer_initialize(intptr_t exinf)
{
	CLOCK    cyc = TO_CLOCK(TIC_NUME, TIC_DENO);

	write_aux_reg(ARC_REG_TIMER0_CTRL, 0);
	write_aux_reg(ARC_REG_TIMER0_LIMIT, cyc);
	write_aux_reg(ARC_REG_TIMER0_CTRL, TIMER_CTRL_IE | TIMER_CTRL_NH);
	write_aux_reg(ARC_REG_TIMER0_CNT, 0);	/* start from 0 */
}

/*
 *  kernel timer termination
 */
void
target_timer_terminate(intptr_t exinf)
{
	/* disable timer interrupt */
	write_aux_reg(ARC_REG_TIMER0_CTRL, 0);
	write_aux_reg(ARC_REG_TIMER0_CNT, 0);
}

/*
 *  kernel timer interrupt handler
 */
void
target_timer_handler(void)
{
	i_begin_int(INTNO_TIMER);
	/* clear timer interrupt */
	write_aux_reg(ARC_REG_TIMER0_CTRL, TIMER_CTRL_IE | TIMER_CTRL_NH);
	signal_time();		/* signal the kernel */
	i_end_int(INTNO_TIMER);
}
