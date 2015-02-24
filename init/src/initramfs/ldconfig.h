/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_HACKS_LDCONFIG_H_
#define _INITRAMFS_HACKS_LDCONFIG_H_

#include "config.h"

int initramfs_update_ldconfig ( const int quiet );
int initramfs_update_ldconfig_if_exists ( const int quiet );

#endif /* _INITRAMFS_HACKS_LDCONFIG_H_ */
