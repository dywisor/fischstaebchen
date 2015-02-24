/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_DOMOUNT_H_
#define _INITRAMFS_DOMOUNT_H_

#include "../common/fs/mount_config.h"

int initramfs_domount (
   const struct mount_config* const p_mount
);

#endif /* _INITRAMFS_DOMOUNT_H_ */
