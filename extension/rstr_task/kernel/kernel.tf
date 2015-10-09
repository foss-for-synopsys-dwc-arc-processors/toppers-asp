$ ======================================================================
$ 
$   TOPPERS/ASP Kernel
$       Toyohashi Open Platform for Embedded Real-Time Systems/
$       Advanced Standard Profile Kernel
$ 
$   Copyright (C) 2007 by TAKAGI Nobuhisa
$   Copyright (C) 2007-2011 by Embedded and Real-Time Systems Laboratory
$               Graduate School of Information Science, Nagoya Univ., JAPAN
$  
$	The above copyright holders grant permission gratis to use,
$	duplicate, modify, or redistribute (hereafter called use) this
$	software (including the one made by modifying this software),
$	provided that the following four conditions (1) through (4) are
$	satisfied.
$
$	(1) When this software is used in the form of source code, the above
$    	copyright notice, this use conditions, and the disclaimer shown
$    	below must be retained in the source code without modification.
$
$	(2) When this software is redistributed in the forms usable for the
$    	development of other software, such as in library form, the above
$    	copyright notice, this use conditions, and the disclaimer shown
$    	below must be shown without modification in the document provided
$    	with the redistributed software, such as the user manual.
$
$	(3) When this software is redistributed in the forms unusable for the
$    	development of other software, such as the case when the software
$    	is embedded in a piece of equipment, either of the following two
$   	 conditions must be satisfied:
$
$  		(a) The above copyright notice, this use conditions, and the
$         	disclaimer shown below must be shown without modification in
$     		the document provided with the redistributed software, such as
$      		the user manual.
$
$ 		(b) How the software is to be redistributed must be reported to the
$     		TOPPERS Project according to the procedure described
$     		separately.
$
$	(4) The above copyright holders and the TOPPERS Project are exempt
$    	from responsibility for any type of damage directly or indirectly
$   	caused from the use of this software and are indemnified by any
$    	users or end users of this software from any and all causes of
$    	action whatsoever.
$
$	THIS SOFTWARE IS PROVIDED "AS IS." THE ABOVE COPYRIGHT HOLDERS AND
$	THE TOPPERS PROJECT DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES,
$	INCLUDING, BUT NOT LIMITED TO, ITS APPLICABILITY TO A PARTICULAR
$	PURPOSE. IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS AND THE
$	TOPPERS PROJECT BE LIABLE FOR ANY TYPE OF DAMAGE DIRECTLY OR
$	INDIRECTLY CAUSED FROM THE USE OF THIS SOFTWARE.
$ 
$   $Id: kernel.tf 2024 2011-01-02 08:59:23Z ertl-hiro $
$  
$ =====================================================================

$ =====================================================================
$ generate kernel_cfg.h
$ =====================================================================

$FILE "kernel_cfg.h"$
/* kernel_cfg.h */$NL$
#ifndef TOPPERS_KERNEL_CFG_H$NL$
#define TOPPERS_KERNEL_CFG_H$NL$
$NL$
#define TNUM_TSKID	$LENGTH(TSK.ID_LIST)$$NL$
#define TNUM_SEMID	$LENGTH(SEM.ID_LIST)$$NL$
#define TNUM_FLGID	$LENGTH(FLG.ID_LIST)$$NL$
#define TNUM_DTQID	$LENGTH(DTQ.ID_LIST)$$NL$
#define TNUM_PDQID	$LENGTH(PDQ.ID_LIST)$$NL$
#define TNUM_MBXID	$LENGTH(MBX.ID_LIST)$$NL$
#define TNUM_MPFID	$LENGTH(MPF.ID_LIST)$$NL$
#define TNUM_CYCID	$LENGTH(CYC.ID_LIST)$$NL$
#define TNUM_ALMID	$LENGTH(ALM.ID_LIST)$$NL$
$NL$
$FOREACH id TSK.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id SEM.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id FLG.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id DTQ.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id PDQ.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id MBX.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id MPF.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id CYC.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id ALM.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$NL$
#endif /* TOPPERS_KERNEL_CFG_H */$NL$

$ =====================================================================
$ generate kernel_cfg.c
$ =====================================================================

$FILE "kernel_cfg.c"$
/* kernel_cfg.c */$NL$
#include "kernel/kernel_int.h"$NL$
#include "kernel_cfg.h"$NL$
$NL$
#ifndef TOPPERS_EMPTY_LABEL$NL$
#define TOPPERS_EMPTY_LABEL(x,y) x y[0]$NL$
#endif$NL$
$NL$
#if TKERNEL_PRID != 0x07u$NL$
#error The kernel does not match this configuration file.$NL$
#endif$NL$
$NL$

