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
 *  @(#) $Id: sample1.c 1968 2010-11-20 11:00:07Z ertl-hiro $
 */

/*
 *  sample program(1)
 *
 * 	this sample program is used to demonstrate the functions of
 * 	TOPPERS/ASP kernel
 *
 *  program description:
 *
 *	there are 1 main task (task ID : MAIN_TASK, task priority :
 *	MAIN_PRIORIYT) used as user interface, 3 target tasks (task ID: TASK1
 *	~ TASK3, initial task priority : MID_PRIORITY), 1 cyclic handler
 *	with a period of 2s in this program.
 *
 *  In the 3 target tasks which share the same task body, 'task_loop'
 *  loops are made and then an message is output to show the specified
 *  task is running.
 *
 *  In the cyclic handler the ready queues of 3 task priorities (HIGH_PRIORITY, MID_PRIORITY,
 *  and LOW_PRIORITY) is rotated. The cyclic handler is stooped at the
 *  start of program.
 *
 *  The main task receives and processes the char commands from the
 *  serial port. The target tasks are running when the main task is
 *  waiting for char commands. The functions of char commands are the
 *  following (when Control-C or 'Q' are received, the sample program
 *  exits)
 *
 *  '1' : the target task is set to TASK1 (default).
 *  '2' : the target task is set to TASK2.
 *  '3' : the target task is set to TASK1.
 *  'a' : activate the target task.
 *  'A' : cancel the activation request for the target task.
 *  'e' : let the target task exits by itself.
 *  't' : terminate the target task.
 *  '>' : set the priority of target task to HIGH_PRIORITY.
 *  '=' : set the priority of target task to MID_PRIORITY.
 *  '<' : set the priority of target task to low_PRIORITY.
 *  'G' : get the priority of target task.
 *  's' : let the target task sleep by itself.
 *  'S' : let the target task sleep for 10s by itself.
 *  'w' : wakeup the target task.
 *  'W' : cancel the wakeup request for the target task.
 *  'l' : release the target task from waiting state.
 *  'u' : suspend the target task.
 *  'm' : resume the target task.
 *  'd' : let the target task delay for 10s (dly_tsk(10s)).
 *  'x' : raise the target task's task exception with a value of 0x0001.
 *  'X' : raise the target task's task exception with a value of 0x0002.
 *  'y' : let the target task call dis_tex() to disable task exception.
 *  'Y' : let the target task call ena_tex() to enable task exception.
 *  'r' : rotate the ready queues of the 3 task priorities
 *  	  (HIGH_PRIORITY, MID_PRIORITY and LOW_PRIORITY).
 *  'c' : start the cyclic handler.
 *  'C' : stop the cyclic handler
 *  'b' : start the alarm handler 5s later.
 *  'B' : stop the alarm handler.
 *  'z' : trigger the CPU exception in the target task (terminate the
 *  	  target task).
 *  'Z' : trigger the CPU exception in CPU lock state in the target task
 *		  (terminate the program).
 *  '@' : create task3 by acre_tsk, and set the task exception routine 
 *  	  by def_tex.
 *  '!' : delete the target task by del_tsk.
 *  'V' : get the system time for performance evaluation by get_utm()
 * 		  twice.
 *  'v' : show the call of service calls (default).
 *  'q' : do not show the call of service calls.
 */

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"

/*
 *  output the error message of service calls
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

/*
 *  message for target tasks
 */
char_t	message[3];

/*
 *  the number of loop
 */
ulong_t	task_loop;		/* loop in task */
ulong_t	tex_loop;		/* loop in task exception routine */

/*
 *  target tasks
 */
void task(intptr_t exinf)
{
	volatile ulong_t	i;
	int_t		n = 0;
	int_t		tskno = (int_t) exinf;
	const char	*graph[] = { "|", "  +", "    *" };
	char_t		c;

	SVC_PERROR(ena_tex());
	while (1) {
		syslog(LOG_NOTICE, "task%d is running (%03d).   %s",
										tskno, ++n, graph[tskno-1]);
		for (i = 0; i < task_loop; i++);
		c = message[tskno-1];
		message[tskno-1] = 0;
		switch (c) {
		case 'e':
			syslog(LOG_INFO, "#%d#ext_tsk()", tskno);
			SVC_PERROR(ext_tsk());
			assert(0);
		case 's':
			syslog(LOG_INFO, "#%d#slp_tsk()", tskno);
			SVC_PERROR(slp_tsk());
			break;
		case 'S':
			syslog(LOG_INFO, "#%d#tslp_tsk(10000)", tskno);
			SVC_PERROR(tslp_tsk(10000));
			break;
		case 'd':
			syslog(LOG_INFO, "#%d#dly_tsk(10000)", tskno);
			SVC_PERROR(dly_tsk(10000));
			break;
		case 'y':
			syslog(LOG_INFO, "#%d#dis_tex()", tskno);
			SVC_PERROR(dis_tex());
			break;
		case 'Y':
			syslog(LOG_INFO, "#%d#ena_tex()", tskno);
			SVC_PERROR(ena_tex());
			break;
#ifdef CPUEXC1
		case 'z':
			syslog(LOG_NOTICE, "#%d#raise CPU exception", tskno);
			RAISE_CPU_EXCEPTION;
			break;
		case 'Z':
			SVC_PERROR(loc_cpu());
			syslog(LOG_NOTICE, "#%d#raise CPU exception", tskno);
			RAISE_CPU_EXCEPTION;
			SVC_PERROR(unl_cpu());
			break;
#endif /* CPUEXC1 */
		default:
			break;
		}
	}
}

/*
 *  task exception routine for the target tasks
 */
void tex_routine(TEXPTN texptn, intptr_t exinf)
{
	volatile ulong_t	i;
	int_t	tskno = (int_t) exinf;

	syslog(LOG_NOTICE, "task%d receives exception 0x%04x.", tskno, texptn);
	for (i = 0; i < tex_loop; i++);

	if ((texptn & 0x8000U) != 0U) {
		syslog(LOG_INFO, "#%d#ext_tsk()", tskno);
		SVC_PERROR(ext_tsk());
		assert(0);
	}
}

/*
 *  CPU exception handler
 */
#ifdef CPUEXC1

void
cpuexc_handler(void *p_excinf)
{
	ID		tskid;

	syslog(LOG_NOTICE, "CPU exception handler (p_excinf = %08p).", p_excinf);
	if (sns_ctx() != true) {
		syslog(LOG_WARNING,
					"sns_ctx() is not true in CPU exception handler.");
	}
	if (sns_dpn() != true) {
		syslog(LOG_WARNING,
					"sns_dpn() is not true in CPU exception handler.");
	}
	syslog(LOG_INFO, "sns_loc = %d sns_dsp = %d sns_tex = %d",
									sns_loc(), sns_dsp(), sns_tex());
	syslog(LOG_INFO, "xsns_dpn = %d xsns_xpn = %d",
									xsns_dpn(p_excinf), xsns_xpn(p_excinf));

	if (xsns_xpn(p_excinf)) {
		syslog(LOG_NOTICE, "Sample program ends with exception.");
		SVC_PERROR(ext_ker());
		assert(0);
	}

	SVC_PERROR(iget_tid(&tskid));
	SVC_PERROR(iras_tex(tskid, 0x8000U));
}

#endif /* CPUEXC1 */

/*
 *  cyclic handler
 *
 *  rotate the ready queues of the 3 task priorities
 *  (HIGH_PRIORITY, MID_PRIORITY and LOW_PRIORITY).
 */
void cyclic_handler(intptr_t exinf)
{
	SVC_PERROR(irot_rdq(HIGH_PRIORITY));
	SVC_PERROR(irot_rdq(MID_PRIORITY));
	SVC_PERROR(irot_rdq(LOW_PRIORITY));
}

/*
 *  alarm handler
 *
 *  rotate the ready queues of the 3 task priorities
 *  (HIGH_PRIORITY, MID_PRIORITY and LOW_PRIORITY).
 */
void alarm_handler(intptr_t exinf)
{
	SVC_PERROR(irot_rdq(HIGH_PRIORITY));
	SVC_PERROR(irot_rdq(MID_PRIORITY));
	SVC_PERROR(irot_rdq(LOW_PRIORITY));
}

/*
 *  main task
 */
void main_task(intptr_t exinf)
{
	char_t	c;
	ID		tskid = TASK1;
	int_t	tskno = 1;
	ER_UINT	ercd;
	PRI		tskpri;
#ifndef TASK_LOOP
	volatile ulong_t	i;
	SYSTIM	stime1, stime2;
#endif /* TASK_LOOP */
#ifdef TOPPERS_SUPPORT_GET_UTM
	SYSUTM	utime1, utime2;
#endif /* TOPPERS_SUPPORT_GET_UTM */
	T_CTSK	ctsk;
	T_DTEX	dtex;
	ID		TASK3 = -1;

	SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));
	syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);

	/*
	 *  initialize the serial port
	 *
	 *  If the same serial port is shared between main task and system
	 *  log task. As this serial port is already opened in system log
	 *  task, serial_opn_por will return E_OBJ which is not a real
	 *  error.
	 */
	ercd = serial_opn_por(TASK_PORTID);
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
									itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(TASK_PORTID,
							(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

	/*
 	 *  adjustment of task_loop.
	 *
	 *  If TASK_LOOP is defined, the adjustment of task_loop will not be
	 *  done.
	 *
	 *  If MEASURE_TWICE is defined, 2 measurements are made. The 2nd
	 *  result is adopted as the 1st is a little longer than the 2nd.
	 */
#ifdef TASK_LOOP
	task_loop = TASK_LOOP;
#else /* TASK_LOOP */

#ifdef MEASURE_TWICE
	task_loop = LOOP_REF;
	SVC_PERROR(get_tim(&stime1));
	for (i = 0; i < task_loop; i++);
	SVC_PERROR(get_tim(&stime2));
#endif /* MEASURE_TWICE */

	task_loop = LOOP_REF;
	SVC_PERROR(get_tim(&stime1));
	for (i = 0; i < task_loop; i++);
	SVC_PERROR(get_tim(&stime2));
	task_loop = LOOP_REF * 400UL / (stime2 - stime1);

#endif /* TASK_LOOP */
	tex_loop = task_loop / 5;

	/*
 	 *  activate the target tasks
	 */
	SVC_PERROR(act_tsk(TASK1));
	SVC_PERROR(act_tsk(TASK2));

	/*
 	 *  main loop
	 */
	do {
		SVC_PERROR(serial_rea_dat(TASK_PORTID, &c, 1));
		switch (c) {
		case 'e':
		case 's':
		case 'S':
		case 'd':
		case 'y':
		case 'Y':
		case 'z':
		case 'Z':
			message[tskno-1] = c;
			break;
		case '1':
			tskno = 1;
			tskid = TASK1;
			break;
		case '2':
			tskno = 2;
			tskid = TASK2;
			break;
		case '3':
			tskno = 3;
			tskid = TASK3;
			break;
		case 'a':
			syslog(LOG_INFO, "#act_tsk(%d)", tskno);
			SVC_PERROR(act_tsk(tskid));
			break;
		case 'A':
			syslog(LOG_INFO, "#can_act(%d)", tskno);
			SVC_PERROR(ercd = can_act(tskid));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "can_act(%d) returns %d", tskno, ercd);
			}
			break;
		case 't':
			syslog(LOG_INFO, "#ter_tsk(%d)", tskno);
			SVC_PERROR(ter_tsk(tskid));
			break;
		case '>':
			syslog(LOG_INFO, "#chg_pri(%d, HIGH_PRIORITY)", tskno);
			SVC_PERROR(chg_pri(tskid, HIGH_PRIORITY));
			break;
		case '=':
			syslog(LOG_INFO, "#chg_pri(%d, MID_PRIORITY)", tskno);
			SVC_PERROR(chg_pri(tskid, MID_PRIORITY));
			break;
		case '<':
			syslog(LOG_INFO, "#chg_pri(%d, LOW_PRIORITY)", tskno);
			SVC_PERROR(chg_pri(tskid, LOW_PRIORITY));
			break;
		case 'G':
			syslog(LOG_INFO, "#get_pri(%d, &tskpri)", tskno);
			SVC_PERROR(ercd = get_pri(tskid, &tskpri));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "priority of task %d is %d", tskno, tskpri);
			}
			break;
		case 'w':
			syslog(LOG_INFO, "#wup_tsk(%d)", tskno);
			SVC_PERROR(wup_tsk(tskid));
			break;
		case 'W':
			syslog(LOG_INFO, "#can_wup(%d)", tskno);
			SVC_PERROR(ercd = can_wup(tskid));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "can_wup(%d) returns %d", tskno, ercd);
			}
			break;
		case 'l':
			syslog(LOG_INFO, "#rel_wai(%d)", tskno);
			SVC_PERROR(rel_wai(tskid));
			break;
		case 'u':
			syslog(LOG_INFO, "#sus_tsk(%d)", tskno);
			SVC_PERROR(sus_tsk(tskid));
			break;
		case 'm':
			syslog(LOG_INFO, "#rsm_tsk(%d)", tskno);
			SVC_PERROR(rsm_tsk(tskid));
			break;
		case 'x':
			syslog(LOG_INFO, "#ras_tex(%d, 0x0001U)", tskno);
			SVC_PERROR(ras_tex(tskid, 0x0001U));
			break;
		case 'X':
			syslog(LOG_INFO, "#ras_tex(%d, 0x0002U)", tskno);
			SVC_PERROR(ras_tex(tskid, 0x0002U));
			break;
		case 'r':
			syslog(LOG_INFO, "#rot_rdq(three priorities)");
			SVC_PERROR(rot_rdq(HIGH_PRIORITY));
			SVC_PERROR(rot_rdq(MID_PRIORITY));
			SVC_PERROR(rot_rdq(LOW_PRIORITY));
			break;
		case 'c':
			syslog(LOG_INFO, "#sta_cyc(1)");
			SVC_PERROR(sta_cyc(CYCHDR1));
			break;
		case 'C':
			syslog(LOG_INFO, "#stp_cyc(1)");
			SVC_PERROR(stp_cyc(CYCHDR1));
			break;
		case 'b':
			syslog(LOG_INFO, "#sta_alm(1, 5000)");
			SVC_PERROR(sta_alm(ALMHDR1, 5000));
			break;
		case 'B':
			syslog(LOG_INFO, "#stp_alm(1)");
			SVC_PERROR(stp_alm(ALMHDR1));
			break;
		case '@':
			ctsk.tskatr = TA_NULL;
			ctsk.exinf = 3;
			ctsk.task = task;
			ctsk.itskpri = MID_PRIORITY;
			ctsk.stksz = STACK_SIZE;
			ctsk.stk = NULL;
			SVC_PERROR(TASK3 = acre_tsk(&ctsk));

			dtex.texatr = TA_NULL;
			dtex.texrtn = tex_routine;
			SVC_PERROR(def_tex(TASK3, &dtex));

			syslog(LOG_NOTICE, "task3 is created with tskid = %d.",
														(int_t) TASK3);
			break;
		case '!':
			syslog(LOG_INFO, "#del_tsk(%d)", tskno);
			SVC_PERROR(del_tsk(tskid));
			break;

		case 'V':
