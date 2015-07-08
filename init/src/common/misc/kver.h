/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_MISC_KVER_H_
#define _COMMON_MISC_KVER_H_

#include <sys/utsname.h>

/** Returns the kernel release str (from uname(2)). */
__attribute__((warn_unused_result))
char* get_kernel_release (void);

__attribute__((warn_unused_result))
char* get_kernel_release_from_uname (
   const struct utsname* const p_uinfo
);

/**
 * Returns the kernel's localversion str (e.g. "-mypc").
 *
 * @param kversion_str    kernel version as returned by get_kernel_release()
 *
 * @return NULL on error, else str (may be empty)
 */
const char* get_localversion ( const char* const kversion_str );

#endif /* _COMMON_MISC_KVER_H_ */
