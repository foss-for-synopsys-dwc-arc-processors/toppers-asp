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
 *  @(#) $Id: kernel.h 1727 2010-01-31 14:12:36Z ertl-hiro $
 */

/*
 *		standard head file of TOPPERS/ASP kernel
 *
 *  service call declarations, data types, constants and macros of
 *  TOPPERS/ASP kernel.
 *
 *  When this head file is included in assembly language source file,
 *  the macro TOPPERS_MACRO_ONLY should be defined. Then, the
 *  definitions that are not Marcos will be excluded.
 *
 *  No files are required to be included before this head file.
 */

#ifndef TOPPERS_KERNEL_H
#define TOPPERS_KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *	TOPPERS common data types, constants and macros
 */
#include <t_stddef.h>

/*
 *  target-dependent part
 */
#include "target_kernel.h"

#ifndef TOPPERS_MACRO_ONLY

/*
 *  data type definitions
 */

/*
 *  type definitions of bit pattern and object id for some objects
 */
typedef	uint_t		TEXPTN;		/* bit pattern of task exception request */
typedef	uint_t		FLGPTN;		/* bit pattern of event flag */
typedef	uint_t		INTNO;		/* interrupt number */
typedef	uint_t		INHNO;		/* interrupt handler number */
typedef	uint_t		EXCNO;		/* CPU exception handler number*/

/*
 *  type definition of process units
 */
typedef void	(*TASK)(intptr_t exinf);
typedef void	(*TEXRTN)(TEXPTN texptn, intptr_t exinf);
typedef void	(*CYCHDR)(intptr_t exinf);
typedef void	(*ALMHDR)(intptr_t exinf);
typedef void	(*ISR)(intptr_t exinf);
typedef void	(*INTHDR)(void);
typedef void	(*EXCHDR)(void *p_excinf);
typedef void	(*INIRTN)(intptr_t exinf);
typedef void	(*TERRTN)(intptr_t exinf);

/*
 *  type definitions of memory area
 */
#ifndef TOPPERS_STK_T
#define TOPPERS_STK_T	intptr_t
#endif /* TOPPERS_STK_T */
typedef	TOPPERS_STK_T	STK_T;	/* type of stack area */

#ifndef TOPPERS_MPF_T
#define TOPPERS_MPF_T	intptr_t
#endif /* TOPPERS_MPF_T */
typedef	TOPPERS_MPF_T	MPF_T;	/* type of fixed-size memory pool area */

/*
 *  definition of message head
 */
typedef	struct t_msg {			/* message head of mailbox */
	struct t_msg	*pk_next;
} T_MSG;

typedef	struct t_msg_pri {		/* message head with priority */
	T_MSG	msgque;				/* message head */
	PRI		msgpri;				/* message priority */
} T_MSG_PRI;

/*
 *  definition of data packets for kernel objects reference
 */
typedef struct t_rtsk {
	STAT	tskstat;	/* task state */
	PRI		tskpri;		/* task current priority */
	PRI		tskbpri;	/* task base priority */
	STAT	tskwait;	/* reason for waiting */
	ID		wobjid;		/* object ID for which task is waiting*/
	TMO		lefttmo;	/* remaining time until timeout */
	uint_t	actcnt;		/* activation request count*/
	uint_t	wupcnt;		/* wake up requests count */
} T_RTSK;

typedef struct t_rtex {
	STAT	texstat;	/* task exception state */
	TEXPTN	pndptn;		/* pending exception code */
} T_RTEX;

typedef struct t_rsem {
	ID		wtskid;		/* ID of the task at the head of the semaphore's
   					  	   wait queue	*/
	uint_t	semcnt;		/* current semaphore resource count */
} T_RSEM;

typedef struct t_rflg {
	ID		wtskid;		/* ID of the task at the head of the eventflag's
   					  	   wait queue	*/
	FLGPTN	flgptn;		/* current eventflag bit pattern */
} T_RFLG;

typedef struct t_rdtq {
	ID		stskid;		/* ID of the task at the head of the data queue's
   					  	   send-wait queue	*/
	ID		rtskid;		/* ID of the task at the head of the data queue's
   					  	   receive-wait queue	*/
	uint_t	sdtqcnt;	/* the number of data elements in the data queue */
} T_RDTQ;

typedef struct t_rpdq {
	ID		stskid;		/* ID of the task at the head of the priority data queue's
   					  	   send-wait queue	*/
	ID		rtskid;		/* ID of the task at the head of the priority data queue's
   					  	   receive-wait queue	*/
	uint_t	spdqcnt;	/* the number of data elements in the priority data queue */
} T_RPDQ;

typedef struct t_rmbx {
	ID		wtskid;		/* ID of the task at the head of the mailbox's
   					  	   wait queue	*/
	T_MSG	*pk_msg;	/* start address of the message packet at the head of the message queue */
} T_RMBX;

typedef struct t_rmtx {
	ID		htskid;		/* ID of the task that locks the mutex */
	ID		wtskid;		/* ID of the task at the head of the mutex's
   					  	   wait queue	*/
} T_RMTX;

typedef struct t_rmpf {
	ID		wtskid;		/* ID of the task at the head of the fixed-sized memory pool's
   					  	   wait queue	*/
	uint_t	fblkcnt;	/* number of free memory blocks in the fixed-sized memory pool */
} T_RMPF;

typedef struct t_rcyc {
	STAT	cycstat;	/* cyclic handler operational state */
	RELTIM	lefttim;	/* time left before the next activation */
} T_RCYC;

typedef struct t_ralm {
	STAT	almstat;	/* alarm handler operational state */
	RELTIM	lefttim;	/* time left before the activation */
} T_RALM;

/*
 * declarations of service call
 */

/*
 *  task management functions
 */
extern ER		act_tsk(ID tskid) throw();
extern ER		iact_tsk(ID tskid) throw();
extern ER_UINT	can_act(ID tskid) throw();
extern ER		ext_tsk(void) throw();
extern ER		ter_tsk(ID tskid) throw();
extern ER		chg_pri(ID tskid, PRI tskpri) throw();
extern ER		get_pri(ID tskid, PRI *p_tskpri) throw();
extern ER		get_inf(intptr_t *p_exinf) throw();
extern ER		ref_tsk(ID tskid, T_RTSK *pk_rtsk) throw();

/*
 *  task synchronization functions
 */
extern ER		slp_tsk(void) throw();
extern ER		tslp_tsk(TMO tmout) throw();
extern ER		wup_tsk(ID tskid) throw();
extern ER		iwup_tsk(ID tskid) throw();
extern ER_UINT	can_wup(ID tskid) throw();
extern ER		rel_wai(ID tskid) throw();
extern ER		irel_wai(ID tskid) throw();
extern ER		sus_tsk(ID tskid) throw();
extern ER		rsm_tsk(ID tskid) throw();
extern ER		dly_tsk(RELTIM dlytim) throw();

/*
 *  task exception handling functions
 */
extern ER		ras_tex(ID tskid, TEXPTN rasptn) throw();
extern ER		iras_tex(ID tskid, TEXPTN rasptn) throw();
extern ER		dis_tex(void) throw();
extern ER		ena_tex(void) throw();
extern bool_t	sns_tex(void) throw();
extern ER		ref_tex(ID tskid, T_RTEX *pk_rtex) throw();

/*
 *  synchronization & communication functions
 */
extern ER		sig_sem(ID semid) throw();
extern ER		isig_sem(ID semid) throw();
extern ER		wai_sem(ID semid) throw();
extern ER		pol_sem(ID semid) throw();
extern ER		twai_sem(ID semid, TMO tmout) throw();
extern ER		ini_sem(ID semid) throw();
extern ER		ref_sem(ID semid, T_RSEM *pk_rsem) throw();

extern ER		set_flg(ID flgid, FLGPTN setptn) throw();
extern ER		iset_flg(ID flgid, FLGPTN setptn) throw();
extern ER		clr_flg(ID flgid, FLGPTN clrptn) throw();
extern ER		wai_flg(ID flgid, FLGPTN waiptn,
						MODE wfmode, FLGPTN *p_flgptn) throw();
extern ER		pol_flg(ID flgid, FLGPTN waiptn,
						MODE wfmode, FLGPTN *p_flgptn) throw();
extern ER		twai_flg(ID flgid, FLGPTN waiptn,
						MODE wfmode, FLGPTN *p_flgptn, TMO tmout) throw();
extern ER		ini_flg(ID flgid) throw();
extern ER		ref_flg(ID flgid, T_RFLG *pk_rflg) throw();

extern ER		snd_dtq(ID dtqid, intptr_t data) throw();
extern ER		psnd_dtq(ID dtqid, intptr_t data) throw();
extern ER		ipsnd_dtq(ID dtqid, intptr_t data) throw();
extern ER		tsnd_dtq(ID dtqid, intptr_t data, TMO tmout) throw();
extern ER		fsnd_dtq(ID dtqid, intptr_t data) throw();
extern ER		ifsnd_dtq(ID dtqid, intptr_t data) throw();
extern ER		rcv_dtq(ID dtqid, intptr_t *p_data) throw();
extern ER		prcv_dtq(ID dtqid, intptr_t *p_data) throw();
extern ER		trcv_dtq(ID dtqid, intptr_t *p_data, TMO tmout) throw();
extern ER		ini_dtq(ID dtqid) throw();
extern ER		ref_dtq(ID dtqid, T_RDTQ *pk_rdtq) throw();

extern ER		snd_pdq(ID pdqid, intptr_t data, PRI datapri) throw();
extern ER		psnd_pdq(ID pdqid, intptr_t data, PRI datapri) throw();
extern ER		ipsnd_pdq(ID pdqid, intptr_t data, PRI datapri) throw();
extern ER		tsnd_pdq(ID pdqid, intptr_t data,
										PRI datapri, TMO tmout) throw();
extern ER		rcv_pdq(ID pdqid, intptr_t *p_data, PRI *p_datapri) throw();
extern ER		prcv_pdq(ID pdqid, intptr_t *p_data, PRI *p_datapri) throw();
extern ER		trcv_pdq(ID pdqid, intptr_t *p_data,
										PRI *p_datapri, TMO tmout) throw();
extern ER		ini_pdq(ID pdqid) throw();
extern ER		ref_pdq(ID pdqid, T_RPDQ *pk_rpdq) throw();

extern ER		snd_mbx(ID mbxid, T_MSG *pk_msg) throw();
extern ER		rcv_mbx(ID mbxid, T_MSG **ppk_msg) throw();
extern ER		prcv_mbx(ID mbxid, T_MSG **ppk_msg) throw();
extern ER		trcv_mbx(ID mbxid, T_MSG **ppk_msg, TMO tmout) throw();
extern ER		ini_mbx(ID mbxid) throw();
extern ER		ref_mbx(ID mbxid, T_RMBX *pk_rmbx) throw();

extern ER		loc_mtx(ID mtxid) throw();
extern ER		ploc_mtx(ID mtxid) throw();
extern ER		tloc_mtx(ID mtxid, TMO tmout) throw();
extern ER		unl_mtx(ID mtxid) throw();
extern ER		ini_mtx(ID mtxid) throw();
extern ER		ref_mtx(ID mtxid, T_RMTX *pk_rmtx) throw();

/*
 *  fixed-sized memory pool management functions
 */
extern ER		get_mpf(ID mpfid, void **p_blk) throw();
extern ER		pget_mpf(ID mpfid, void **p_blk) throw();
extern ER		tget_mpf(ID mpfid, void **p_blk, TMO tmout) throw();
extern ER		rel_mpf(ID mpfid, void *blk) throw();
extern ER		ini_mpf(ID mpfid) throw();
extern ER		ref_mpf(ID mpfid, T_RMPF *pk_rmpf) throw();

/*
 *  time management functions
 */
extern ER		get_tim(SYSTIM *p_systim) throw();
extern ER		get_utm(SYSUTM *p_sysutm) throw();

extern ER		sta_cyc(ID cycid) throw();
extern ER		stp_cyc(ID cycid) throw();
extern ER		ref_cyc(ID cycid, T_RCYC *pk_rcyc) throw();

extern ER		sta_alm(ID almid, RELTIM almtim) throw();
extern ER		ista_alm(ID almid, RELTIM almtim) throw();
extern ER		stp_alm(ID almid) throw();
extern ER		istp_alm(ID almid) throw();
extern ER		ref_alm(ID almid, T_RALM *pk_ralm) throw();

/*
 *  system state management functions
 */
extern ER		rot_rdq(PRI tskpri) throw();
extern ER		irot_rdq(PRI tskpri) throw();
extern ER		get_tid(ID *p_tskid) throw();
extern ER		iget_tid(ID *p_tskid) throw();
extern ER		loc_cpu(void) throw();
extern ER		iloc_cpu(void) throw();
extern ER		unl_cpu(void) throw();
extern ER		iunl_cpu(void) throw();
extern ER		dis_dsp(void) throw();
extern ER		ena_dsp(void) throw();
extern bool_t	sns_ctx(void) throw();
extern bool_t	sns_loc(void) throw();
extern bool_t	sns_dsp(void) throw();
extern bool_t	sns_dpn(void) throw();
extern bool_t	sns_ker(void) throw();
extern ER		ext_ker(void) throw();

/*
 *  interrupt management functions
 */
extern ER		dis_int(INTNO intno) throw();
extern ER		ena_int(INTNO intno) throw();
extern ER		chg_ipm(PRI intpri) throw();
extern ER		get_ipm(PRI *p_intpri) throw();

/*
 *  CPU exception management functions
 */
extern bool_t	xsns_dpn(void *p_excinf) throw();
extern bool_t	xsns_xpn(void *p_excinf) throw();

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  definitions of object attribute
 */
#define TA_ACT			UINT_C(0x02)	/* task is activated after creation */

#define TA_TPRI			UINT_C(0x01)	/* task wait queue is in task priority order */
#define TA_MPRI			UINT_C(0x02)	/* message queue is in message priority order */

#define TA_WMUL			UINT_C(0x02)	/* multiple tasks are allowed to be in the waiting state for the eventflag*/
#define TA_CLR			UINT_C(0x04)	/* eventflag's bit pattern is cleared when a task is released from the 
 										   waiting state for that eventflag */

#define TA_STA			UINT_C(0x02)	/* cyclic handler is in an operational state after the creation */

#define TA_NONKERNEL	UINT_C(0x02)	/* interrupt is not managed by the kernel */

#define TA_ENAINT		UINT_C(0x01)	/* clear the interrupt disable flag */
#define TA_EDGE			UINT_C(0x02)	/* edge trig */

/*
 *  definitions of service call operation mode
 */
#define TWF_ORW			UINT_C(0x01)	/* OR waiting condition for an eventflag */
#define TWF_ANDW		UINT_C(0x02)	/* AND waiting condition for an eventflag */

/*
 *  definitions of object states
 */
#define TTS_RUN			UINT_C(0x01)	/* RUNNING state */
#define TTS_RDY			UINT_C(0x02)	/* READY state */
#define TTS_WAI			UINT_C(0x04)	/* WAITING state */
#define TTS_SUS			UINT_C(0x08)	/* SUSPENDED state */
#define TTS_WAS			UINT_C(0x0c)	/* WAITING-SUSPENDED state */
#define TTS_DMT			UINT_C(0x10)	/* DORMANT state */

#define TTW_SLP			UINT_C(0x0001)	/* sleeping state */
#define TTW_DLY			UINT_C(0x0002)	/* delayed state */
#define TTW_SEM			UINT_C(0x0004)	/* waiting state for a semaphore resource */
#define TTW_FLG			UINT_C(0x0008)	/* waiting state for an eventflag */
#define TTW_SDTQ		UINT_C(0x0010)	/* sending waiting state for a data queue */
#define TTW_RDTQ		UINT_C(0x0020)	/* receive waiting state for a data queue */
#define TTW_SPDQ		UINT_C(0x0100)	/* sending waiting state for a priority data queue */
#define TTW_RPDQ		UINT_C(0x0200)	/* receive waiting state for a priority data queue */
#define TTW_MBX			UINT_C(0x0040)	/* receive waiting state for a mailbox */
#define TTW_MPF			UINT_C(0x2000)	/* receive waiting state for a fixed-sized memory block */

#define TTEX_ENA		UINT_C(0x01)	/* task exception enabled state */
#define TTEX_DIS		UINT_C(0x02)	/* task exception disabled state */

#define TCYC_STP		UINT_C(0x01)	/* cyclic handler is in a non-operational state */
#define TCYC_STA		UINT_C(0x02)	/* cyclic handler is in an operational state */

#define TALM_STP		UINT_C(0x01)	/* alarm handler is in a non-operational state */
#define TALM_STA		UINT_C(0x02)	/* alarm handler is in an operational state */

/*
 *  definitions of other constants
 */
#define TSK_SELF		0			/* specifying invoking task */
#define TSK_NONE		0			/* no applicable task */

#define TPRI_SELF		0			/* specifying the base priority of the invoking task */
#define TPRI_INI		0			/* specifying the initial priority of the task */

#define TIPM_ENAALL		0			/* remove all the interrupt priority mask */

/*
 *  kernel configuration constants and macros
 */

/*
 *  the supported functions
 */
#ifdef TOPPERS_TARGET_SUPPORT_DIS_INT
#define TOPPERS_SUPPORT_DIS_INT			/* dis_int is supported */
#endif /* TOPPERS_TARGET_SUPPORT_DIS_INT */

#ifdef TOPPERS_TARGET_SUPPORT_ENA_INT
#define TOPPERS_SUPPORT_ENA_INT			/*  ena_int is supported */
#endif /* TOPPERS_TARGET_SUPPORT_ENA_INT */

#ifdef TOPPERS_TARGET_SUPPORT_GET_UTM
#define TOPPERS_SUPPORT_GET_UTM			/*  get_utm is supported */
#endif /* TOPPERS_TARGET_SUPPORT_GET_UTM */

/*
 *  range of priority
 */
#define TMIN_TPRI		1			/* minimum task priority (the highest)*/
#define TMAX_TPRI		16			/* maximum task priority (the lowest)*/
#define TMIN_DPRI		1			/* minimum data priority (the highest)*/
#define TMAX_DPRI		16			/* maximum data priority (the lowest)*/
#define TMIN_MPRI		1			/* minimum message priority (the highest)*/
#define TMAX_MPRI		16			/* maximum message priority (the lowest)*/
#define TMIN_ISRPRI		1			/* minimum interrupt service routine priority */
#define TMAX_ISRPRI		16			/* maximum interrupt service routine priority */

/*
 *  version information
 */
#define TKERNEL_MAKER	UINT_C(0x0118)	/* kernel maker code */
#define TKERNEL_PRID	UINT_C(0x0007)	/* ID of the kernel */
#define TKERNEL_SPVER	UINT_C(0xf512)	/* kernel specification version number */
#define TKERNEL_PRVER	UINT_C(0x1050)	/* kernel version number */

/*
 *  maximum nesting/queueing ount
 */
#define TMAX_ACTCNT		UINT_C(1)		/* maximum activation request count */
#define TMAX_WUPCNT		UINT_C(1)		/* maximum wakeup request count */

/*
 * number of bits in bit patterns
 */
#ifndef TBIT_TEXPTN					/* number of bits in the task exceptions code */
#define TBIT_TEXPTN		(sizeof(TEXPTN) * CHAR_BIT)
#endif /* TBIT_TEXPTN */

#ifndef TBIT_FLGPTN					/* number of bits in an eventflag */
#define TBIT_FLGPTN		(sizeof(FLGPTN) * CHAR_BIT)
#endif /* TBIT_FLGPTN */

/*
 *  macros for memory partitioning
 *
 *   in TOPPERS_ROUND_SZ TOPPERS_COUNT_SZ
 *   unit should be the power of 2
 */
#ifndef TOPPERS_COUNT_SZ
#define TOPPERS_COUNT_SZ(sz, unit)	(((sz) + (unit) - 1) / (unit))
#endif /* TOPPERS_COUNT_SZ */
#ifndef TOPPERS_ROUND_SZ
#define TOPPERS_ROUND_SZ(sz, unit)	(((sz) + (unit) - 1) & ~((unit) - 1))
#endif /* TOPPERS_ROUND_SZ */

#define COUNT_STK_T(sz)		TOPPERS_COUNT_SZ(sz, sizeof(STK_T))
#define ROUND_STK_T(sz)		TOPPERS_ROUND_SZ(sz, sizeof(STK_T))

#define COUNT_MPF_T(blksz)	TOPPERS_COUNT_SZ(blksz, sizeof(MPF_T))
#define ROUND_MPF_T(blksz)	TOPPERS_ROUND_SZ(blksz, sizeof(MPF_T))

/*
 *  other configuration constants
 */
#define TMAX_MAXSEM		UINT_MAX	/* the maximum number of semaphore resources */

#ifdef __cplusplus
}
#endif

#endif /* TOPPERS_KERNEL_H */
