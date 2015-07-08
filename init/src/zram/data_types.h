/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _ZRAM_DATA_TYPES_H_
#define _ZRAM_DATA_TYPES_H_

#include <stdint.h>
#include <sys/types.h>

#include "globals.h"
#include "config.h"


enum {
   ZRAM_COMP_NONE,

   ZRAM_COMP_LZO,
   ZRAM_COMP_LZ4,

   ZRAM_COMP_UNKNOWN,

   ZRAM_COMP_DEFAULT = ZRAM_DEFAULT_COMPRESSION
};

extern const char* ZRAM_COMPRESSION_DESC[];

enum {
   ZRAM_TYPE_NONE       = 0x0,
   ZRAM_TYPE_SWAP       = 0x1,

   ZRAM_TYPE_DISK       = 0x2,
   ZRAM_TYPE_DISK_EXT2  = 0x4,
   ZRAM_TYPE_DISK_EXT4  = 0x8,
   ZRAM_TYPE_DISK_BTRFS = 0x10,

   ZRAM_TYPE__DUMMY,
   ZRAM_TYPE_FSTYPE_MASK = (
      ZRAM_TYPE_DISK_EXT2 | ZRAM_TYPE_DISK_EXT4 | ZRAM_TYPE_DISK_BTRFS
   )
};

enum {
   ZRAM_STATUS_NONE        = 0,
   ZRAM_STATUS_INITIALIZED = 0x1,
   ZRAM_STATUS_CLAIMED     = 0x2,
   ZRAM_STATUS_INUSE       = 0x4,

   ZRAM_STATUS__DUMMY
};

struct zram_dev_info {
   unsigned      type;
   char*         name;
   char*         path;
   char*         devpath;
   int           comp_algorithm;
   char*         fslabel;
   /* <= 255 identifiers */
   uint8_t       identifier;
   size_t        size_m;
   uint_fast8_t  status;

#if ZRAM_EXPORT_SIZE_B
   /** size in bytes, only set after allocating the device */
   size_t        size_b;
#endif
};


void zram_dev_info_free ( struct zram_dev_info* const p_dev );

__attribute__((warn_unused_result))
struct zram_dev_info* zram_dev_info_new_from_identifier (
   const uint8_t identifier
);

size_t get_zram_dev_size_b ( const struct zram_dev_info* const p_dev );

void zram_dev_set_size_b (
   struct zram_dev_info* const p_dev, const size_t size_b
);

__attribute__((pure))
const char* zram_get_comp_desc    ( const int algi );
const char* zram_get_comp_desc_of ( const struct zram_dev_info* const p_dev );

__attribute__((warn_unused_result))
char* zram_get_zdisk_devpath ( const struct zram_dev_info* const p_dev );

void zram_dev_info_set_fstype (
   struct zram_dev_info* const p_dev, int fstype
);

#endif /* _ZRAM_DATA_TYPES_H_ */
