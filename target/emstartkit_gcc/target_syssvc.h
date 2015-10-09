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
 *  the chip dependent part of system service (for emstartkit)
 *
 *   the include file of chip dependent part of system service.
 */

#ifndef TOPPERS_TARGET_SYSSVC_H
#define TOPPERS_TARGET_SYSSVC_H

/*
 *  trace log related definition
 */
#ifdef TOPPERS_ENABLE_TRACE
#include "logtrace/trace_config.h"
#endif /* TOPPERS_ENABLE_TRACE */


/*
 *  the target name in startup message
 */
#define TARGET_NAME    "EM_START_KIT(ARC)"

/*
 *  the low level output function for syslog
 *
 *  output/save the char (c) using the target dependent function.
 */
extern void	target_fput_log(char_t c);

#ifdef TOPPERS_ENABLE_TRACE
/*
 *  output the trace log file
 */
extern void	target_fput_log_file(char_t c);
#endif /* TOPPERS_ENABLE_TRACE */

/*
 *  serial port number
 */
#define TNUM_PORT        2        /* the number of supported serial port */

/*
 *  the related constants for syslog task
 *
 *  use the default setting
 */

#endif /* TOPPERS_TARGET_SYSSVC_H */
