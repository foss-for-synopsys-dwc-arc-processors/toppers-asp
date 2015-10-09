/*
 *  the assemble part of chip dependent module (for EMSTARTKIT)
 */

#define  TOPPERS_MACRO_ONLY
#define UINT_C(val)	(val)		/* macro to make uint_t constant */
#define ULONG_C(val)	(val)		/* macro to make ulong_t constant */
#include "kernel_impl.h"

/*
 *  low level target hardware initialization
 *
 *  this routine is called before memory initialization at the 
 *  startup module.
 	clock, mem controller etc. should be initialized here
 */
        .text
        .align 4
        .global hardware_init_hook
hardware_init_hook:
        j  [blink]
