/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _ZRAM_ZALLOCATE_H_
#define _ZRAM_ZALLOCATE_H_

#include <sys/types.h>

#include "data_types.h"

int zram_claim_device    ( struct zram_dev_info* const p_dev );
int zram_release_device  ( struct zram_dev_info* const p_dev, const int force );

__attribute__((warn_unused_result))
struct zram_dev_info* zram_claim_device_from_identifier (
   const uint8_t identifier, const size_t size_m
);

int zram_claim_next_free (
   const size_t size_m, struct zram_dev_info** const pp_dev_out
);

int zram_claim_several_next_free (
   const uint8_t num_devices,
   const size_t size_m,
   struct zram_dev_info** const p_dev_out_arr
);

int zram_claim_several_varsize_next_free (
   const uint8_t num_devices,
   const size_t* const p_size_m_arr,
   struct zram_dev_info** const pp_dev_out_arr
);

int zram_release_devices (
   const uint8_t num_devices,
   struct zram_dev_info* const* const pp_dev_arr,
   int force
);

#endif /* _ZRAM_ZALLOCATE_H_ */
