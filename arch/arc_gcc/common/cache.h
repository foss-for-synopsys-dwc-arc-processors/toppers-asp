/* Copyright (C) 
 * 2014 - Wayne Ren
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */
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
	
