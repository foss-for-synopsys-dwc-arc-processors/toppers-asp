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