$ 
$  include directives  (#include)
$ 
/*$NL$
$SPC$*  Include Directives (#include)$NL$
$SPC$*/$NL$
$NL$
$INCLUDES$
$NL$

$ 
$  variables for object ID
$ 
$IF USE_EXTERNAL_ID$
	/*$NL$
	$SPC$*  Variables for Object ID$NL$
	$SPC$*/$NL$
	$NL$
	$FOREACH id TSK.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id SEM.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id FLG.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id DTQ.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id PDQ.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id MBX.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id MPF.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id CYC.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id ALM.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
$END$

$ 
$  default definitions of trace log macros
$ 
/*$NL$
$SPC$*  Default Definitions of Trace Log Macros$NL$
$SPC$*/$NL$
$NL$
#ifndef LOG_ISR_ENTER$NL$
#define LOG_ISR_ENTER(intno)$NL$
#endif /* LOG_ISR_ENTER */$NL$
$NL$
#ifndef LOG_ISR_LEAVE$NL$
#define LOG_ISR_LEAVE(intno)$NL$
#endif /* LOG_ISR_LEAVE */$NL$
$NL$

$ 
$  task
$ 
/*$NL$
$SPC$*  Task Management Functions$NL$
$SPC$*/$NL$
$NL$

$  check whether at least 1 task is registered
$IF !LENGTH(TSK.ID_LIST)$
	$ERROR$$FORMAT(_("no task is registered"))$$END$
$END$

$ the max number of task id
const ID _kernel_tmax_tskid = (TMIN_TSKID + TNUM_TSKID - 1);$NL$
$NL$

$ error check
$FOREACH tskid TSK.ID_LIST$
$	// if tskatr is not [TA_ACT] (E_RSATR)
	$IF (TSK.TSKATR[tskid] & ~(TA_ACT|TA_RSTR|TARGET_TSKATR)) != 0$
		$ERROR TSK.TEXT_LINE[tskid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "tskatr", TSK.TSKATR[tskid], tskid, "CRE_TSK")$$END$
	$END$

$	// if (TMIN_TPRI <= itskpri && itskpri <= TMAX_TPRI) is not matched (E_PAR)
	$IF !(TMIN_TPRI <= TSK.ITSKPRI[tskid] && TSK.ITSKPRI[tskid] <= TMAX_TPRI)$
		$ERROR TSK.TEXT_LINE[tskid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "itskpri", TSK.ITSKPRI[tskid], tskid, "CRE_TSK")$$END$
	$END$

$ 	// if texatr is not TA_NULL (also not TA_ACT) (E_RSATR)
	$IF LENGTH(TSK.TEXATR[tskid]) && TSK.TEXATR[tskid] != 0$
		$ERROR DEF_TEX.TEXT_LINE[tskid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "texatr", TSK.TEXATR[tskid], tskid, "DEF_TEX")$$END$
	$END$
$END$

$ check and generate the task stack
$FOREACH tskid TSK.ID_LIST$
$	// if stksz is 0, or is smaller than (TARGET_MIN_STKSZ) (E_PAR)
	$IF TSK.STKSZ[tskid] == 0 || (TARGET_MIN_STKSZ
									&& TSK.STKSZ[tskid] < TARGET_MIN_STKSZ)$
		$ERROR TSK.TEXT_LINE[tskid]$E_PAR: $FORMAT(_("%1% `%2%\' of `%3%\' in %4% is too small"), "stksz", TSK.STKSZ[tskid], tskid, "CRE_TSK")$$END$
	$END$
$ 	// if stksz is not aligned (E_PAR)
	$IF !EQ(TSK.STK[tskid], "NULL") && CHECK_STKSZ_ALIGN
							&& (TSK.STKSZ[tskid] & (CHECK_STKSZ_ALIGN - 1))$
		$ERROR TSK.TEXT_LINE[tskid]$E_PAR: $FORMAT(_("%1% `%2%\' of `%3%\' in %4% is not aligned"), "stksz", TSK.STKSZ[tskid], tskid, "CRE_TSK")$$END$
	$END$
$END$

$ generate the task stack
$FOREACH tskid TSK.ID_LIST$
	$IF EQ(TSK.STK[tskid], "NULL")$
		$IF (TSK.TSKATR[tskid] & TA_RSTR) == 0$
			static STK_T _kernel_stack_$tskid$[COUNT_STK_T($TSK.STKSZ[tskid]$)];$NL$
			$TSK.TINIB_STKSZ[tskid] = FORMAT("ROUND_STK_T(%1%)", TSK.STKSZ[tskid])$
			$TSK.TINIB_STK[tskid] = CONCAT("_kernel_stack_", tskid)$
		$ELSE$
			$TSK.TINIB_STKSZ[tskid] = FORMAT("ROUND_STK_T(%1%)", TSK.STKSZ[tskid])$
			$TSK.TINIB_STK[tskid] = CONCAT("_kernel_shared_stack_", +TSK.ITSKPRI[tskid])$
$ 			// calculate the commmon task stack size for restricted tasks  
			$IF !LENGTH(shared_stack_size[TSK.ITSKPRI[tskid]])
					|| shared_stack_size[TSK.ITSKPRI[tskid]] < TSK.STKSZ[tskid]$
				$shared_stack_size[TSK.ITSKPRI[tskid]] = TSK.STKSZ[tskid]$
			$END$
		$END$
	$ELSE$
		$TSK.TINIB_STKSZ[tskid] = TSK.STKSZ[tskid]$
		$TSK.TINIB_STK[tskid] = TSK.STK[tskid]$
	$END$
$END$
$FOREACH tskpri RANGE(TMIN_TPRI, TMAX_TPRI)$
	$IF LENGTH(shared_stack_size[tskpri])$
		static STK_T _kernel_shared_stack_$tskpri$[COUNT_STK_T($shared_stack_size[tskpri]$)];$NL$
	$END$
$END$
$NL$

$ generate the task initialization block table of statically created tasks (at least one task exists)
const TINIB _kernel_tinib_table[TNUM_TSKID] = {$NL$
$JOINEACH tskid TSK.ID_LIST ",\n"$
$	// task attribute, extended information, entry, init priority
	$TAB${
	$SPC$($TSK.TSKATR[tskid]$), (intptr_t)($TSK.EXINF[tskid]$),
	$SPC$((TASK)($TSK.TASK[tskid]$)), INT_PRIORITY($TSK.ITSKPRI[tskid]$),

$	// task init context block, task stack area
	$IF USE_TSKINICTXB$
		$GENERATE_TSKINICTXB(tskid)$
	$ELSE$
		$SPC$$TSK.TINIB_STKSZ[tskid]$, $TSK.TINIB_STK[tskid]$,
	$END$

$	//  the attribute and entry of task exception routine
	$SPC$($ALT(TSK.TEXATR[tskid],"TA_NULL")$), ($ALT(TSK.TEXRTN[tskid],"NULL")$) }
$END$$NL$
};$NL$
$NL$

$ generate the task control block table for all tasks
TCB _kernel_tcb_table[TNUM_TSKID];$NL$
$NL$

$ generate task order table
const ID _kernel_torder_table[TNUM_TSKID] = {$NL$
$TAB$$JOINEACH tskid TSK.ORDER_LIST ", "$$tskid$$END$$NL$
};$NL$
$NL$

$ 
$  semaphore
$
/*$NL$
$SPC$*  Semaphore Functions$NL$
$SPC$*/$NL$
$NL$

$ the max number of semaphore id
const ID _kernel_tmax_semid = (TMIN_SEMID + TNUM_SEMID - 1);$NL$
$NL$

$ generate the initialization block table of statically created semaphores
$IF LENGTH(SEM.ID_LIST)$
	const SEMINIB _kernel_seminib_table[TNUM_SEMID] = {$NL$
	$JOINEACH semid SEM.ID_LIST ",\n"$
$		// if sematr is not [TA_TPRI] (E_RSATR)
		$IF (SEM.SEMATR[semid] & ~TA_TPRI) != 0$
			$ERROR SEM.TEXT_LINE[semid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "sematr", SEM.SEMATR[semid], semid, "CRE_SEM")$$END$
		$END$

$		// if (0 <= isemcnt && isemcnt <= maxsem) is not matched (E_PAR)
		$IF !(0 <= SEM.ISEMCNT[semid] && SEM.ISEMCNT[semid] <= SEM.MAXSEM[semid])$
			$ERROR SEM.TEXT_LINE[semid]$E_PAR: $FORMAT(_("too large %1% `%2%\' of `%3%\' in %4%"), "isemcnt", SEM.ISEMCNT[semid], semid, "CRE_SEM")$$END$
		$END$

$		// if (1 <= maxsem && maxsem <= TMAX_MAXSEM) is not matched (E_PAR)
		$IF !(1 <= SEM.MAXSEM[semid] && SEM.MAXSEM[semid] <= TMAX_MAXSEM)$
			$ERROR SEM.TEXT_LINE[semid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "maxsem", SEM.MAXSEM[semid], semid, "CRE_SEM")$$END$
		$END$

$		// semaphore initialization block
		$TAB${ ($SEM.SEMATR[semid]$), ($SEM.ISEMCNT[semid]$), ($SEM.MAXSEM[semid]$) }
	$END$$NL$
	};$NL$
	$NL$

$	// generate the semaphore control block table for all semaphores
	SEMCB _kernel_semcb_table[TNUM_SEMID];$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const SEMINIB, _kernel_seminib_table);$NL$
	TOPPERS_EMPTY_LABEL(SEMCB, _kernel_semcb_table);$NL$
