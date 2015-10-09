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

#include "kernel_impl.h"
#include "cache.h"

#ifdef ARC_HAS_I_CACHE
void icache_invalidate_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t final_addr;
	uint32_t cache_addr;
	uint32_t line_size;

	if (size == 0) {
		return;
	}

	line_size = 8 << ((read_aux_reg(ARC_REG_I_CACHE_BUILD) & 0x000f0000) >> 16);
	cache_addr = start_addr;
	final_addr = cache_addr + size;

	while (cache_addr < final_addr) {
		write_aux_reg(ARC_REG_IC_IVIL, cache_addr);
		Asm("nop_s");
		Asm("nop_s");
		Asm("nop_s");
		cache_addr += line_size;
	}

}

void icache_lock_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t final_addr;
	uint32_t cache_addr;
	uint32_t line_size;

	if (size == 0) {
		return;
	}

	line_size = 8 << ((read_aux_reg(ARC_REG_I_CACHE_BUILD) & 0x000f0000) >> 16);
	cache_addr = start_addr;
	final_addr = cache_addr + line_size;

	while (cache_addr < final_addr) {
		write_aux_reg(ARC_REG_IC_LIL, cache_addr);
		Asm("nop_s");
		Asm("nop_s");
		Asm("nop_s");
		cache_addr += line_size;
	}

}

#endif


#ifdef ARC_HAS_D_CACHE
void dcache_invalidate_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t final_addr;
	uint32_t cache_addr;
	uint32_t line_size;

	if (size == 0) {
		return;
	}

	line_size = 16 << ((read_aux_reg(ARC_REG_D_CACHE_BUILD) & 0x000f0000) >> 16);
	cache_addr = start_addr;
	final_addr = cache_addr + line_size;

	while (cache_addr < final_addr) {
		write_aux_reg(ARC_REG_DC_IVDL, cache_addr);
		cache_addr += line_size;
	}

}

void dcache_flush_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t final_addr;
	uint32_t cache_addr;
	uint32_t line_size;

	if (size == 0) {
		return;
	}

	x_lock_cpu();
	line_size = 16 << ((read_aux_reg(ARC_REG_D_CACHE_BUILD) & 0x000f0000) >> 16);
	cache_addr = start_addr;
	final_addr = cache_addr + size;

	while (cache_addr < final_addr) {
		write_aux_reg(ARC_REG_DC_FLDL, cache_addr);
		cache_addr += line_size;
	}

	/* wait for flush completion */
	while (read_aux_reg(ARC_REG_DC_CTRL) & DC_CTRL_FLUSH_STATUS);
	x_unlock_cpu();
}

void dcache_lock_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t final_addr;
	uint32_t cache_addr;
	uint32_t line_size;

	if (size == 0) {
		return;
	}

	line_size = 16 << ((read_aux_reg(ARC_REG_D_CACHE_BUILD) & 0x000f0000) >> 16);
	cache_addr = start_addr;
	final_addr = cache_addr + size;

	while (cache_addr < final_addr) {
		write_aux_reg(ARC_REG_DC_LDL, cache_addr);
		cache_addr += line_size;
	}
}
#endif

#if defined(ARC_HAS_D_CACHE) || defined(ARC_HAS_I_CACHE)
void arc_cache_init(void)
{
	uint32_t cache_ver;
	uint32_t cache_size;
	uint32_t cache_llen;
	uint32_t cache_ways;
	uint32_t build_cfg;


#ifdef ARC_HAS_I_CACHE
	icache_enable(IC_CTRL_INDIRECT_ACCESS);
	icache_invalidate();
#endif
#ifdef ARC_HAS_D_CACHE
	dcache_enable(DC_CTRL_DISABLE_FLUSH_LOCKED | 
			DC_CTRL_INDIRECT_ACCESS | DC_CTRL_INVALID_FLUSH);
	dcache_invalidate();
#endif
	build_cfg = read_aux_reg(ARC_REG_D_CACHE_BUILD);

	cache_ver = build_cfg & 0xff;
	cache_ways = 1 << ((build_cfg >> 8) & 0xf);
	cache_size = 512 << ((build_cfg >> 12) & 0xf);
	cache_llen = 16 << ((build_cfg >> 16) & 0xf);
	
	syslog_4(LOG_NOTICE, "Data cache, ver:%d, capacity:%d, ways:%d, line len:%d",
			cache_ver, cache_size, cache_ways, cache_llen);

	build_cfg = read_aux_reg(ARC_REG_I_CACHE_BUILD);

	cache_ver = build_cfg & 0xff;
	cache_ways = 1 << ((build_cfg >> 8) & 0xf);
	cache_size = 512 << ((build_cfg >> 12) & 0xf);
	cache_llen = 8 << ((build_cfg >> 16) & 0xf);
	
	syslog_4(LOG_NOTICE, "Instruction cache, ver:%d, capacity:%d, ways:%d, line len:%d",
			cache_ver, cache_size, cache_ways, cache_llen);
	/* wait a while to let info be output */
	sil_dly_nse(5000000);
}
#endif
