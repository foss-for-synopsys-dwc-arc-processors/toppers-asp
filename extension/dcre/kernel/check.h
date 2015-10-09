/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: check.h 1970 2010-11-20 11:27:06Z ertl-hiro $
 */

/*
 *		macros for error check
 */

#ifndef TOPPERS_CHECK_H
#define TOPPERS_CHECK_H

/*
 *  check attribute (E_PAR)
 */
#define CHECK_RSATR(atr, valid_atr) {						\
	if (((atr) & ~(valid_atr)) != 0U) {						\
		ercd = E_RSATR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check the priority range
 */
#define VALID_TPRI(tpri)	(TMIN_TPRI <= (tpri) && (tpri) <= TMAX_TPRI)

#define VALID_DPRI(dpri)	(TMIN_DPRI <= (dpri) && (dpri) <= TMAX_DPRI)

#define VALID_MPRI(mpri)	(TMIN_MPRI <= (mpri) && (mpri) <= TMAX_MPRI)

#define VALID_ISRPRI(isrpri) \
				(TMIN_ISRPRI <= (isrpri) && (isrpri) <= TMAX_ISRPRI)

#ifndef VALID_INTPRI_CHGIPM
#define VALID_INTPRI_CHGIPM(intpri) \
				(TMIN_INTPRI <= (intpri) && (intpri) <= TIPM_ENAALL)
#endif /* VALID_INTPRI_CHGIPM */

/*
 *  check the task priority (E_PAR)
 */
#define CHECK_TPRI(tpri) {									\
	if (!VALID_TPRI(tpri)) {								\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

#define CHECK_TPRI_INI(tpri) {								\
	if (!(VALID_TPRI(tpri) || (tpri) == TPRI_INI)) {		\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

#define CHECK_TPRI_SELF(tpri) {								\
	if (!(VALID_TPRI(tpri) || (tpri) == TPRI_SELF)) {		\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check dataqueue priority (E_PAR)
 */
#define CHECK_DPRI(dpri) {									\
	if (!VALID_DPRI(dpri)) {								\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check message priority (E_PAR)
 */
#define CHECK_MPRI(mpri) {									\
	if (!VALID_MPRI(mpri)) {								\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check interrupt service routine priority (E_PAR)
 */
#define CHECK_ISRPRI(isrpri) {								\
	if (!VALID_ISRPRI(isrpri)) {							\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check the timeout parameter (E_PAR)
 */
#define CHECK_TMOUT(tmout) {								\
	if (!(TMO_FEVR <= (tmout))) {							\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check interrupt priority (E_PAR)
 */
#define CHECK_INTPRI_CHGIPM(intpri) {						\
	if (!VALID_INTPRI_CHGIPM(intpri)) {						\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check the interrupt no (E_PAR)
 */
#define CHECK_INTNO_CREISR(intno) {							\
	if (!VALID_INTNO_CREISR(intno)) {						\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

#define CHECK_INTNO_DISINT(intno) {							\
	if (!VALID_INTNO_DISINT(intno)) {						\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check aligned
 */
#define ALIGNED(val, align)		((((uintptr_t)(val)) & ((align) - 1U)) == 0U)

#ifdef CHECK_FUNC_ALIGN
#define FUNC_ALIGNED(func)		ALIGNED(func, CHECK_FUNC_ALIGN)
#else /* CHECK_FUNC_ALIGN */
#define FUNC_ALIGNED(func)		true
#endif /* CHECK_FUNC_ALIGN */

#ifdef CHECK_STKSZ_ALIGN
#define STKSZ_ALIGNED(stksz)	ALIGNED(stksz, CHECK_STKSZ_ALIGN)
#else /* CHECK_STKSZ_ALIGN */
#define STKSZ_ALIGNED(stksz)	true
#endif /* CHECK_STKSZ_ALIGN */

#ifdef CHECK_STACK_ALIGN
#define STACK_ALIGNED(stack)	ALIGNED(stack, CHECK_STACK_ALIGN)
#else /* CHECK_STACK_ALIGN */
#define STACK_ALIGNED(stack)	true
#endif /* CHECK_STACK_ALIGN */

#ifdef CHECK_MPF_ALIGN
#define MPF_ALIGNED(mpf)		ALIGNED(mpf, CHECK_MPF_ALIGN)
#else /* CHECK_MPF_ALIGN */
#define MPF_ALIGNED(mpf)		true
#endif /* CHECK_MPF_ALIGN */

#ifdef CHECK_MB_ALIGN
#define MB_ALIGNED(mb)			ALIGNED(mb, CHECK_MB_ALIGN)
#else /* CHECK_MB_ALIGN */
#define MB_ALIGNED(mb)			true
#endif /* CHECK_MB_ALIGN */

/*
 *  check if it is not NULL
 */
#ifdef CHECK_FUNC_NONNULL
#define FUNC_NONNULL(func)		((func) != NULL)
#else /* CHECK_FUNC_NONNULL */
#define FUNC_NONNULL(func)		true
#endif /* CHECK_FUNC_NONNULL */

#ifdef CHECK_STACK_NONNULL
#define STACK_NONNULL(stack)	((stack) != NULL)
#else /* CHECK_STACK_NONNULL */
#define STACK_NONNULL(stack)	true
#endif /* CHECK_STACK_NONNULL */

#ifdef CHECK_MPF_NONNULL
#define MPF_NONNULL(mpf)		((mpf) != NULL)
#else /* CHECK_MPF_NONNULL */
#define MPF_NONNULL(mpf)		true
#endif /* CHECK_MPF_NONNULL */

/*
 * check the start address of the function (E_PAR)
 */
#define CHECK_ALIGN_FUNC(func) {							\
	if (!FUNC_ALIGNED(func)) {								\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

#define CHECK_NONNULL_FUNC(func) {							\
	if (!FUNC_NONNULL(func)) {								\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check the stack size (E_PAR)
 */
#define CHECK_ALIGN_STKSZ(stksz) {							\
	if (!STKSZ_ALIGNED(stksz)) {							\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

#ifndef TARGET_MIN_STKSZ
#define TARGET_MIN_STKSZ	1U		/* check non-zero if undefined */
#endif /* TARGET_MIN_STKSZ */

#define CHECK_STKSZ_MIN(stksz) {							\
	if ((stksz) < TARGET_MIN_STKSZ) {						\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check the top address of the stack (E_PAR)
 */
#define CHECK_ALIGN_STACK(stack) {							\
	if (!STACK_ALIGNED(stack)) {							\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

#define CHECK_NONNULL_STACK(stack) {						\
	if (!STACK_NONNULL(stack)) {							\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check the starting address of the fixed-size memory pool area (E_PAR)
 */
#define CHECK_ALIGN_MPF(mpf) {								\
	if (!MPF_ALIGNED(mpf)) {								\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

#define CHECK_NONNULL_MPF(mpf) {							\
	if (!MPF_NONNULL(mpf)) {								\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check of the start address of the management area (E_PAR)
 */
#define CHECK_ALIGN_MB(mb) {								\
	if (!MB_ALIGNED(mb)) {									\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check other errors of the parameter (E_PAR)
 */
#define CHECK_PAR(exp) {									\
	if (!(exp)) {											\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
}

/*
 *  check the object id range
 */
#define VALID_TSKID(tskid)	(TMIN_TSKID <= (tskid) && (tskid) <= tmax_tskid)
#define VALID_SEMID(semid)	(TMIN_SEMID <= (semid) && (semid) <= tmax_semid)
#define VALID_FLGID(flgid)	(TMIN_FLGID <= (flgid) && (flgid) <= tmax_flgid)
#define VALID_DTQID(dtqid)	(TMIN_DTQID <= (dtqid) && (dtqid) <= tmax_dtqid)
#define VALID_PDQID(pdqid)	(TMIN_PDQID <= (pdqid) && (pdqid) <= tmax_pdqid)
#define VALID_MBXID(mbxid)	(TMIN_MBXID <= (mbxid) && (mbxid) <= tmax_mbxid)
#define VALID_MPFID(mpfid)	(TMIN_MPFID <= (mpfid) && (mpfid) <= tmax_mpfid)
#define VALID_CYCID(cycid)	(TMIN_CYCID <= (cycid) && (cycid) <= tmax_cycid)
#define VALID_ALMID(almid)	(TMIN_ALMID <= (almid) && (almid) <= tmax_almid)
#define VALID_ISRID(isrid)	(TMIN_ISRID <= (isrid) && (isrid) <= tmax_isrid)

/*
 *  check object id (E_ID)
 */
#define CHECK_TSKID(tskid) {								\
	if (!VALID_TSKID(tskid)) {								\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
}

#define CHECK_TSKID_SELF(tskid) {							\
	if (!(VALID_TSKID(tskid) || (tskid) == TSK_SELF)) {		\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
}

#define CHECK_SEMID(semid) {								\
	if (!VALID_SEMID(semid)) {								\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
}

#define CHECK_FLGID(flgid) {								\
	if (!VALID_FLGID(flgid)) {								\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
}

#define CHECK_DTQID(dtqid) {								\
	if (!VALID_DTQID(dtqid)) {								\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
}

#define CHECK_PDQID(pdqid) {								\
	if (!VALID_PDQID(pdqid)) {								\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
}

#define CHECK_MBXID(mbxid) {								\
	if (!VALID_MBXID(mbxid)) {								\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
}

#define CHECK_MPFID(mpfid) {								\
	if (!VALID_MPFID(mpfid)) {								\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
}

#define CHECK_CYCID(cycid) {								\
	if (!VALID_CYCID(cycid)) {								\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
}

#define CHECK_ALMID(almid) {								\
	if (!VALID_ALMID(almid)) {								\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
}

#define CHECK_ISRID(isrid) {								\
	if (!VALID_ISRID(isrid)) {								\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
}

/*
 *  check running context (E_CTX)
 */
#define CHECK_TSKCTX() {									\
	if (sense_context()) {									\
		ercd = E_CTX;										\
		goto error_exit;									\
	}														\
}

#define CHECK_INTCTX() {									\
	if (!sense_context()) {									\
		ercd = E_CTX;										\
		goto error_exit;									\
	}														\
}

/*
 *  check running context and cpu status (E_CTX)
 */
#define CHECK_TSKCTX_UNL() {								\
	if (sense_context() || t_sense_lock()) {				\
		ercd = E_CTX;										\
		goto error_exit;									\
	}														\
}

#define CHECK_INTCTX_UNL() {								\
	if (!sense_context() || i_sense_lock()) {				\
		ercd = E_CTX;										\
		goto error_exit;									\
	}														\
}

/*
 *  check whether a dispatch can be made (E_CTX)
 */
#define CHECK_DISPATCH() {									\
	if (sense_context() || t_sense_lock() || !dspflg) {		\
		ercd = E_CTX;										\
		goto error_exit;									\
	}														\
}

/*
 *  check other running context errors (E_CTX)
 */
#define CHECK_CTX(exp) {									\
	if (!(exp)) {											\
		ercd = E_CTX;										\
		goto error_exit;									\
	}														\
}

/*
 *  check whether the task is current task (E_ILUSE)
 */
#define CHECK_NONSELF(p_tcb) {								\
	if ((p_tcb) == p_runtsk) {								\
		ercd = E_ILUSE;										\
		goto error_exit;									\
	}														\
}

/*
 *  check other illeagl use (E_ILUSE)
 */
#define CHECK_ILUSE(exp) {									\
	if (!(exp)) {											\
		ercd = E_ILUSE;										\
		goto error_exit;									\
	}														\
}

/*
 *  check unsupported fuction (E_NOSPT)
 */
#define CHECK_NOSPT(exp) {									\
	if (!(exp)) {											\
		ercd = E_NOSPT;										\
		goto error_exit;									\
	}														\
}

#endif /* TOPPERS_CHECK_H */