$END$$NL$

$ 
$  eventflag
$ 
/*$NL$
$SPC$*  Eventflag Functions$NL$
$SPC$*/$NL$
$NL$

$ the max number of eventflag id
const ID _kernel_tmax_flgid = (TMIN_FLGID + TNUM_FLGID - 1);$NL$
$NL$

$  generate the initialization block table of statically created eventflags
$IF LENGTH(FLG.ID_LIST)$
	const FLGINIB _kernel_flginib_table[TNUM_FLGID] = {$NL$
	$JOINEACH flgid FLG.ID_LIST ",\n"$
$		//  if flgatr is not [TA_TPRI | TA_WUL | TA_CLR] (E_RSATR)
		$IF (FLG.FLGATR[flgid] & ~(TA_TPRI|TA_WMUL|TA_CLR)) != 0$
			$ERROR FLG.TEXT_LINE[flgid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "flgatr", FLG.FLGATR[flgid], flgid, "CRE_FLG")$$END$
		$END$

$		// eventflag initialization block
		$TAB${ ($FLG.FLGATR[flgid]$), ($FLG.IFLGPTN[flgid]$) }
	$END$$NL$
	};$NL$
	$NL$

$	//  generate the eventflag control block table for all eventflags
	FLGCB _kernel_flgcb_table[TNUM_FLGID];$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const FLGINIB, _kernel_flginib_table);$NL$
	TOPPERS_EMPTY_LABEL(FLGCB, _kernel_flgcb_table);$NL$
$END$$NL$

$ 
$   dataqueue
$ 
/*$NL$
$SPC$*  Dataqueue Functions$NL$
$SPC$*/$NL$
$NL$

$ the max number of dataqueue id
const ID _kernel_tmax_dtqid = (TMIN_DTQID + TNUM_DTQID - 1);$NL$
$NL$

$IF LENGTH(DTQ.ID_LIST)$
	$FOREACH dtqid DTQ.ID_LIST$
$		// if dtqatr is not [TA_TPRI] (E_RSATR)
		$IF (DTQ.DTQATR[dtqid] & ~TA_TPRI) != 0$
			$ERROR DTQ.TEXT_LINE[dtqid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "dtqatr", DTQ.DTQATR[dtqid], dtqid, "CRE_DTQ")$$END$
		$END$

$		// if dtqmb is not NULL (E_NOSPT)
		$IF !EQ(DTQ.DTQMB[dtqid], "NULL")$
			$ERROR DTQ.TEXT_LINE[dtqid]$E_NOSPT: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "dtqmb", DTQ.DTQMB[dtqid], dtqid, "CRE_DTQ")$$END$
		$END$

$		// dataqueue management block area
		$IF DTQ.DTQCNT[dtqid]$
			static DTQMB _kernel_dtqmb_$dtqid$[$DTQ.DTQCNT[dtqid]$];$NL$
		$END$
	$END$

$	// generate the initialization block table of statically created dataqueues
	const DTQINIB _kernel_dtqinib_table[TNUM_DTQID] = {$NL$
	$JOINEACH dtqid DTQ.ID_LIST ",\n"$
		$TAB${ ($DTQ.DTQATR[dtqid]$), ($DTQ.DTQCNT[dtqid]$), $IF DTQ.DTQCNT[dtqid]$(_kernel_dtqmb_$dtqid$)$ELSE$NULL$END$ }
	$END$$NL$
	};$NL$
	$NL$

$	// generate the dataqueue control block table for all dataqueues
	DTQCB _kernel_dtqcb_table[TNUM_DTQID];$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const DTQINIB, _kernel_dtqinib_table);$NL$
	TOPPERS_EMPTY_LABEL(DTQCB, _kernel_dtqcb_table);$NL$
$END$$NL$

$ 
$   priority dataqueue
$ 
/*$NL$
$SPC$*  Priority Dataqueue Functions$NL$
$SPC$*/$NL$
$NL$

$ the max number of priority dataqueue id
const ID _kernel_tmax_pdqid = (TMIN_PDQID + TNUM_PDQID - 1);$NL$
$NL$

$IF LENGTH(PDQ.ID_LIST)$
	$FOREACH pdqid PDQ.ID_LIST$
$		// if pdqatr is not [TA_TPRI] (E_RSATR)
		$IF (PDQ.PDQATR[pdqid] & ~TA_TPRI) != 0$
			$ERROR PDQ.TEXT_LINE[pdqid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "pdqatr", PDQ.PDQATR[pdqid], pdqid, "CRE_PDQ")$$END$
		$END$

$		// if (TMIN_DPRI <= maxdpri && maxdpri <= TMAX_DPRI) is not matched (E_PAR)
		$IF !(TMIN_DPRI <= PDQ.MAXDPRI[pdqid] && PDQ.MAXDPRI[pdqid] <= TMAX_DPRI)$
			$ERROR PDQ.TEXT_LINE[pdqid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "maxdpri", PDQ.MAXDPRI[pdqid], pdqid, "CRE_PDQ")$$END$
		$END$

$		// if pdqmb is not NULL (E_NOSPT)
		$IF !EQ(PDQ.PDQMB[pdqid], "NULL")$
			$ERROR PDQ.TEXT_LINE[pdqid]$E_NOSPT: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "pdqmb", PDQ.PDQMB[pdqid], pdqid, "CRE_PDQ")$$END$
		$END$

$		// priority dataqueue management block area
		$IF PDQ.PDQCNT[pdqid]$
			static PDQMB _kernel_pdqmb_$pdqid$[$PDQ.PDQCNT[pdqid]$];$NL$
		$END$
	$END$

