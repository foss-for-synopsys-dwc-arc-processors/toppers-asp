$ ======================================================================
$ 
$   TOPPERS/ASP Kernel
$       Toyohashi Open Platform for Embedded Real-Time Systems/
$       Advanced Standard Profile Kernel
$ 
$   Copyright (C) 2008-2011 by Embedded and Real-Time Systems Laboratory
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
$   @(#) $Id: kernel_check.tf 2062 2011-04-10 16:27:41Z ertl-hiro $
$  
$ =====================================================================

$ 
$  check the alignment of function address
$ 
$IF CHECK_FUNC_ALIGN || CHECK_FUNC_NONNULL$
$	// check the address of task entry and task exception routine
	$tinib = SYMBOL("_kernel_tinib_table")$
	$FOREACH tskid TSK.ID_LIST$
		$task = PEEK(tinib + offsetof_TINIB_task, sizeof_FP)$
		$IF CHECK_FUNC_ALIGN && (task & (CHECK_FUNC_ALIGN - 1)) != 0$
			$ERROR TSK.TEXT_LINE[tskid]$E_PAR: 
				$FORMAT(_("%1% `%2%\' of `%3%\' in %4% is not aligned"),
				"task", TSK.TASK[tskid], tskid, "CRE_TSK")$$END$
		$END$
		$IF CHECK_FUNC_NONNULL && task == 0$
			$ERROR TSK.TEXT_LINE[tskid]$E_PAR: 
				$FORMAT(_("%1% `%2%\' of `%3%\' in %4% is null"),
				"task", TSK.TASK[tskid], tskid, "CRE_TSK")$$END$
		$END$
		$texrtn = PEEK(tinib + offsetof_TINIB_texrtn, sizeof_FP)$
		$IF CHECK_FUNC_ALIGN && (texrtn & (CHECK_FUNC_ALIGN - 1)) != 0$
			$ERROR DEF_TEX.TEXT_LINE[tskid]$E_PAR: 
				$FORMAT(_("%1% `%2%\' of `%3%\' in %4% is not aligned"),
				"texrtn", TSK.TEXRTN[tskid], tskid, "DEF_TEX")$$END$
		$END$
		$tinib = tinib + sizeof_TINIB$
	$END$

$	// check the address of cyclic handlers
	$cycinib = SYMBOL("_kernel_cycinib_table")$
	$FOREACH cycid CYC.ID_LIST$
		$cychdr = PEEK(cycinib + offsetof_CYCINIB_cychdr, 4)$
		$IF CHECK_FUNC_ALIGN && (cychdr & (CHECK_FUNC_ALIGN - 1)) != 0$
			$ERROR CYC.TEXT_LINE[cycid]$E_PAR: 
				$FORMAT(_("%1% `%2%\' of `%3%\' in %4% is not aligned"),
				"cychdr", CYC.CYCHDR[cycid], cycid, "CRE_CYC")$$END$
		$END$
		$IF CHECK_FUNC_NONNULL && cychdr == 0$
			$ERROR CYC.TEXT_LINE[cycid]$E_PAR: 
				$FORMAT(_("%1% `%2%\' of `%3%\' in %4% is null"),
				"cychdr", CYC.CYCHDR[cycid], cycid, "CRE_CYC")$$END$
		$END$
		$cycinib = cycinib + sizeof_CYCINIB$
	$END$

$	// check the address of alarm handlers
	$alminib = SYMBOL("_kernel_alminib_table")$
	$FOREACH almid ALM.ID_LIST$
		$almhdr = PEEK(alminib + offsetof_ALMINIB_almhdr, 4)$
		$IF CHECK_FUNC_ALIGN && (almhdr & (CHECK_FUNC_ALIGN - 1)) != 0$
			$ERROR ALM.TEXT_LINE[almid]$E_PAR: 
				$FORMAT(_("%1% `%2%\' of `%3%\' in %4% is not aligned"),
				"almhdr", ALM.ALMHDR[almid], almid, "CRE_ALM")$$END$
		$END$
		$IF CHECK_FUNC_NONNULL && almhdr == 0$
			$ERROR ALM.TEXT_LINE[almid]$E_PAR: 
				$FORMAT(_("%1% `%2%\' of `%3%\' in %4% is null"),
				"almhdr", ALM.ALMHDR[almid], almid, "CRE_ALM")$$END$
		$END$
		$alminib = alminib + sizeof_ALMINIB$
	$END$
$END$

$ 
$  check the address of stack area
$ 
$IF CHECK_STACK_ALIGN || CHECK_STACK_NONNULL$
$	// check task stack address
	$tinib = SYMBOL("_kernel_tinib_table")$
	$FOREACH tskid TSK.ID_LIST$
		$IF USE_TSKINICTXB$
			$stk = GET_STK_TSKINICTXB(tinib)$
		$ELSE$
			$stk = PEEK(tinib + offsetof_TINIB_stk, sizeof_void_ptr)$
		$END$
		$IF CHECK_STACK_ALIGN && (stk & (CHECK_STACK_ALIGN - 1)) != 0$
			$ERROR TSK.TEXT_LINE[tskid]$E_PAR: 
				$FORMAT(_("%1% `%2%\' of `%3%\' in %4% is not aligned"),
				"stk", TSK.STK[tskid], tskid, "CRE_TSK")$$END$
		$END$
		$IF CHECK_STACK_NONNULL && stk == 0$
			$ERROR TSK.TEXT_LINE[tskid]$E_PAR: 
				$FORMAT(_("%1% `%2%\' of `%3%\' in %4% is null"),
				"stk", TSK.STK[tskid], tskid, "CRE_TSK")$$END$
		$END$
		$tinib = tinib + sizeof_TINIB$
	$END$

$	// check non-task context stack address
	$istk = PEEK(SYMBOL("_kernel_istk"), sizeof_void_ptr)$
	$IF CHECK_STACK_ALIGN && (istk & (CHECK_STACK_ALIGN - 1)) != 0$
		$ERROR ICE.TEXT_LINE[1]$E_PAR: 
			$FORMAT(_("%1% `%2%\' in %3% is not aligned"),
			"istk", ICS.ISTK[1], "DEF_ICS")$$END$
	$END$
	$IF CHECK_STACK_NONNULL && istk == 0$
		$ERROR ICE.TEXT_LINE[1]$E_PAR: 
			$FORMAT(_("%1% `%2%\' in %3% is null"),
			"istk", ICS.ISTK[1], "DEF_ICS")$$END$
	$END$
$END$

$ 
$  check the address of fixed-sized memory pool
$ 
$IF CHECK_MPF_ALIGN || CHECK_MPF_NONNULL$
$	// check the address of fixed-sized memory pool
	$mpfinib = SYMBOL("_kernel_mpfinib_table")$
	$FOREACH mpfid MPF.ID_LIST$
		$mpf = PEEK(mpfinib + offsetof_MPFINIB_mpf, sizeof_void_ptr)$
		$IF CHECK_MPF_ALIGN && (mpf & (CHECK_MPF_ALIGN - 1)) != 0$
			$ERROR MPF.TEXT_LINE[mpfid]$E_PAR: 
				$FORMAT(_("%1% `%2%\' of `%3%\' in %4% is not aligned"),
				"mpf", MPF.MPF[mpfid], mpfid, "CRE_MPF")$$END$
		$END$
		$IF CHECK_MPF_NONNULL && mpf == 0$
			$ERROR MPF.TEXT_LINE[mpfid]$E_PAR: 
				$FORMAT(_("%1% `%2%\' of `%3%\' in %4% is null"),
				"mpf", MPF.MPF[mpfid], mpfid, "CRE_MPF")$$END$
		$END$
		$mpfinib = mpfinib + sizeof_MPFINIB$
	$END$
$END$
