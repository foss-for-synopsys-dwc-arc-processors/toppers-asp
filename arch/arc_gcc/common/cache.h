/* ------------------------------------------
 * Copyright (c) 2015, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */
#ifndef CACHE_H
#define CACHE_H
#include "arc.h"
#include "core_config.h"

#define IC_CTRL_IC_ENABLE	0x0
#define IC_CTRL_IC_DISABLE	0x1
#define IC_CTRL_DIRECT_ACCESS	0x0
#define IC_CTRL_INDIRECT_ACCESS	0x20
#define IC_CTRL_OP_SUCCEEDED	0x4


#define DC_CTRL_DC_ENABLE	0x0
#define DC_CTRL_DC_DISABLE	0x1
#define DC_CTRL_INVALID_ONLY	0x0
#define DC_CTRL_INVALID_FLUSH	0x40
#define DC_CTRL_ENABLE_FLUSH_LOCKED	0x80
#define DC_CTRL_DISABLE_FLUSH_LOCKED	0x0
#define DC_CTRL_FLUSH_STATUS	0x100
#define DC_CTRL_DIRECT_ACCESS	0x0
#define DC_CTRL_INDIRECT_ACCESS	0x20
#define DC_CTRL_OP_SUCCEEDED	0x4

#ifdef ARC_HAS_I_CACHE
Inline void icache_enable(icache_en_mask)
{
	write_aux_reg(ARC_REG_IC_CTRL, icache_en_mask);
}

Inline void icache_disable(void)
{
	write_aux_reg(ARC_REG_IC_CTRL, IC_CTRL_IC_DISABLE);
}

Inline void icache_invalidate(void)
{
	/* invalidate the entire icache */
	write_aux_reg(ARC_REG_IC_IVIC, 0);
	Asm("nop_s");	
}

Inline void icache_invalidate_line(uint32_t address)
{
	write_aux_reg(ARC_REG_IC_IVIL, address);
	/* the 3 nops are required by ARCv2 ISA */
	Asm("nop_s");
	Asm("nop_s");
	Asm("nop_s");
}

Inline void icache_lock_line(uint32_t address)
{
	write_aux_reg(ARC_REG_IC_LIL, address);
}
#endif

#ifdef ARC_HAS_I_CACHE
Inline void dcache_invalidate(void)
{
	write_aux_reg(ARC_REG_DC_IVDC, 1);
}

Inline void dcache_invalidate_line(uint32_t address)
{
	write_aux_reg(ARC_REG_DC_IVDL, address);
}

Inline void dcache_enable(dcache_en_mask)
{
	write_aux_reg(ARC_REG_DC_CTRL, dcache_en_mask);
}

Inline void dcache_disable(void)
{
	write_aux_reg(ARC_REG_DC_CTRL, DC_CTRL_DC_DISABLE);
}

Inline void dcache_flush(void)
{
	x_lock_cpu();
	write_aux_reg(ARC_REG_DC_FLSH, 1);
	/* wait for flush completion */
	while (read_aux_reg(ARC_REG_DC_CTRL) & DC_CTRL_FLUSH_STATUS);

	x_unlock_cpu();
}


Inline void dcache_flush_line(uint32_t address)
{
	x_lock_cpu();
	write_aux_reg(ARC_REG_DC_FLDL, address);
	while (read_aux_reg(ARC_REG_DC_CTRL) & DC_CTRL_FLUSH_STATUS);
	x_unlock_cpu();
}

Inline void dcache_lock_line(uint32_t address)
{
	write_aux_reg(ARC_REG_DC_LDL, address);
}
#endif

extern void icache_invalidate_mlines(uint32_t start_addr, uint32_t size);
extern void icache_lock_mlines(uint32_t start_addr, uint32_t size);
extern void dcache_invalidate_mlines(uint32_t start_addr, uint32_t size);
extern void dcache_flush_mlines(uint32_t start_addr, uint32_t size);
extern void dcache_lock_mlines(uint32_t start_addr, uint32_t size);
extern void arc_cache_init(void);

#endif
	
