/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "../common/fs/fileio.h"
#include "../common/mac.h"
#include "../common/message.h"

#include "globals.h"
#include "data_types.h"
#include "zallocate.h"



static int _zram_do_release_device ( struct zram_dev_info* const p_dev ) {
   int ret;

   errno = 0;
   ret   = sysfs_write_str ( p_dev->path, "reset", "1" );

   if ( ret == 0 ) {
      p_dev->status &= ~(ZRAM_STATUS_CLAIMED | ZRAM_STATUS_INITIALIZED);
   }
   return ret;
}

static inline int _zram_claim_device_set_compression (
   struct zram_dev_info* const p_dev
) {
   const char* alg_desc;

   alg_desc = zram_get_comp_desc_of ( p_dev );
   if ( alg_desc == NULL ) { return 1; }

   errno = 0;
   if (
      sysfs_write_str ( p_dev->path, "comp_algorithm", alg_desc ) != 0
   ) {
      /* failed to write comp_algorithm, actual alg is unknown */
      p_dev->comp_algorithm = ZRAM_COMP_UNKNOWN;
      return -1;
   }

   return 0;
}

#if ZRAM_CLAIM_CHECK_INITSTATE
static int _zram_claim_device_check_initstate (
   const struct zram_dev_info* const p_dev
) {
   int ret;
   char* val;

   val = NULL;
   if ( sysfs_read_str ( p_dev->path, "initstate", &val ) != 0 ) {
      return -1;
   }

   if ( (val != NULL) && (*val == '0') && (*(val+1) == '\0') ) {
      ret = 0;
   } else {
      errno = EBUSY;
      ret   = 1;
   }

   x_free ( val );
   return ret;
}
#else
static int _zram_claim_device_check_initstate (
   const struct zram_dev_info* const p_dev
) {
   return 0;
}
#endif

static int _zram_claim_device_do_claim (
   struct zram_dev_info* const p_dev, const size_t size_b
) {
   int ret;

   errno = 0;
   ret   = sysfs_write_uint64_decimal ( p_dev->path, "disksize", size_b );
   if ( ret == 0 ) {
      p_dev->status |= (ZRAM_STATUS_CLAIMED | ZRAM_STATUS_INITIALIZED);
   }
   return ret;
}

#if ZRAM_SET_COMPRESSION_AFTER_CLAIM
static inline int _zram_claim_device_and_set_compression (
   struct zram_dev_info* const p_dev, const size_t size_b
) {
   int ret;

   ret = _zram_claim_device_do_claim ( p_dev, size_b );
   if ( ret != 0 ) { return ret; }

   if ( _zram_do_release_device ( p_dev ) != 0 ) {
      printf_error (
         NULL, "Failed to reset zram device %s: %s", NULL,
         p_dev->name, strerror(errno)
      );
      return -1;
   }

   if (
      _zram_claim_device_set_compression ( p_dev ) < 0
   ) {
      printf_warning (
         NULL,
         (
            "Failed to set compression for %s: %s (lost race?), "
            "still trying to reclaim it."
         ),
         NULL, p_dev->name, strerror(errno)
      );
   }

   ret = _zram_claim_device_do_claim ( p_dev, size_b );

   if ( ret != 0 ) {
      printf_error (
         NULL, "Failed to reclaim zram device %s: %s", NULL,
         p_dev->name, strerror(errno)
      );
   }

   return ret;
}
#else
static inline int _zram_claim_device_and_set_compression (
   struct zram_dev_info* const p_dev, const size_t size_b
) {
   IGNORE_RETCODE (
      _zram_claim_device_set_compression ( p_dev )
   );

   return _zram_claim_device_do_claim ( p_dev, size_b );
}
#endif

int zram_claim_device ( struct zram_dev_info* const p_dev ) {
   size_t size_b;

   if ( _zram_claim_device_check_initstate ( p_dev ) != 0 ) {
      /* errno already set */
      printf_debug ( /* preserves errno */
         NULL, "zram device %s is in use (initstate != 0)", NULL,
         p_dev->name
      );
      return -1;
   }

   printf_debug ( NULL, "Trying to claim zram device %s", "\n", p_dev->name );

   if ( (p_dev->status & (ZRAM_STATUS_CLAIMED|ZRAM_STATUS_INITIALIZED)) ) {
      return -10;
   }

   size_b = get_zram_dev_size_b ( p_dev );
   if ( size_b <= 0 ) { return -1; } /* should be compiled as "== 0" */
   zram_dev_set_size_b ( p_dev, size_b );

   return _zram_claim_device_and_set_compression ( p_dev, size_b );
}

struct zram_dev_info* zram_claim_device_from_identifier (
   const uint8_t identifier, const size_t size_m
) {
   struct zram_dev_info* p_dev;
   int ret;

   p_dev = zram_dev_info_new_from_identifier ( identifier );
   p_dev->size_m = size_m;

   ret = zram_claim_device ( p_dev );

   if ( ret != 0 ) {
      switch (errno) {
         case EBUSY:
         case ENODEV:
         case ENOENT:
         case ENXIO:
            break;

         default:
            printf_error ( NULL,
               "Failed to allocate %s: %s", "\n", p_dev->name, strerror(errno)
            );
            break;  /* nop */
      }

      zram_dev_info_free ( p_dev );
      x_free ( p_dev );
   }

   if ( p_dev != NULL ) {
      printf_debug ( NULL, "Successfully allocated %s", "\n", p_dev->name );
   }

   return p_dev;
}

