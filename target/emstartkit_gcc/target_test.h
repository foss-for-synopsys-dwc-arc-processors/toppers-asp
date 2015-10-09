/* Copyright (C) 
 * 2013 - Wayne Ren
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

/*
 *	the chip dependent definitions for test programs (for emstartkit)
 */

#ifndef TOPPERS_TARGET_TEST_H
#define TOPPERS_TARGET_TEST_H

/*
 *  chip common definition
 */
#define CPUEXC1 9

#if defined(TOPPERS_ENABLE_GCOV_PART) || defined(TOPPERS_ENABLE_GCOV_FULL)
#define RAISE_CPU_EXCEPTION	Asm("trap_s 2") 
#else
#define RAISE_CPU_EXCEPTION	Asm("trap_s 1")
#endif /* defined(TOPPERS_ENABLE_GCOV_PART) || defined(TOPPERS_ENABLE_GCOV_FULL) */

#endif /* TOPPERS_TARGET_TEST_H */
