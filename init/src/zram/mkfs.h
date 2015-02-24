/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _ZRAM_MKFS_H_
#define _ZRAM_MKFS_H_

#include "data_types.h"
#include "config.h"


int zram_mkfs_ext2  ( struct zram_dev_info* const p_dev );
int zram_mkfs_ext4  ( struct zram_dev_info* const p_dev );
int zram_mkfs_btrfs ( struct zram_dev_info* const p_dev );
int zram_mkfs       ( struct zram_dev_info* const p_dev );

int check_have_mkfs_ext2    (void);
int check_have_mkfs_ext4    (void);
int check_have_mkfs_btrfs   (void);
unsigned get_best_mkfs_type (void);



#endif /* _ZRAM_MKFS_H_ */
