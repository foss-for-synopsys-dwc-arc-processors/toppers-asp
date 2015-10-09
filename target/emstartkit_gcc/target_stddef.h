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
 *  the chip dependent part of t_stddef.h (for emstartkit)
 *
 *  This file is included in t_stddef.h. Othe files should not include it
 *  directly. 
 */

#ifndef TOPPERS_TARGET_STDDEF_H
#define TOPPERS_TARGET_STDDEF_H

/*
 *  the target identify macro
 */
#define TOPPERS_EMSTARTKIT               /* system name */

/*
 *  common definitions of tool chain
 * 
 *  If stdint.h is prepared in tool chain, delete TOPPERS_STDINT_TYPE1
 *  and include stdint.h  
 */
#define TOPPERS_STDFLOAT_TYPE1
#include "gcc/tool_stddef.h"

/*
 *  core common definitions
 */
#include "arc_gcc/common/core_stddef.h"


#ifndef TOPPERS_MACRO_ONLY
#include <stdint.h>

/*
 *  the assert abort processing (to be done)
 */
Inline void
TOPPERS_assert_abort(void)
{    
}

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_TARGET_STDDEF_H */
