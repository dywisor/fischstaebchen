/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <inttypes.h>

#include "data_types.h"
#include "config.h"
#include "globals.h"
#include "../common/mac.h"
#include "../common/strutil/join.h"


const char* ZRAM_COMPRESSION_DESC[] = {
   [ZRAM_COMP_LZO] = "lzo",
   [ZRAM_COMP_LZ4] = "lz4"
};

const char* zram_get_comp_desc ( const int algi ) {
   if ( (algi > ZRAM_COMP_NONE) && (algi < ZRAM_COMP_UNKNOWN) ) {
      return ZRAM_COMPRESSION_DESC [algi];
   } else {
      return NULL;
   }
}

const char* zram_get_comp_desc_of ( const struct zram_dev_info* const p_dev ) {
   return zram_get_comp_desc ( p_dev->comp_algorithm );
}


void zram_dev_info_set_fstype (
   struct zram_dev_info* const p_dev, int fstype
) {
   /* (p_dev->type & ~ZRAM_TYPE_FSTYPE_MASK) | ... */
   p_dev->type = ( ZRAM_TYPE_DISK | ( fstype & ZRAM_TYPE_FSTYPE_MASK ) );
}

#if ZRAM_EXPORT_SIZE_B
void zram_dev_set_size_b (
   struct zram_dev_info* const p_dev, const size_t size_b
) {
   p_dev->size_b = size_b;
}
#else
void zram_dev_set_size_b (
   struct zram_dev_info* const UNUSED(p_dev), const size_t UNUSED(size_b)
) { ; }
#endif


void zram_dev_info_free ( struct zram_dev_info* const p_dev ) {
   RET_VOID_IF_NULL ( p_dev );

   if ( p_dev->name != NULL ) {
      if ( (p_dev->fslabel) == (p_dev->name) ) {
         /* unref shared name */
         p_dev->fslabel = NULL;
      }

      x_free ( p_dev->name );
   }

   x_free ( p_dev->path );
   x_free ( p_dev->devpath );
   x_free ( p_dev->fslabel );
}


struct zram_dev_info* zram_dev_info_new_from_identifier (
   const uint8_t identifier
) {
   char* name_path_triple[3];
   struct zram_dev_info* p_dev;

   if ( get_zram_name_path_triple ( identifier, name_path_triple ) != 0 ) {
      return NULL;
   }

   p_dev = malloc ( sizeof *p_dev );
   if ( p_dev == NULL ) {
      x_free_arr_items ( name_path_triple, 3 );
      return NULL;
   }

   p_dev->type           = ZRAM_TYPE_NONE;
   p_dev->name           = name_path_triple[0];
   p_dev->path           = name_path_triple[1];
   p_dev->devpath        = name_path_triple[2];
   p_dev->identifier     = identifier;
   p_dev->fslabel        = p_dev->name;
   p_dev->comp_algorithm = ZRAM_COMP_DEFAULT;
   p_dev->size_m         = 0;
   p_dev->status         = 0x0;
   zram_dev_set_size_b ( p_dev, 0 );

   name_path_triple[0] = NULL; /* not necessary */
   name_path_triple[1] = NULL; /* not necessary */
   name_path_triple[2] = NULL; /* not necessary */

   return p_dev;
}

size_t get_zram_dev_size_b ( const struct zram_dev_info* const p_dev ) {
   size_t size_b;
   size_t rem;

   size_b = ZRAM_BYTES_TO_MBYTES_FACTOR * (p_dev->size_m);
   if ( zram_globals->system_page_size > 0 ) {
      rem = size_b % zram_globals->system_page_size;
      if ( rem ) {
         size_b = size_b - rem + zram_globals->system_page_size;
      }
   }

   return size_b;
}

char* zram_get_zdisk_devpath ( const struct zram_dev_info* const p_dev ) {
   if ( p_dev->fslabel != NULL ) {
      return join_str_pair (
         (ZRAM_DISK_SYMDIR_PATH "/"), p_dev->fslabel
      );
   } else {
      return NULL;
   }
}