$	// generate the initialization block table of statically created priority dataqueues
	const PDQINIB _kernel_pdqinib_table[TNUM_PDQID] = {$NL$
	$JOINEACH pdqid PDQ.ID_LIST ",\n"$
		$TAB${ ($PDQ.PDQATR[pdqid]$), ($PDQ.PDQCNT[pdqid]$), ($PDQ.MAXDPRI[pdqid]$), $IF PDQ.PDQCNT[pdqid]$(_kernel_pdqmb_$pdqid$)$ELSE$NULL$END$ }
	$END$$NL$
	};$NL$
	$NL$

$	// generate the priority dataqueue control block table for all priority dataqueues
	PDQCB _kernel_pdqcb_table[TNUM_PDQID];$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const PDQINIB, _kernel_pdqinib_table);$NL$
	TOPPERS_EMPTY_LABEL(PDQCB, _kernel_pdqcb_table);$NL$
$END$$NL$

$ 
$  mailbox
$  
/*$NL$
$SPC$*  Mailbox Functions$NL$
$SPC$*/$NL$
$NL$

$ the max number of mailbox id
const ID _kernel_tmax_mbxid = (TMIN_MBXID + TNUM_MBXID - 1);$NL$
$NL$

$ generate the initialization block table of statically created mailboxs
$IF LENGTH(MBX.ID_LIST)$
	const MBXINIB _kernel_mbxinib_table[TNUM_MBXID] = {$NL$
	$JOINEACH mbxid MBX.ID_LIST ",\n"$
$		// if mbxatr is not [TA_TPRI] | [TA_MPRI] (E_RSATR)
		$IF (MBX.MBXATR[mbxid] & ~(TA_TPRI|TA_MPRI)) != 0$
			$ERROR MBX.TEXT_LINE[mbxid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "mbxatr", MBX.MBXATR[mbxid], mbxid, "CRE_MBX")$$END$
		$END$

$		// if (TMIN_MPRI <= maxmpri && maxmpri <= TMAX_MPRI) is not matched (E_PAR)
		$IF !(TMIN_MPRI <= MBX.MAXMPRI[mbxid] && MBX.MAXMPRI[mbxid] <= TMAX_MPRI)$
			$ERROR MBX.TEXT_LINE[mbxid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "maxmpri", MBX.MAXMPRI[mbxid], mbxid, "CRE_MBX")$$END$
		$END$

$		// if mprihd is NULL (E_NOSPT)
		$IF !EQ(MBX.MPRIHD[mbxid], "NULL")$
			$ERROR MBX.TEXT_LINE[mbxid]$E_NOSPT: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "mprihd", MBX.MPRIHD[mbxid], mbxid, "CRE_MBX")$$END$
		$END$

$		// maibox initialization block
		$TAB${ ($MBX.MBXATR[mbxid]$), ($MBX.MAXMPRI[mbxid]$) }
	$END$$NL$
	};$NL$
	$NL$

$	// generate the mailbox control block table for all mailboxs
	MBXCB _kernel_mbxcb_table[TNUM_MBXID];$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const MBXINIB, _kernel_mbxinib_table);$NL$
	TOPPERS_EMPTY_LABEL(MBXCB, _kernel_mbxcb_table);$NL$
$END$$NL$

$ 
$   fixed-sized memorypool
$  
/*$NL$
$SPC$*  Fixed-sized Memorypool Functions$NL$
$SPC$*/$NL$
$NL$

$ the max number of fixed-sized memorypool id
const ID _kernel_tmax_mpfid = (TMIN_MPFID + TNUM_MPFID - 1);$NL$
$NL$

$IF LENGTH(MPF.ID_LIST)$
	$FOREACH mpfid MPF.ID_LIST$
$		// if mpfatr is not [TA_TPRI] (E_RSATR)
		$IF (MPF.MPFATR[mpfid] & ~TA_TPRI) != 0$
			$ERROR MPF.TEXT_LINE[mpfid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "mpfatr", MPF.MPFATR[mpfid], mpfid, "CRE_MPF")$$END$
		$END$

$		// if blkcnt is 0 (E_PAR)
		$IF MPF.BLKCNT[mpfid] == 0$
			$ERROR MPF.TEXT_LINE[mpfid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "blkcnt", MPF.BLKCNT[mpfid], mpfid, "CRE_MPF")$$END$
		$END$

$		// if blksz is 0 (E_PAR)
		$IF MPF.BLKSZ[mpfid] == 0$
			$ERROR MPF.TEXT_LINE[mpfid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "blksz", MPF.BLKSZ[mpfid], mpfid, "CRE_MPF")$$END$
		$END$

$		//  generate the fixed-sized memorypool area
		$IF EQ(MPF.MPF[mpfid], "NULL")$
			static MPF_T _kernel_mpf_$mpfid$[($MPF.BLKCNT[mpfid]$) * COUNT_MPF_T($MPF.BLKSZ[mpfid]$)];$NL$
		$END$

$		// if mpfmb is not NULL (E_NOSPT)
		$IF !EQ(MPF.MPFMB[mpfid], "NULL")$
			$ERROR MPF.TEXT_LINE[mpfid]$E_NOSPT: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "mpfmb", MPF.MPFMB[mpfid], mpfid, "CRE_MPF")$$END$
		$END$

$		// fixed-sized memorypool management block
		static MPFMB _kernel_mpfmb_$mpfid$[$MPF.BLKCNT[mpfid]$];$NL$
	$END$

$	// generate the initialization block table of statically created fixed-sized memorypools
	const MPFINIB _kernel_mpfinib_table[TNUM_MPFID] = {$NL$
	$JOINEACH mpfid MPF.ID_LIST ",\n"$
		$TAB${ ($MPF.MPFATR[mpfid]$), ($MPF.BLKCNT[mpfid]$), ROUND_MPF_T($MPF.BLKSZ[mpfid]$), $IF EQ(MPF.MPF[mpfid],"NULL")$(_kernel_mpf_$mpfid$)$ELSE$($MPF.MPF[mpfid]$)$END$, (_kernel_mpfmb_$mpfid$) }
	$END$$NL$
	};$NL$
	$NL$

$	// generate the fixed-sized memorypool control block table for all fixed-sized memorypools
	MPFCB _kernel_mpfcb_table[TNUM_MPFID];$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const MPFINIB, _kernel_mpfinib_table);$NL$
	TOPPERS_EMPTY_LABEL(MPFCB, _kernel_mpfcb_table);$NL$
$END$$NL$

$ 
$  cyclic handler
$ 
/*$NL$
$SPC$*  Cyclic Handler Functions$NL$
$SPC$*/$NL$
$NL$

$ the max number of cyclic handler id
const ID _kernel_tmax_cycid = (TMIN_CYCID + TNUM_CYCID - 1);$NL$
$NL$

