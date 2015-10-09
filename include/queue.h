/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: queue.h 1549 2009-05-10 15:21:52Z ertl-hiro $
 */

/*
 *		queue operation library
 *  In this queue operation library, the queue is a double-linked ring
 *  queue including the queue header. The next entry field in queue
 *  header points to the first entry in the queue, the pre entry field
 *  in queue header points to the last entry in the queue. The pre entry
 *  field of the first entry in the queue and the next entry field of
 *  the last entry in the queue point to the queue header. If the next
 *  entry field and pre entry field of the queue header point to each
 *  other, it's an empty queue.
 */

#ifndef	TOPPERS_QUEUE_H
#define	TOPPERS_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  the definition of queue data structure
 */
typedef struct queue {
	struct queue *p_next;		/* pointer to next entry */
	struct queue *p_prev;		/* pointer to previous entry */
} QUEUE;

/*
 *  initialize the queue
 *
 *  the p_queue points to the queue header.
 */
Inline void
queue_initialize(QUEUE *p_queue)
{
	p_queue->p_prev = p_queue->p_next = p_queue;
}

/*
 *  insert one entry into the queue's pre entry
 *
 *  insert one entry (p_entry) into the p_queue's pre entry. If p_queue
 *  is the queue header, p_entry will be the end of queue.
 */
Inline void
queue_insert_prev(QUEUE *p_queue, QUEUE *p_entry)
{
	p_entry->p_prev = p_queue->p_prev;
	p_entry->p_next = p_queue;
	p_queue->p_prev->p_next = p_entry;
	p_queue->p_prev = p_entry;
}

/*
 *  insert one entry into the queue's next entry
 *
 *  insert one entry (p_entry) into the p_queue's next entry. If p_queue
 *  is the queue header, p_entry will be the start of queue.
 */
Inline void
queue_insert_next(QUEUE *p_queue, QUEUE *p_entry)
{
	p_entry->p_prev = p_queue;
	p_entry->p_next = p_queue->p_next;
	p_queue->p_next->p_prev = p_entry;
	p_queue->p_next = p_entry;
}

/*
 *  delete entry
 *
 *  delete p_entry from the queue.
 */
Inline void
queue_delete(QUEUE *p_entry)
{
	p_entry->p_prev->p_next = p_entry->p_next;
	p_entry->p_next->p_prev = p_entry->p_prev;
}

/*
 *  delete the next entry of p_queue from the queue
 *
 *  delete the next entry of p_queue from the queue, and return the
 *  deleted entry. If p_queue is the queue header, the first entry in
 *  the queue will be deleted. p_queue should not be empty.
 */
Inline QUEUE *
queue_delete_next(QUEUE *p_queue)
{
	QUEUE	*p_entry;

	assert(p_queue->p_next != p_queue);
	p_entry = p_queue->p_next;
	p_queue->p_next = p_entry->p_next;
	p_entry->p_next->p_prev = p_queue;
	return(p_entry);
}

/*
 *  check if the queue is empty
 *
 *  the p_queue is the queue header.
 */
Inline bool_t
queue_empty(QUEUE *p_queue)
{
	if (p_queue->p_next == p_queue) {
		assert(p_queue->p_prev == p_queue);
		return(true);
	}
	return(false);
}

#ifdef __cplusplus
}
#endif

#endif /* TOPPERS_QUEUE_H */
