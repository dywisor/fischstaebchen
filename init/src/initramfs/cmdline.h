/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_CMDLINE_H_
#define _INITRAMFS_CMDLINE_H_

#include "globals.h"

int _initramfs_process_cmdline (struct initramfs_globals_type* const g);

int initramfs_process_cmdline (void);

#endif /* _INITRAMFS_CMDLINE_H_ */