$ generated the initialization block table of statically created cyclic handlers
$IF LENGTH(CYC.ID_LIST)$
	const CYCINIB _kernel_cycinib_table[TNUM_CYCID] = {$NL$
	$JOINEACH cycid CYC.ID_LIST ",\n"$
$		// if cycatr is not [TA_STA] (E_RSATR)
		$IF (CYC.CYCATR[cycid] & ~TA_STA) != 0$
			$ERROR CYC.TEXT_LINE[cycid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "cycatr", CYC.CYCATR[cycid], cycid, "CRE_CYC")$$END$
		$END$

$		// if (0 < cyctim && cyctim <= TMAX_RELTIM) is not matched (E_PAR)
		$IF !(0 < CYC.CYCTIM[cycid] && CYC.CYCTIM[cycid] <= TMAX_RELTIM)$
			$ERROR CYC.TEXT_LINE[cycid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "cyctim", CYC.CYCTIM[cycid], cycid, "CRE_CYC")$$END$
		$END$

$		// if (0 <= cycphs && cycphs <= TMAX_RELTIM) is not matched (E_PAR)
		$IF !(0 <= CYC.CYCPHS[cycid] && CYC.CYCPHS[cycid] <= TMAX_RELTIM)$
			$ERROR CYC.TEXT_LINE[cycid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "cycphs", CYC.CYCPHS[cycid], cycid, "CRE_CYC")$$END$
		$END$

$		// warning¡§if cycatr is TA_STA and (cycphs == 0)
		$IF (CYC.CYCATR[cycid] & TA_STA) != 0 && CYC.CYCPHS[cycid] == 0$
			$WARNING CYC.TEXT_LINE[cycid]$$FORMAT(_("%1% is not recommended when %2% is set to %3% in %4%"), "cycphs==0", "TA_STA", "cycatr", "CRE_CYC")$$END$
		$END$

$		// the initialization block of cyclic handler
		$TAB${ ($CYC.CYCATR[cycid]$), (intptr_t)($CYC.EXINF[cycid]$), ($CYC.CYCHDR[cycid]$), ($CYC.CYCTIM[cycid]$), ($CYC.CYCPHS[cycid]$) }
	$END$$NL$
	};$NL$
	$NL$

$	// generate the cyclic handler control block table for all cyclic handlers
	CYCCB _kernel_cyccb_table[TNUM_CYCID];$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const CYCINIB, _kernel_cycinib_table);$NL$
	TOPPERS_EMPTY_LABEL(CYCCB, _kernel_cyccb_table);$NL$
$END$$NL$

$ 
$  alarm handler
$
/*$NL$
$SPC$*  Alarm Handler Functions$NL$
$SPC$*/$NL$
$NL$

$ the max number of alarm handler id
const ID _kernel_tmax_almid = (TMIN_ALMID + TNUM_ALMID - 1);$NL$
$NL$

$ generated the initialization block table of statically created alarm handlers
$IF LENGTH(ALM.ID_LIST)$
	const ALMINIB _kernel_alminib_table[TNUM_ALMID] = {$NL$
	$JOINEACH almid ALM.ID_LIST ",\n"$
$		// if almatr is not (TA_NULL) (E_RSATR)
		$IF ALM.ALMATR[almid] != 0$
			$ERROR ALM.TEXT_LINE[almid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "almatr", ALM.ALMATR[almid], almid, "CRE_ALM")$$END$
		$END$

$		// alarm handler initialization block
		$TAB${ ($ALM.ALMATR[almid]$), (intptr_t)($ALM.EXINF[almid]$), ($ALM.ALMHDR[almid]$) }
	$END$$NL$
	};$NL$
	$NL$

$	// generate the alarm handler control block table for all alarm handlers
	ALMCB _kernel_almcb_table[TNUM_ALMID];$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const ALMINIB, _kernel_alminib_table);$NL$
	TOPPERS_EMPTY_LABEL(ALMCB, _kernel_almcb_table);$NL$
$END$$NL$

$ 
$  interrupt management
$  
/*$NL$
$SPC$*  Interrupt Management Functions$NL$
$SPC$*/$NL$
$NL$

$  generate the lookup table of intno and inthno
$IF LENGTH(INTNO_ATTISR_VALID) != LENGTH(INHNO_ATTISR_VALID)$
	$ERROR$length of `INTNO_ATTISR_VALID' is different from length of `INHNO_ATTISR_VALID'$END$
$END$
$i = 0$
$FOREACH intno INTNO_ATTISR_VALID$
	$inhno = AT(INHNO_ATTISR_VALID, i)$
	$INHNO[intno] = inhno$
	$INTNO[inhno] = intno$
	$i = i + 1$
$END$

$ error check of the interrupt request line
$i = 0$
$FOREACH intno INT.ORDER_LIST$
$	//  if intno by CFG_INT is illegal (E_PAR)
	$IF !LENGTH(FIND(INTNO_CFGINT_VALID, INT.INTNO[intno]))$
		$ERROR INT.TEXT_LINE[intno]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "intno", INT.INTNO[intno], "CFG_INT")$$END$
	$END$

$	// if intno by CFG_INT is duplicated (E_OBJ)
	$j = 0$
	$FOREACH intno2 INT.ORDER_LIST$
		$IF INT.INTNO[intno] == INT.INTNO[intno2] && j < i$
			$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' in %3% is duplicated"), "intno", INT.INTNO[intno], "CFG_INT")$$END$
		$END$
		$j = j + 1$
	$END$

$	// if intatr is not [TA_ENAINT] | [TA_EDGE]  (E_RSATR)
	$IF (INT.INTATR[intno] & ~(TA_ENAINT|TA_EDGE|TARGET_INTATR)) != 0$
		$ERROR INT.TEXT_LINE[intno]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "intatr", INT.INTATR[intno], "intno", INT.INTNO[intno], "CFG_INT")$$END$
	$END$

$	// if intpri by CFG_INT is illegal (E_PAR)
	$IF !LENGTH(FIND(INTPRI_CFGINT_VALID, INT.INTPRI[intno]))$
		$ERROR INT.TEXT_LINE[intno]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "intpri", INT.INTPRI[intno], "CFG_INT")$$END$
	$END$

$	// for the intno allocated to the kernel,
$	// if intpri is lower than TMIN_INTPRI (E_OBJ)
	$IF LENGTH(FIND(INTNO_FIX_KERNEL, intno))$
		$IF INT.INTPRI[intno] < TMIN_INTPRI$
			$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' must not have higher priority than %3%"), "intno", INT.INTNO[intno], "TMIN_INTPRI")$$END$
		$END$
	$END$

