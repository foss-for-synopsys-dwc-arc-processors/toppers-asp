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
 *  $Id: time_event.c 1747 2010-02-11 18:24:19Z ertl-hiro $
 */

/*
 *	time event management module
 */

#include "kernel_impl.h"
#include "check.h"
#include "time_event.h"

/*
 *   operation macros for time event heap
 *   time event heap is a binary heap
 */
#define	PARENT(index)		((index) >> 1)		/* get parent */
#define	LCHILD(index)		((index) << 1)		/* get left child */
#define	TMEVT_NODE(index)	(tmevt_heap[(index) - 1])

/*
 *  compare time
 */
#define	EVTTIM_LT(t1, t2) (((t1) - min_time) < ((t2) - min_time))
#define	EVTTIM_LE(t1, t2) (((t1) - min_time) <= ((t2) - min_time))

#ifdef TOPPERS_tmeini

/*
 *  current system time(uint: 1ms)
 */
EVTTIM	current_time;

/*
 *  the current valid minimum time in time event heap (unit: 1ms)
 */
EVTTIM	min_time;

/*
 *  the system time when next time tick comes (unit: 1ms)
 */
EVTTIM	next_time;

/*
 *  the compensation time of system time (unit: 1/TIC_DENO ms)
 */
#if TIC_DENO != 1U
uint_t	next_subtime;
#endif /* TIC_DENO != 1U */

/*
 *  the last one's index in time event heap
 */
uint_t	last_index;

/*
 *  time event module initialization
 */
void
initialize_tmevt(void)
{
	current_time = min_time = 0U;
	next_time = current_time + TIC_NUME / TIC_DENO;
#if TIC_DENO != 1U
	next_subtime = TIC_NUME % TIC_DENO;
#endif /* TIC_DENO != 1U */
	last_index = 0U;
}

#endif /* TOPPERS_tmeini */

/*
 *  search up from index to find a right position for 'time'
 *  in time event heap
 */
#ifdef TOPPERS_tmeup

uint_t
tmevt_up(uint_t index, EVTTIM time)
{
	uint_t	parent;

	while (index > 1) {
		/*
		 *  if 'time' is not earlier than parent's time.
		 *  break, the right position is found
		 */
		parent = PARENT(index);
		if (EVTTIM_LE(TMEVT_NODE(parent).time, time)) {
			break;
		}

		/*  else
		 *  move down the parent
		 */
		TMEVT_NODE(index) = TMEVT_NODE(parent);
		TMEVT_NODE(index).p_tmevtb->index = index;

		/*
		 *  update index
		 *  new loop begins
		 */
		index = parent;
	}
	return(index);
}

#endif /* TOPPERS_tmeup */

/*
 *  search down from index to find a right position for 
 *  'time' in time event heap
 *
 */
#ifdef TOPPERS_tmedown

uint_t
tmevt_down(uint_t index, EVTTIM time)
{
	uint_t	child;

	while ((child = LCHILD(index)) <= last_index) {
		/*
		 *  use to the earlier child in two children(left, right),
		 *  to compare with 'time'
		 * 
		 */
		if (child + 1 <= last_index
						&& EVTTIM_LT(TMEVT_NODE(child + 1).time,
										TMEVT_NODE(child).time)) {
			child = child + 1;
		}

		/*
		 *  if the selected child is not earlier than 'time',
		 *  the right position if found
		 */
		if (EVTTIM_LE(time, TMEVT_NODE(child).time)) {
			break;
		}

		/*
		 *  move up the selected child
		 */
		TMEVT_NODE(index) = TMEVT_NODE(child);
		TMEVT_NODE(index).p_tmevtb->index = index;

		/*
		 *  update index
		 *  new loop begins
		 */
		index = child;
	}
	return(index);
}

#endif /* TOPPERS_tmedown */

/*
 *  insert a time event into time event heap
 *
 *
 *  insert a time event which happens in 'time' into the time event heap
 */
#ifdef TOPPERS_tmeins

void
tmevtb_insert(TMEVTB *p_tmevtb, EVTTIM time)
{
	uint_t	index;

	/*
	 *  last_index++
	 *  search up.
	 *  index is the right position for this
	 *  time event
	 */
	index = tmevt_up(++last_index, time);

	/*
	 *  insert time event in the right position 
	 */ 
	TMEVT_NODE(index).time = time;
	TMEVT_NODE(index).p_tmevtb = p_tmevtb;
	p_tmevtb->index = index;
}

