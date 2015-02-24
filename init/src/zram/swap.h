/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _ZRAM_SWAP_H_
#define _ZRAM_SWAP_H_

#include <unistd.h>
#include <sys/swap.h>

#include "data_types.h"
#include "config.h"

/* uclibc */
#ifndef SWAP_FLAG_DISCARD
#define SWAP_FLAG_DISCARD 0x10000
#endif

#define _ZRAM_SWAP_PRIOFLAG ( (-1 << SWAP_FLAG_PRIO_SHIFT) & SWAP_FLAG_PRIO_MASK )
#define _ZRAM_SWAP_FLAGS    _ZRAM_SWAP_PRIOFLAG | SWAP_FLAG_DISCARD


int zram_mkswap  ( struct zram_dev_info* const p_dev );
int zram_swapon  ( struct zram_dev_info* const p_dev );
int zram_swapoff ( struct zram_dev_info* const p_dev );

#endif /* _ZRAM_SWAP_H_ */
