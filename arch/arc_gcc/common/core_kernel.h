/*
 *      the core-dependent part of kernel.h (for ARC)
 *
 *  this head file should not be included directly in the files except
 *  target_kernel.h (or the files included in target_kernel.h). 
 */

#ifndef TOPPERS_CORE_KERNEL_H
#define TOPPERS_CORE_KERNEL_H

#include "arc.h"

#ifndef TOPPERS_MACRO_ONLY

/*
 * get the return address of CPU exceptions
 */
Inline uint32_t
x_get_exc_raddr(void *p_excinf)
{
    return(*(((uint32_t *)(p_excinf)) + P_EXCINF_OFFSET_ERET));
}

/*
 * set the return address of CPU exceptions
 */
Inline void
x_set_exc_raddr(void *p_excinf, uint32_t pc)
{
    *(((uint32_t *)(p_excinf)) + P_EXCINF_OFFSET_ERET) = pc;
}
#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_CORE_KERNEL_H */
