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
 *  $Id: time_event.h 1774 2010-03-19 12:45:46Z ertl-hiro $
 */

/*
 *  time event management module
 */

#ifndef TOPPERS_TIME_EVENT_H
#define TOPPERS_TIME_EVENT_H

/*
 *  data type of event time
 *
 *  It's necessary for EVTTIM to have a wider range than RELTIM.
 *  In uITRON 4.0 specification, RELTIM  is unsigned int type and has
 *  16 or more bits. So EVETIM should be defined as ulong_t to guarantee
 *  its range wider than RELTIM.
 *
 */
typedef ulong_t	EVTTIM;

/*
 *  data types of time event block
 */
typedef void	(*CBACK)(void *);	/* callback*/

typedef struct time_event_block {
	uint_t	index;			/* position in time event heap */
	CBACK	callback;		/* callback routine */
	void	*arg;			/* argument for callback routine */
} TMEVTB;

/*
 *  node in time event heap
 */
typedef struct time_event_node {
	EVTTIM	time;			/* the time when event happens */
	TMEVTB	*p_tmevtb;		/* related time event block */
} TMEVTN;

/*
 *  time event heap(see kernel_cfg.c)
 */
extern TMEVTN	tmevt_heap[];

/*
 *  current system time (unit: 1ms)
 *
 *  when system starts, its value will be initialized to 0 and starts
 *  to count according to time tick
 */
extern EVTTIM	current_time;

/*
 *  the current valid minimum time in time event heap (unit: 1ms)
 */
extern EVTTIM	min_time;

/*
 *  the system time when next time tick comes (unit: 1ms)
 */
extern EVTTIM	next_time;

/*
 *  the compensation time of system time (unit: 1/TIC_DENO ms)
 *  when system time tick's time is not integral multiple of 1ms,
 *  e.g.  1 tick = 1.5 ms, next_subtime is used to compensate the
 *  system time to make system time as accurate as possible.
 *
 *  When TIC_DENO is 1, next_subtime is not needed.
 */
#if TIC_DENO != 1U
extern uint_t	next_subtime;
#endif /* TIC_DENO != 1U */

/*
 *  relative base time (unit: 1ms)
 *
 */
#if TIC_DENO == 1U
#define	base_time	(next_time)
#else /* TIC_DENO == 1U */
#define	base_time	(next_time + (next_subtime > 0U ? 1U : 0U))
#endif /* TIC_DENO == 1U */


/*
 *  the last one's index in time event heap
 */
extern uint_t	last_index;

/*
 *  initialization of time event module
 */
extern void	initialize_tmevt(void);

/*
 *  search the right place in time event heap for a time event.
 */
extern uint_t	tmevt_up(uint_t index, EVTTIM time);
extern uint_t	tmevt_down(uint_t index, EVTTIM time);

/*
 *  insert and delete of time events.
 */
extern void	tmevtb_insert(TMEVTB *p_tmevtb, EVTTIM time);
extern void	tmevtb_delete(TMEVTB *p_tmevtb);

/*
 *  enqueue a time event (time is relative time to current system time)
 *
 *  enqueue the time event specified by p_tmevtb into the time even heap
 *  when 'time' ms passed , the callback with argument 'arg', will be
 *  called in system time tick interrupt handler.
 *
 */
Inline void
tmevtb_enqueue(TMEVTB *p_tmevtb, RELTIM time, CBACK callback, void *arg)
{
	assert(time <= TMAX_RELTIM);

	p_tmevtb->callback = callback;
	p_tmevtb->arg = arg;
	tmevtb_insert(p_tmevtb, base_time + time);
}

/*
 *  enqueue a time event (time is absolute time)
 *
 *  enqueue the time event specified by p_tmevtb into the time even heap
 *  when 'time' ms passed , the callback with argument 'arg', will be
 *  called in system time tick interrupt handler.
 */
Inline void
tmevtb_enqueue_evttim(TMEVTB *p_tmevtb, EVTTIM time, CBACK callback, void *arg)
{
	p_tmevtb->callback = callback;
	p_tmevtb->arg = arg;
	tmevtb_insert(p_tmevtb, time);
}

/*
 *  dequeue a time event
 */
Inline void
tmevtb_dequeue(TMEVTB *p_tmevtb)
{
	tmevtb_delete(p_tmevtb);
}

/*
 *  left time before time event happens
 */
extern RELTIM	tmevt_lefttim(TMEVTB *p_tmevtb);

/*
 *  signal a system time tick comes
 */
extern void	signal_time(void);

#endif /* TOPPERS_TIME_EVENT_H */
