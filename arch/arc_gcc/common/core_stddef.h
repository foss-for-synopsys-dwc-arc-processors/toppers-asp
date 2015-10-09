/*
 *  the core-dependent part of t_stddef.h (for ARC)
 *
 *  this head file should not be included directly in the files except
 *  target_stddef.h (or the files included in target_config.h). 
 */

#ifndef TOPPERS_CORE_STDDEF_H
#define TOPPERS_CORE_STDDEF_H

/*
 * macro for target identification
 */
#define TOPPERS_ARC         /* the processor abbreviation */

/*
 *  stack type
 *  for ARC, stack should be 8-bytes aligned
 */
#define TOPPERS_STK_T  long long

#endif /* TOPPERS_CORE_STDDEF_H */
