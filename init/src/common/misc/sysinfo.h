/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_MISC_SYSINFO_H_
#define _COMMON_MISC_SYSINFO_H_

#include <sys/sysinfo.h>
#include <sys/types.h>

size_t  get_memsize_m  (void);
int     get_cpucount   (void);


#endif /* _COMMON_MISC_SYSINFO_H_ */
