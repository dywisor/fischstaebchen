/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _ZRAM_DEVFS_H_
#define _ZRAM_DEVFS_H_

#include "data_types.h"


int zram_mknod ( const struct zram_dev_info* const p_dev );
int zram_make_devfs_symlink ( struct zram_dev_info* const p_dev );


#endif
