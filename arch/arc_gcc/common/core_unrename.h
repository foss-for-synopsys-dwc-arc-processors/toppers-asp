/* This file is generated from core_rename.def by genrename. */

/* This file is included only when core_rename.h has been included. */
#ifdef TOPPERS_CORE_RENAME_H
#undef TOPPERS_CORE_RENAME_H

/*
 *  core_support.S
 */
#undef start_r
#undef ret_int
#undef ret_exc
#undef exc_tbl
#undef dispatch
#undef start_dispatch
#undef exit_and_dispatch
#undef call_exit_kernel
#undef core_int_entry
#undef core_exc_entry

/*
 *  core_config.c
 */
#undef excpt_nest_count
#undef core_initialize
#undef core_terminate
#undef x_install_exc
#undef default_int_handler
#undef default_exc_handler

#ifdef TOPPERS_LABEL_ASM

/*
 *  core_support.S
 */
#undef _start_r
#undef _ret_int
#undef _ret_exc
#undef _exc_tbl
#undef _dispatch
#undef _start_dispatch
#undef _exit_and_dispatch
#undef _call_exit_kernel
#undef _core_int_entry
#undef _core_exc_entry

/*
 *  core_config.c
 */
#undef _excpt_nest_count
#undef _core_initialize
#undef _core_terminate
#undef _x_install_exc
#undef _default_int_handler
#undef _default_exc_handler

#endif /* TOPPERS_LABEL_ASM */


#endif /* TOPPERS_CORE_RENAME_H */