#ifdef TOPPERS_SUPPORT_GET_UTM
			SVC_PERROR(get_utm(&utime1));
			SVC_PERROR(get_utm(&utime2));
			syslog(LOG_NOTICE, "utime1 = %ld, utime2 = %ld",
										(ulong_t) utime1, (ulong_t) utime2);
#else /* TOPPERS_SUPPORT_GET_UTM */
			syslog(LOG_NOTICE, "get_utm is not supported.");
#endif /* TOPPERS_SUPPORT_GET_UTM */
			break;

		case 'v':
			SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO),
										LOG_UPTO(LOG_EMERG)));
			break;
		case 'q':
			SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_NOTICE),
										LOG_UPTO(LOG_EMERG)));
			break;

#ifdef BIT_KERNEL
		case ' ':
			SVC_PERROR(loc_cpu());
			{
				extern ER	bit_kernel(void);

				SVC_PERROR(ercd = bit_kernel());
				if (ercd >= 0) {
					syslog(LOG_NOTICE, "bit_kernel passed.");
				}
			}
			SVC_PERROR(unl_cpu());
			break;
#endif /* BIT_KERNEL */

		default:
			break;
		}
	} while (c != '\003' && c != 'Q');

	syslog(LOG_NOTICE, "Sample program ends.");
	SVC_PERROR(ext_ker());
	assert(0);
}
