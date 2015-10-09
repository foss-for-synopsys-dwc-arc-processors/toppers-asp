/*
 *  the core-dependent part of sil.h (for arc)
 */

#ifndef TOPPERS_CORE_SIL_H
#define TOPPERS_CORE_SIL_H

#ifndef TOPPERS_MACRO_ONLY

Inline uint32_t
TOPPERS_disint(void)
{
	uint32_t status32;
	Asm("clri %0":"=r"(status32));
	return status32;
}

Inline void
TOPPERS_enaint(uint32_t status32)
{
	Asm("seti %0": : "r"(status32):"memory", "cc");
}



/*
 *  lock and unlock all the interrupts
 */
#define SIL_PRE_LOC   uint32_t TOPPERS_status32
#define SIL_LOC_INT() ((void)(TOPPERS_status32 = TOPPERS_disint()))
#define SIL_UNL_INT() (TOPPERS_enaint(TOPPERS_status32))

/*
 *  delay in nano seconds
 */
Inline void
sil_dly_nse(ulong_t dlytim)
{
	register uint32_t r0 asm("r0") = (uint32_t) dlytim;    
	Asm("bl _sil_dly_nse" : "=g"(r0) : "0"(r0) : "blink","cc");
}

#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_CORE_SIL_H */