#endif /* TOPPERS_tmeins */

/*
 *  delete a time event from time event heap
 */
#ifdef TOPPERS_tmedel

void
tmevtb_delete(TMEVTB *p_tmevtb)
{
	uint_t	index = p_tmevtb->index;
	uint_t	parent;
	EVTTIM	event_time = TMEVT_NODE(last_index).time;

	/*
	 *  last_index--
	 *  if last_index == 0
	 *  do nothing．
	 */
	if (--last_index == 0) {
		return;
	}

	/*
	 *
	 *  insert the last time event into the position of deleted time
	 *  event. In fact, the insert does not happened. The position of 
	 *  deleted time will be a empty node in time event heap. Then this
	 *  empty node will be moved to a right position which is the right
	 *  position of the last time event. 
	 * 
	 *  If the event time of the last time event is earlier than deleted
	 *  time event's parent, then search up, or search down.
	 *
	 */
	if (index > 1 && EVTTIM_LT(event_time,
								TMEVT_NODE(parent = PARENT(index)).time)) {
		/*
		 *  if deleted time event's parent is not earlier than last time event
		 *  in heap, change parent's position and update info.
		 * 
		 */
		TMEVT_NODE(index) = TMEVT_NODE(parent);
		TMEVT_NODE(index).p_tmevtb->index = index;

		/*
		 *  then search up to find the right position for the last time event in heap
		 *  from parent's position
		 */
		index = tmevt_up(parent, event_time);
	}
	else {
		/*
		 *  search down to find right position for last time event in heap．
		 */
		index = tmevt_down(index, event_time);
	}

	/*
	 * update the last time event's info
	 */ 
	TMEVT_NODE(index) = TMEVT_NODE(last_index + 1);
	TMEVT_NODE(index).p_tmevtb->index = index;
}

#endif /* TOPPERS_tmedel */

/*
 *  delete the top time event in time event heap
 */
Inline void
tmevtb_delete_top(void)
{
	uint_t	index;
	EVTTIM	event_time = TMEVT_NODE(last_index).time;

	/*
	 *  last_index--
	 *  if last_index == 0
	 *  do nothing．
	 */
	if (--last_index == 0) {
		return;
	}

	/*
	 *  search down to find righ position for last time event
	 *  in heap
	 */
	index = tmevt_down(1, event_time);

	/*
	 *  update info．
	 */ 
	TMEVT_NODE(index) = TMEVT_NODE(last_index + 1);
	TMEVT_NODE(index).p_tmevtb->index = index;
}

/*
 *  get left time for time event
 */
#ifdef TOPPERS_tmeltim

RELTIM
tmevt_lefttim(TMEVTB *p_tmevtb)
{
	EVTTIM	time;

	time = TMEVT_NODE(p_tmevtb->index).time;
	if (EVTTIM_LE(time, next_time)) {
		/*
		 *  if earlier than next system time
		 */
		return(0U);
	}
	else {
		return((RELTIM)(time - base_time));
	}
}

#endif /* TOPPERS_tmeltim */

/*
 *  signal the kernel that a system tick passed
 */
#ifdef TOPPERS_sigtim

void
signal_time(void)
{
	TMEVTB	*p_tmevtb;

	assert(sense_context());
	assert(!i_sense_lock());

	i_lock_cpu();

	/*
	 *  update current time
	 */
	current_time = next_time;

	/*
	 *  update next_time and next_sub time
	 */
#if TIC_DENO == 1U
	next_time = current_time + TIC_NUME;
#else /* TIC_DENO == 1U */
	next_subtime += TIC_NUME % TIC_DENO;
	next_time = current_time + TIC_NUME / TIC_DENO;
	if (next_subtime >= TIC_DENO) {
		next_subtime -= TIC_DENO;
		next_time += 1U;
	}
#endif /* TIC_DENO == 1U */

	/*
	 *  if current time is not earlier than the earliest time event in time
	 *  event heap, it means a time event shall happen
	 */
	while (last_index > 0 && EVTTIM_LE(TMEVT_NODE(1).time, current_time)) {
		p_tmevtb = TMEVT_NODE(1).p_tmevtb;
		tmevtb_delete_top();
		(*(p_tmevtb->callback))(p_tmevtb->arg);
	}

	/*
	 *  update min time
	 */
	min_time = current_time;

	i_unlock_cpu();
}

#endif /* TOPPERS_sigtim */
