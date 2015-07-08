/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _ZRAM_AUTOSWAP_H_
#define _ZRAM_AUTOSWAP_H_

#include <stdint.h>
#include <sys/types.h>

#include "swap.h"
#include "zallocate.h"


size_t calculate_zram_autoswap_size (
   const int num_swaps, const ushort fractional, const size_t sys_memsize_m
);

int zram_do_autoswap ( const int num_swaps, const size_t swap_size );

int zram_autoswap_fractional (
   const int num_swaps, const ushort fractional, const size_t sys_memsize_m
);

int zram_autoswap_from_fractional ( const ushort fractional );

int zram_autoswap (
   const int num_swaps, const char* const size_spec,
   const size_t sys_memsize_m
);

__attribute__((const))
const char* zram_autoswap_guess_fractional ( const size_t sys_memsize_m );


#endif /* _ZRAM_AUTOSWAP_H_ */
