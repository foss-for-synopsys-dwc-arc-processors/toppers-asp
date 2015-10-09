/* This file is generated from core_rename.def by genrename. */

#ifndef TOPPERS_CORE_RENAME_H
#define TOPPERS_CORE_RENAME_H

/*
 *  core_support.S
 */
#define start_r						_kernel_start_r
#define ret_int						_kernel_ret_int
#define ret_exc						_kernel_ret_exc
#define exc_tbl						_kernel_exc_tbl
#define dispatch					_kernel_dispatch
#define start_dispatch				_kernel_start_dispatch
#define exit_and_dispatch			_kernel_exit_and_dispatch
#define call_exit_kernel			_kernel_call_exit_kernel
#define core_int_entry				_kernel_core_int_entry
#define core_exc_entry				_kernel_core_exc_entry

/*
 *  core_config.c
 */
#define excpt_nest_count			_kernel_excpt_nest_count
#define core_initialize				_kernel_core_initialize
#define core_terminate				_kernel_core_terminate
#define x_install_exc				_kernel_x_install_exc
#define default_int_handler			_kernel_default_int_handler
#define default_exc_handler			_kernel_default_exc_handler

#ifdef TOPPERS_LABEL_ASM

/*
 *  core_support.S
 */
#define _start_r					__kernel_start_r
#define _ret_int					__kernel_ret_int
#define _ret_exc					__kernel_ret_exc
#define _exc_tbl					__kernel_exc_tbl
#define _dispatch					__kernel_dispatch
#define _start_dispatch				__kernel_start_dispatch
#define _exit_and_dispatch			__kernel_exit_and_dispatch
#define _call_exit_kernel			__kernel_call_exit_kernel
#define _core_int_entry				__kernel_core_int_entry
#define _core_exc_entry				__kernel_core_exc_entry

/*
 *  core_config.c
 */
#define _excpt_nest_count			__kernel_excpt_nest_count
#define _core_initialize			__kernel_core_initialize
#define _core_terminate				__kernel_core_terminate
#define _x_install_exc				__kernel_x_install_exc
#define _default_int_handler		__kernel_default_int_handler
#define _default_exc_handler		__kernel_default_exc_handler

#endif /* TOPPERS_LABEL_ASM */


#endif /* TOPPERS_CORE_RENAME_H */
