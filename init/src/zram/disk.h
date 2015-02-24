/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _ZRAM_DISK_H_
#define _ZRAM_DISK_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "data_types.h"

struct zram_disk_config {
   const char* name;

   size_t size_m;
   unsigned max_comp_streams;
   unsigned comp_algorithm;
};

int zram_disk_config_init (
   struct zram_disk_config* const p_zdisk, const char* const name
);

void zram_disk_config_free ( struct zram_disk_config* const p_zdisk );

struct zram_disk_config* new_zram_disk_config ( const char* const name );

int zram_disk_config_set_size_from_str (
   struct zram_disk_config* const p_zdisk,
   const char* const size_spec,
   const size_t sys_memsize_m
);

char* zram_disk_config_create_size_opt (
   const struct zram_disk_config* const p_zdisk
);

int zram_disk_config_set_size (
   struct zram_disk_config* const p_zdisk,
   const char* const size_spec,
   const size_t fallback_size,
   const size_t sys_memsize_m
);

struct zram_dev_info* zram_disk_do_claim (
   const struct zram_disk_config* const p_zdisk
);

int zram_disk_do_mkfs (
   const struct zram_disk_config* const p_zdisk,
   struct zram_dev_info* const p_dev
);

int zram_disk_do_setup (
   const struct zram_disk_config* const p_zdisk,
   struct zram_dev_info** const pp_dev
);

int zram_disk_do_setup_and_mount (
   const struct zram_disk_config* const p_zdisk,
   const char* const mp,
   const unsigned long flags
);

int zram_disk_simple (
   const char* const name,
   const char* const mp, /* may be NULL */
   const char* const size_spec,
   const size_t sys_memsize_m
);



#endif /* _ZRAM_DISK_H_ */