$	// for the intno not allocated to the kernel,
$	// if intpri is lower than TMIN_INTPRI (E_OBJ)
	$IF LENGTH(FIND(INTNO_FIX_NONKERNEL, intno))$
		$IF INT.INTPRI[intno] >= TMIN_INTPRI$
			$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' must have higher priority than %3%"), "intno", INT.INTNO[intno], "TMIN_INTPRI")$$END$
		$END$
	$END$
	$i = i + 1$
$END$

$ error check of interrupt handler
$i = 0$
$FOREACH inhno INH.ORDER_LIST$
$	// if inhno by DEF_INH is illegal (E_PAR)
	$IF !LENGTH(FIND(INHNO_DEFINH_VALID, INH.INHNO[inhno]))$
		$ERROR INH.TEXT_LINE[inhno]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "inhno", INH.INHNO[inhno], "DEF_INH")$$END$
	$END$

$	// if inthno by DEF_INH is duplicated (E_OBJ)
	$j = 0$
	$FOREACH inhno2 INH.ORDER_LIST$
		$IF INH.INHNO[inhno] == INH.INHNO[inhno2] && j < i$
			$ERROR INH.TEXT_LINE[inhno]$E_OBJ: $FORMAT(_("%1% `%2%\' in %3% is duplicated"), "inhno", INH.INHNO[inhno], "DEF_INH")$$END$
		$END$
		$j = j + 1$
	$END$

$	// if inhatr is not (TA_NULL) (E_RSATR)
	$IF (INH.INHATR[inhno] & ~TARGET_INHATR) != 0$
		$ERROR INH.TEXT_LINE[inhno]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "inhatr", INH.INHATR[inhno], "inhno", INH.INHNO[inhno], "DEF_INH")$$END$
	$END$

$	// for the inhno allocated to the kernel,¡¤if inhatr is TA_NONKERNEL (E_RSATR)
	$IF LENGTH(FIND(INHNO_FIX_KERNEL, inhno))$
		$IF (INH.INHATR[inhno] & TA_NONKERNEL) != 0$
			$ERROR INH.TEXT_LINE[inhno]$E_RSATR: $FORMAT(_("%1% `%2%\' must not be non-kernel interrupt"), "inhno", INH.INHNO[inhno])$$END$
		$END$
	$END$

$	// for the inhno not allocated to the kernel¡¤if inhatr is not TA_NONKERNEL (E_RSATR)
	$IF LENGTH(FIND(INHNO_FIX_NONKERNEL, inhno))$
		$IF (INH.INHATR[inhno] & TA_NONKERNEL) == 0$
			$ERROR INH.TEXT_LINE[inhno]$E_RSATR: $FORMAT(_("%1% `%2%\' must be non-kernel interrupt"), "inhno", INH.INHNO[inhno])$$END$
		$END$
	$END$

	$IF LENGTH(INTNO[INH.INHNO[inhno]])$
		$intno = INTNO[INH.INHNO[inhno]]$
$		// if intno corresponding to inhno is not configured by CFG_INT (E_OBJ)
		$IF !LENGTH(INT.INTNO[intno])$
			$ERROR INH.TEXT_LINE[inhno]$E_OBJ: $FORMAT(_("%1% `%2%\' corresponding to %3% `%4%\' is not configured with %5%"), "intno", INT.INTNO[intno], "inhno", INH.INHNO[inhno], "CFG_INT")$$END$
		$ELSE$
			$IF (INH.INHATR[inhno] & TA_NONKERNEL) == 0$
$				// if inhatr is not TA_NONKERNEL and the intpri of corresponding intno
$				// by CFG_INT is lower than TMIN_INTPRI (E_OBJ)
				$IF INT.INTPRI[intno] < TMIN_INTPRI$
					$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' configured for %3% `%4%\' is higher than %5%"), "intpri", INT.INTPRI[intno], "inhno", INH.INHNO[inhno], "TMIN_INTPRI")$$END$
				$END$
			$ELSE$
$				// if inhatr is TA_NONKERNEL and the intpri of corresponding intno
$				// by CFG_INT is higher than TMIN_INTPRI (E_OBJ)
				$IF INT.INTPRI[intno] >= TMIN_INTPRI$
					$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' configured for %3% `%4%\' is lower than or equal to %5%"), "intpri", INT.INTPRI[intno], "inhno", INH.INHNO[inhno], "TMIN_INTPRI")$$END$
				$END$
			$END$
		$END$
	$END$
	$i = i + 1$
$END$

$ error check of interrupt service routine (ISR) and generate the
$ coressponding interrupt handler
$FOREACH order ISR.ORDER_LIST$
$	// if isratr is not (TA_NULL) (E_RSATR)
	$IF (ISR.ISRATR[order] & ~TARGET_ISRATR) != 0$
		$ERROR ISR.TEXT_LINE[order]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "isratr", ISR.ISRATR[order], "ATT_ISR")$$END$
	$END$

$	// if intno in ATT_ISR is illegal (E_PAR)
	$IF !LENGTH(FIND(INTNO_ATTISR_VALID, ISR.INTNO[order]))$
		$ERROR ISR.TEXT_LINE[order]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "intno", ISR.INTNO[order], "ATT_ISR")$$END$
	$END$

$	// if (TMIN_ISRPRI <= isrpri && isrpri <= TMAX_ISRPRI) is not matched (E_PAR)
	$IF !(TMIN_ISRPRI <= ISR.ISRPRI[order] && ISR.ISRPRI[order] <= TMAX_ISRPRI)$
		$ERROR ISR.TEXT_LINE[order]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "isrpri", ISR.ISRPRI[order], "ATT_ISR")$$END$
	$END$
$END$

$FOREACH intno INTNO_ATTISR_VALID$
	$inhno = INHNO[intno]$

$	// generate the ISR order list registered to intno
	$isr_order_list = {}$
	$FOREACH order ISR.ORDER_LIST$
		$IF ISR.INTNO[order] == intno$
			$isr_order_list = APPEND(isr_order_list, order)$
			$order_for_error = order$
		$END$
	$END$

$	// if isr_order_list is not empty
	$IF LENGTH(isr_order_list) > 0$
$		// if inhno for the intno is duplicated by DEF_INH (E_OBJ)
		$IF LENGTH(INH.INHNO[inhno])$
			$ERROR ISR.TEXT_LINE[order_for_error]$E_OBJ: $FORMAT(_("%1% `%2%\' in %3% is duplicated with %4% `%5%\'"), "intno", ISR.INTNO[order_for_error], "ATT_ISR", "inhno", INH.INHNO[inhno])$$END$
		$END$

$		// if intno by CFG_INT does not exist (E_OBJ)
		$IF !LENGTH(INT.INTNO[intno])$
			$ERROR ISR.TEXT_LINE[order_for_error]$E_OBJ: $FORMAT(_("%1% `%2%\' is not configured with %3%"), "intno", ISR.INTNO[order_for_error], "CFG_INT")$$END$
		$ELSE$
