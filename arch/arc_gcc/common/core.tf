$ 
$	core-dependent template in pass2 (for ARC)
$ 


$
$ vaild interrupt no and interrupt handler no
$
$INTNO_VALID = RANGE(TMIN_INTNO, TMAX_INTNO)$
$INHNO_VALID = INTNO_VALID$

$ 
$ valid CPU exception numbers
$ 
$EXCNO_VALID = { 1,2,...,15 }$

$
$ valid intno and inhno for ATTISR
$
$INTNO_ATTISR_VALID = INTNO_VALID$
$INHNO_ATTISR_VALID = INHNO_VALID$


$
$ valid intno and inhno for DEFINH DEFEXC
$
$INHNO_DEFINH_VALID = INHNO_VALID$
$EXCNO_DEFEXC_VALID = EXCNO_VALID$

$
$ valid intno and intpri for CFGINT
$
$INTNO_CFGINT_VALID = INTNO_VALID$
$INTPRI_CFGINT_VALID = RANGE(TMIN_INTPRI, TMAX_INTPRI)$


$ 
$  include kernel standard template file
$ 
$INCLUDE "kernel/kernel.tf"$

/*$NL$
$SPC$*  Target-dependent Definitions (ARC)$NL$
$SPC$*/$NL$
$NL$

$ 
$  generate the vector table
$ 
$FILE "kernel_cfg.c"$


__attribute__ ((section(".vector"))) $NL$
const FP _kernel_vector_table[] =      $NL$ 
{                                    $NL$
	$TAB$(FP)_start,                 // 0 The reset handler         $NL$

$FOREACH excno EXCNO_VALID$ 
	$TAB$(FP)(_kernel_core_exc_entry),
	$SPC$$FORMAT("/* %d */", +excno)$$NL$
$END$

$FOREACH inhno INTNO_VALID$ 
	$IF LENGTH(INH.INHNO[inhno]) && ((INH.INHATR[inhno] & TA_NONKERNEL) != 0)$
		$TAB$(FP)($INH.INTHDR[inhno]$),
	$ELSE$
		$TAB$(FP)(_kernel_core_int_entry),
	$END$
	$SPC$$FORMAT("/* %d */", +inhno)$$NL$
$END$

$NL$};$NL$
$NL$


$
$ generate the exception table
$
$NL$
const FP _kernel_exc_tbl[] = {$NL$
	$TAB$(FP)_start,                 // 0 The reset handler         $NL$
$FOREACH excno EXCNO_VALID$ 
	$IF LENGTH(EXC.EXCNO[excno])$
		$TAB$(FP)($EXC.EXCHDR[excno]$),
	$ELSE$
		$TAB$(FP)(_kernel_default_exc_handler),
	$END$
	$SPC$$FORMAT("/* %d */", +excno)$$NL$
$END$

$FOREACH inhno INHNO_VALID$
	$IF LENGTH(INH.INHNO[inhno])$
		$TAB$(FP)($INH.INTHDR[inhno]$),
	$ELSE$
		$TAB$(FP)(_kernel_default_int_handler),
	$END$
	$SPC$$FORMAT("/* %d */", +inhno)$$NL$
$END$

$NL$};$NL$
$NL$

