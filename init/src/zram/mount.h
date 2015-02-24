/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _ZRAM_MOUNT_H_
#define _ZRAM_MOUNT_H_

#include "data_types.h"

int zram_mount (
   struct zram_dev_info* const p_dev,
   const char* const mp, const unsigned long extra_flags
);

int zram_umount (
   struct zram_dev_info* const p_dev, const char* const mp
);

int zram_mount_ext2 (
   struct zram_dev_info* const p_dev,
   const char* const mp, const unsigned long extra_flags
);

int zram_mount_ext4 (
   struct zram_dev_info* const p_dev,
   const char* const mp, const unsigned long extra_flags
);

int zram_mount_btrfs (
   struct zram_dev_info* const p_dev,
   const char* const mp, const unsigned long extra_flags
);




#endif /* _ZRAM_MOUNT_H_ */