int zram_release_device (
   struct zram_dev_info* const p_dev, const int force
) {
   if ( force == 0 ) {
      if (
         ( (p_dev->status & ZRAM_STATUS_CLAIMED) == 0 )
         && ((p_dev->status & ZRAM_STATUS_INITIALIZED) == 0)
      ) {
         return -2;
      }

      if ( p_dev->status & ZRAM_STATUS_INUSE ) { return -3; }
   }

   return _zram_do_release_device ( p_dev );
}

int zram_release_devices (
   const uint8_t num_devices,
   struct zram_dev_info* const* const pp_dev_arr,
   int force
) {
   int retcode;
   int fret;
   uint_fast16_t numdev_iter;

   retcode = 0;
   for ( numdev_iter = 0; numdev_iter < num_devices; numdev_iter++ ) {
      if ( pp_dev_arr [numdev_iter] != NULL ) {
         fret = zram_release_device ( pp_dev_arr [numdev_iter], force );
         if ( fret ) { retcode = fret; }
      }
   }

   return retcode;
}



static inline int _zram_claim_not_worth_trying_again ( const int e ) {
   return (
         ( e == ENOENT )
      || ( e == EINVAL )
   ) ? 0 : 1;
}

static int zram_claim_next_free__startfrom (
   const uint8_t iter_start,
   const size_t size_m, struct zram_dev_info** const pp_dev_out
) {
   struct zram_dev_info* p_dev;
   uint_fast16_t ident_iter;

   print_debug ( NULL, "claim_next_free() looping" );

   for ( ident_iter = iter_start; ident_iter < 256; ident_iter++ ) {
      p_dev = zram_claim_device_from_identifier ( (uint8_t)ident_iter, size_m );
      if ( p_dev != NULL ) {
         /* found and allocated a zram device, return it  */
         *pp_dev_out = p_dev;
         zram_globals->next_free_identifier = (uint8_t)ident_iter + 1;
         return 0;
      } else if ( _zram_claim_not_worth_trying_again ( errno ) == 0 ) {
         return -2;
      }
   }

   return -1;
}

int zram_claim_next_free (
   const size_t size_m, struct zram_dev_info** const pp_dev_out
) {
   uint8_t next_free_identifier;

   if (  pp_dev_out == NULL ) { return -5; }
   if ( *pp_dev_out != NULL ) { return -6; }

   if ( zram_globals == NULL ) {
      print_error (
         NULL, "zram globals not initialized - cannot claim devices!"
      );
      return -1;
   }

   next_free_identifier = zram_globals->next_free_identifier;

   if ( next_free_identifier > 0 ) {
      if (
         zram_claim_next_free__startfrom (
            next_free_identifier, size_m, pp_dev_out
         ) == 0
      ) {
         return 0;
      }
   }

   return zram_claim_next_free__startfrom ( 0, size_m, pp_dev_out );
}

int zram_claim_several_next_free (
   const uint8_t num_devices,
   const size_t size_m,
   struct zram_dev_info** const pp_dev_out_arr
) {
   int ret;
   int numdev_iter;
   int rev_iter;

   for ( numdev_iter = 0; numdev_iter < num_devices; numdev_iter++ ) {
      pp_dev_out_arr [numdev_iter] = NULL;
      if (
         zram_claim_next_free ( size_m, (pp_dev_out_arr + numdev_iter) ) != 0
      ) {
         /* roll back ... */
         ret = -1;
         for ( rev_iter = (numdev_iter-1); rev_iter > 0; rev_iter-- ) {
            if ( zram_release_device ( pp_dev_out_arr [rev_iter], 0 ) != 0 ) {
               ret = -20;
            }
         }
         return ret;
      }
   }

   return 0;
}

int zram_claim_several_varsize_next_free (
   const uint8_t num_devices,
   const size_t* const p_size_m_arr,
   struct zram_dev_info** const pp_dev_out_arr
) {
   int ret;
   int numdev_iter;
   int rev_iter;

   for ( numdev_iter = 0; numdev_iter < num_devices; numdev_iter++ ) {
      pp_dev_out_arr [numdev_iter] = NULL;
      if (
         zram_claim_next_free (
            p_size_m_arr[numdev_iter],
            (pp_dev_out_arr + numdev_iter)
         ) != 0
      ) {
         /* roll back ... */
         ret = -1;
         for ( rev_iter = (numdev_iter-1); rev_iter >= 0; rev_iter-- ) {
            if ( zram_release_device ( pp_dev_out_arr [rev_iter], 0 ) != 0 ) {
               ret = -20;
            }
         }
         return ret;
      }
   }

   return 0;
}