$			// if the priority of intno configured by CFG_INT is lower than TMIN_INTPRI
$			// (E_OBJ)
			$IF INT.INTPRI[intno] < TMIN_INTPRI$
				$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' configured for %3% `%4%\' is higher than %5%"), "intpri", INT.INTPRI[intno], "intno", ISR.INTNO[order_for_error], "TMIN_INTPRI")$$END$
			$END$
		$END$

$		// DEF_INH(inhno, { TA_NULL, _kernel_inthdr_<intno> } );
		$INH.INHNO[inhno] = inhno$
		$INH.INHATR[inhno] = VALUE("TA_NULL", 0)$
		$INH.INTHDR[inhno] = CONCAT("_kernel_inthdr_", intno)$
		$INH.ORDER_LIST = APPEND(INH.ORDER_LIST, inhno)$

$		// interrupt hander for ISR
		void$NL$
		_kernel_inthdr_$intno$(void)$NL$
		{$NL$
		$IF LENGTH(isr_order_list) > 1$
			$TAB$PRI	saved_ipm;$NL$
			$NL$
			$TAB$i_begin_int($intno$);$NL$
			$TAB$saved_ipm = i_get_ipm();$NL$
		$ELSE$
			$TAB$i_begin_int($intno$);$NL$
		$END$
$		// call ISR in order
		$JOINEACH order SORT(isr_order_list, "ISR.ISRPRI") "\tif (i_sense_lock()) {\n\t\ti_unlock_cpu();\n\t}\n\ti_set_ipm(saved_ipm);\n"$
			$TAB$LOG_ISR_ENTER($intno$);$NL$
			$TAB$((ISR)($ISR.ISR[order]$))((intptr_t)($ISR.EXINF[order]$));$NL$
			$TAB$LOG_ISR_LEAVE($intno$);$NL$
		$END$
		$TAB$i_end_int($intno$);$NL$
		}$NL$
	$END$
$END$
$NL$

$ 
$  generate the standard initialization information for interrupt
$  management
$ 
$ the necessary information for interrupt handlers initialization
$IF !OMIT_INITIALIZE_INTERRUPT || ALT(USE_INHINIB_TABLE,0)$

$ the number of interrupt handlers
#define TNUM_INHNO	$LENGTH(INH.ORDER_LIST)$$NL$
const uint_t _kernel_tnum_inhno = TNUM_INHNO;$NL$
$NL$
$FOREACH inhno INH.ORDER_LIST$
	$IF (INH.INHATR[inhno] & TA_NONKERNEL) == 0$
		INTHDR_ENTRY($INH.INHNO[inhno]$, $+INH.INHNO[inhno]$, $INH.INTHDR[inhno]$)$NL$
	$END$
$END$
$NL$

$  generated the initialization block table of interrupt handlers
$IF LENGTH(INH.ORDER_LIST)$
	const INHINIB _kernel_inhinib_table[TNUM_INHNO] = {$NL$
	$JOINEACH inhno INH.ORDER_LIST ",\n"$
		$IF (INH.INHATR[inhno] & TA_NONKERNEL) == 0$
			$TAB${ ($INH.INHNO[inhno]$), ($INH.INHATR[inhno]$), (FP)(INT_ENTRY($INH.INHNO[inhno]$, $INH.INTHDR[inhno]$)) }
		$ELSE$
			$TAB${ ($INH.INHNO[inhno]$), ($INH.INHATR[inhno]$), (FP)($INH.INTHDR[inhno]$) }
		$END$
	$END$$NL$
	};$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const INHINIB, _kernel_inhinib_table);$NL$
$END$$NL$
$END$

$ the necessary information for interrupt request lines initialization
$IF !OMIT_INITIALIZE_INTERRUPT || ALT(USE_INHINTB_TABLE,0)$

$ the number of interrupt request lines
#define TNUM_INTNO	$LENGTH(INT.ORDER_LIST)$$NL$
const uint_t _kernel_tnum_intno = TNUM_INTNO;$NL$
$NL$

$ generated the initialization block table of interrupt request lines
$IF LENGTH(INT.ORDER_LIST)$
	const INTINIB _kernel_intinib_table[TNUM_INTNO] = {$NL$
	$JOINEACH intno INT.ORDER_LIST ",\n"$
		$TAB${ ($INT.INTNO[intno]$), ($INT.INTATR[intno]$), ($INT.INTPRI[intno]$) }
	$END$$NL$
	};$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const INTINIB, _kernel_intinib_table);$NL$
$END$$NL$
$END$

$ 
$  CPU exception management functions
$ 
/*$NL$
$SPC$*  CPU Exception Management Functions$NL$
$SPC$*/$NL$
$NL$

$ error check of CPU exception handler
$i = 0$
$FOREACH excno EXC.ORDER_LIST$
$	// if excno by DEF_EXC is illegal (E_PAR)
	$IF !LENGTH(FIND(EXCNO_DEFEXC_VALID, EXC.EXCNO[excno]))$
		$ERROR EXC.TEXT_LINE[excno]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "excno", EXC.EXCNO[excno], "DEF_EXC")$$END$
	$END$

$	// if excno by DEF_EXC is duplicated (E_OBJ)
	$j = 0$
	$FOREACH excno2 EXC.ORDER_LIST$
		$IF EXC.EXCNO[excno] == EXC.EXCNO[excno2] && j < i$
			$ERROR EXC.TEXT_LINE[excno]$E_OBJ: $FORMAT(_("%1% `%2%\' in %3% is duplicated"), "excno", EXC.EXCNO[excno], "DEF_EXC")$$END$
		$END$
		$j = j + 1$
	$END$

$	// if excatr is not (TA_NULL) (E_RSATR)
	$IF (EXC.EXCATR[excno] & ~TARGET_EXCATR) != 0$
		$ERROR EXC.TEXT_LINE[excno]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "excatr", EXC.EXCATR[excno], "excno", EXC.EXCNO[excno], "DEF_EXC")$$END$
	$END$
	$i = i + 1$
$END$

$  generate the standard initialization information for CPU exception
$  management
$IF !OMIT_INITIALIZE_EXCEPTION$

$ the number of CPU exception handlers
#define TNUM_EXCNO	$LENGTH(EXC.ORDER_LIST)$$NL$
const uint_t _kernel_tnum_excno = TNUM_EXCNO;$NL$
$NL$
$FOREACH excno EXC.ORDER_LIST$
	EXCHDR_ENTRY($EXC.EXCNO[excno]$, $+EXC.EXCNO[excno]$, $EXC.EXCHDR[excno]$)$NL$
