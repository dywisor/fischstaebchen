/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>

#include "disk.h"
#include "data_types.h"
#include "config.h"
#include "zallocate.h"
#include "mkfs.h"
#include "mount.h"
#include "../common/mac.h"
#include "../common/message.h"
#include "../common/fs/mount_opts.h"

int zram_disk_config_init (
   struct zram_disk_config* const p_zdisk, const char* const name
) {
   p_zdisk->name              = name;
   p_zdisk->size_m            = 0;
   p_zdisk->max_comp_streams  = 0;
   p_zdisk->comp_algorithm    = ZRAM_DEFAULT_COMPRESSION;

   return 0;
}

void zram_disk_config_free ( struct zram_disk_config* const p_zdisk ) {
   p_zdisk->name = NULL;
}

struct zram_disk_config* new_zram_disk_config ( const char* const name ) {
   struct zram_disk_config* p_zdisk;

   RET_NULL_IFNOT_MALLOC_VAR ( p_zdisk );

   if ( zram_disk_config_init ( p_zdisk, name ) != 0 ) {
      x_free ( p_zdisk );
      return NULL;
   }

   return p_zdisk;
}

int zram_disk_config_set_size_from_str (
   struct zram_disk_config* const p_zdisk,
   const char* const size_spec,
   const size_t sys_memsize_m
) {
   size_t size_m;

   if ( parse_tmpfs_size_spec ( size_spec, sys_memsize_m, &size_m ) == 0 ) {
      p_zdisk->size_m = size_m;
      return 0;

   } else {
      return -1;
   }
}

int zram_disk_config_set_size (
   struct zram_disk_config* const p_zdisk,
   const char* const size_spec,
   const size_t fallback_size,
   const size_t sys_memsize_m
) {
   p_zdisk->size_m = 0;

   if ( size_spec != NULL ) {
      if (
         zram_disk_config_set_size_from_str (
            p_zdisk, size_spec, sys_memsize_m
         ) == 0
      ) {
         return 0;
      } else {
         printf_error (
            NULL, "failed to pare size=%s option, resetting size.", "\n",
            size_spec
         );
      }
   }

   if ( fallback_size > 0 ) {
      p_zdisk->size_m = fallback_size;
      return 1;
   }

   if ( sys_memsize_m > 1 ) {
      p_zdisk->size_m = sys_memsize_m / 2;
      return 2;
   }

   return -1;
}

char* zram_disk_config_create_size_opt (
   const struct zram_disk_config* const p_zdisk
) {
   if ( p_zdisk->size_m < 1 ) {
      return NULL;
   } else {
      return create_tmpfs_size_opt ( p_zdisk->size_m );
   }
}

struct zram_dev_info* zram_disk_do_claim (
   const struct zram_disk_config* const p_zdisk
) {
   struct zram_dev_info* p_dev;

   p_dev = NULL;
   if ( zram_claim_next_free ( p_zdisk->size_m, &p_dev ) != 0 ) {
      print_error ( NULL, "zram disk: no free device found!" );
      return NULL;
   }
   return p_dev;
}

int zram_disk_do_mkfs (
   const struct zram_disk_config* const p_zdisk,
   struct zram_dev_info* const p_dev
) {
   int retcode;

   p_dev->fslabel = (char*) (
      (p_zdisk->name == NULL) ? p_dev->name : p_zdisk->name
   );
   retcode        = zram_mkfs ( p_dev );
   p_dev->fslabel = NULL;

   if ( retcode != 0 ) {
      printf_error (
         NULL, "zram disk: mkfs %s failed!", "\n", p_dev->devpath
      );
   }

   return retcode;
}

int zram_disk_do_setup (
   const struct zram_disk_config* const p_zdisk,
   struct zram_dev_info** const pp_dev
) {
   struct zram_dev_info* p_dev;

   if ( pp_dev != NULL ) { *pp_dev = NULL; }

   p_dev = zram_disk_do_claim ( p_zdisk );

   if ( p_dev == NULL ) {
      return 1;

   } else if ( zram_disk_do_mkfs ( p_zdisk, p_dev ) != 0 ) {

      zram_release_device ( p_dev, 1 );

      zram_dev_info_free ( p_dev );
      x_free ( p_dev );

      return 2;
   }

   if ( pp_dev != NULL ) {
      *pp_dev = p_dev;
   } else {
      zram_dev_info_free ( p_dev );
      x_free ( p_dev );
   }

   return 0;
}

int zram_disk_do_setup_and_mount (
   const struct zram_disk_config* const p_zdisk,
   const char* const mp,
   const unsigned long flags
) {
   int esav;
   int retcode;
   struct zram_dev_info* p_dev;

   if ( (mp == NULL) || (*mp == '\0') ) {
      errno = EINVAL;
      return -1;
   }

   retcode = zram_disk_do_setup ( p_zdisk, &p_dev );

   if ( retcode != 0 ) {
      if ( p_dev != NULL ) {
         MSG_PRINT_CODE_ERR (
            "zram_disk_do_setup() failed, but did set p_dev != NULL"
         );
         zram_dev_info_free ( p_dev );
         x_free ( p_dev );
      }
      return retcode;
   }

   errno   = 0;
   retcode = zram_mount ( p_dev, mp, flags );
   esav    = errno;

   if ( retcode != 0 ) {
      printf_error (
         NULL, "failed to mount zram disk %s on %s!", "\n",
         p_dev->devpath, mp
      );

      zram_release_device ( p_dev, 1 );
      retcode = -1;
   }


   zram_dev_info_free ( p_dev );
   x_free ( p_dev );

   errno = esav;
   return retcode;
}

int zram_disk_simple (
   const char* const name,
   const char* const mp, /* may be NULL */
   const char* const size_spec,
   const size_t sys_memsize_m
) {
   int ret;
   struct zram_disk_config zdisk;

   if ( zram_disk_config_init ( &zdisk, name ) != 0 ) { return -1; }

   if (
      zram_disk_config_set_size ( &zdisk, size_spec, 0, sys_memsize_m ) != 0
   ) {
      ret = -1;

   } else if ( mp == NULL ) {
      ret = zram_disk_do_setup ( &zdisk, NULL );

   } else {
      ret = zram_disk_do_setup_and_mount ( &zdisk, mp, MS_RELATIME );
   }

   zram_disk_config_free ( &zdisk );
   return ret;
}