$END$
$NL$

$ generated the initialization block table of CPU exception handler
$IF LENGTH(EXC.ORDER_LIST)$
	const EXCINIB _kernel_excinib_table[TNUM_EXCNO] = {$NL$
	$JOINEACH excno EXC.ORDER_LIST ",\n"$
		$TAB${ ($EXC.EXCNO[excno]$), ($EXC.EXCATR[excno]$), (FP)(EXC_ENTRY($EXC.EXCNO[excno]$, $EXC.EXCHDR[excno]$)) }
	$END$$NL$
	};$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const EXCINIB, _kernel_excinib_table);$NL$
$END$$NL$
$END$

$ 
$  stack area for non-task context
$ 
/*$NL$
$SPC$*  Stack Area for Non-task Context$NL$
$SPC$*/$NL$
$NL$

$IF !LENGTH(ICS.ORDER_LIST)$
$	// if DEF_ICS does not exsit, use the default value
	#ifdef DEFAULT_ISTK$NL$
	$NL$
	#define TOPPERS_ISTKSZ		DEFAULT_ISTKSZ$NL$
	#define TOPPERS_ISTK		DEFAULT_ISTK$NL$
	$NL$
	#else /* DEAULT_ISTK */$NL$
	$NL$
	static STK_T				_kernel_istack[COUNT_STK_T(DEFAULT_ISTKSZ)];$NL$
	#define TOPPERS_ISTKSZ		ROUND_STK_T(DEFAULT_ISTKSZ)$NL$
	#define TOPPERS_ISTK		_kernel_istack$NL$
	$NL$
	#endif /* DEAULT_ISTK */$NL$
$ELSE$

$	// if there are more than 1 DEF_ICS (E_OBJ)
	$IF LENGTH(ICS.ORDER_LIST) > 1$
		$ERROR$E_OBJ: $FORMAT(_("too many %1%"), "DEF_ICS")$$END$
	$END$

$ 	// if istksz is not aligned (E_PAR)
	$IF !EQ(ICS.ISTK[1], "NULL") && CHECK_STKSZ_ALIGN
							&& (ICS.ISTKSZ[1] & (CHECK_STKSZ_ALIGN - 1))$
		$ERROR ICS.TEXT_LINE[1]$E_PAR: $FORMAT(_("%1% `%2%\' in %3% is not aligned"), "istksz", ICS.ISTKSZ[1], "DEF_ICS")$$END$
	$END$

	$IF EQ(ICS.ISTK[1], "NULL")$
$		//  allocate the stack by configurator
		static STK_T				_kernel_istack[COUNT_STK_T($ICS.ISTKSZ[1]$)];$NL$
		#define TOPPERS_ISTKSZ		ROUND_STK_T($ICS.ISTKSZ[1]$)$NL$
		#define TOPPERS_ISTK		_kernel_istack$NL$
	$ELSE$
		#define TOPPERS_ISTKSZ		($ICS.ISTKSZ[1]$)$NL$
		#define TOPPERS_ISTK		($ICS.ISTK[1]$)$NL$
	$END$
$END$
$NL$

$ stack area for non-task context
const SIZE		_kernel_istksz = TOPPERS_ISTKSZ;$NL$
STK_T *const	_kernel_istk = TOPPERS_ISTK;$NL$
$NL$
#ifdef TOPPERS_ISTKPT$NL$
STK_T *const	_kernel_istkpt = TOPPERS_ISTKPT(TOPPERS_ISTK, TOPPERS_ISTKSZ);$NL$
#endif /* TOPPERS_ISTKPT */$NL$
$NL$

$ 
$  time event management
$ 
/*$NL$
$SPC$*  Time Event Management$NL$
$SPC$*/$NL$
$NL$
TMEVTN   _kernel_tmevt_heap[TNUM_TSKID + TNUM_CYCID + TNUM_ALMID];$NL$
$NL$

$ 
$  module initialization function
$ 
/*$NL$
$SPC$*  Module Initialization Function$NL$
$SPC$*/$NL$
$NL$
void$NL$
_kernel_initialize_object(void)$NL$
{$NL$
$TAB$_kernel_initialize_task();$NL$
$IF LENGTH(SEM.ID_LIST)$	_kernel_initialize_semaphore();$NL$$END$
$IF LENGTH(FLG.ID_LIST)$	_kernel_initialize_eventflag();$NL$$END$
$IF LENGTH(DTQ.ID_LIST)$	_kernel_initialize_dataqueue();$NL$$END$
$IF LENGTH(PDQ.ID_LIST)$	_kernel_initialize_pridataq();$NL$$END$
$IF LENGTH(MBX.ID_LIST)$	_kernel_initialize_mailbox();$NL$$END$
$IF LENGTH(MPF.ID_LIST)$	_kernel_initialize_mempfix();$NL$$END$
$IF LENGTH(CYC.ID_LIST)$	_kernel_initialize_cyclic();$NL$$END$
$IF LENGTH(ALM.ID_LIST)$	_kernel_initialize_alarm();$NL$$END$
$TAB$_kernel_initialize_interrupt();$NL$
$TAB$_kernel_initialize_exception();$NL$
}$NL$
$NL$

$ 
$  initialization routine
$
/*$NL$
$SPC$*  Initialization Routine$NL$
$SPC$*/$NL$
$NL$
void$NL$
_kernel_call_inirtn(void)$NL$
{$NL$
$FOREACH order INI.ORDER_LIST$
$ 	// if iniatr is not (TA_NULL) (E_RSATR)
	$IF INI.INIATR[order] != 0$
		$ERROR INI.TEXT_LINE[order]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "iniatr", INI.INIATR[order], "inirtn", INI.INIRTN[order], "ATT_INI")$$END$
	$END$
	$TAB$((INIRTN)($INI.INIRTN[order]$))((intptr_t)($INI.EXINF[order]$));$NL$
$END$
}$NL$
$NL$

$ 
$  termination routine
$ 
/*$NL$
$SPC$*  Termination Routine$NL$
$SPC$*/$NL$
$NL$
void$NL$
_kernel_call_terrtn(void)$NL$
{$NL$
$FOREACH rorder TER.RORDER_LIST$
$ 	// if teratr is not (TA_NULL) (E_RSATR)
	$IF TER.TERATR[rorder] != 0$
		$ERROR TER.TEXT_LINE[rorder]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "teratr", TER.TERATR[rorder], "terrtn", TER.TERRTN[rorder], "ATT_TER")$$END$
	$END$
	$TAB$((TERRTN)($TER.TERRTN[rorder]$))((intptr_t)($TER.EXINF[rorder]$));$NL$
$END$
}$NL$
$NL$
