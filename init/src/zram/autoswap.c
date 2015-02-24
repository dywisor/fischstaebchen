/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/types.h>

#include "autoswap.h"
#include "data_types.h"
#include "../common/misc/sysinfo.h"
#include "../common/message.h"
#include "../common/fs/mount_opts.h"
#include "../common/strutil/compare.h"
#include "../common/strutil/convert.h"

const char* zram_autoswap_guess_fractional ( const size_t m ) {
   if ( m >= 40000 ) {
      return "/7";
   } else if ( m >= 30000 ) {
      return "/6";
   } else if ( m >= 20000 ) {
      return "/5";
   } else if ( m >= 10000 ) {
      return "/4";
   } else {
      return "/2";
   }
}

static int _zram_do_autoswap (
   const size_t num_swaps, const size_t swap_size
);

int zram_autoswap (
   const int num_swaps, const char* const size_spec,
   const size_t sys_memsize_m
) {
   size_t   size_m;
   long     lfrac;

   if ( STR_IS_EMPTY ( size_spec ) ) { return -1; }

   if ( *size_spec == '/' ) {
      /* is a "fractional" */
      if ( str_to_long ( (size_spec+1), &lfrac ) != 0 ) { return -1; }
      if ( (lfrac < 1) || (lfrac > 99) ) { return -1; }

      return zram_autoswap_fractional ( num_swaps, (ushort)lfrac, sys_memsize_m );

   } else {
      /* is a tmpfs-like spec */
      if (
         parse_tmpfs_size_spec ( size_spec, sys_memsize_m, &size_m ) != 0
      ) {
         return -1;
      }

      return zram_do_autoswap ( num_swaps, size_m );
   }
}


int zram_autoswap_from_fractional ( const ushort fractional ) {
   return zram_autoswap_fractional (
      get_cpucount(), fractional, get_memsize_m()
   );
}

int zram_autoswap_fractional (
   const int num_swaps, const ushort fractional, const size_t sys_memsize_m
) {
   return zram_do_autoswap (
      num_swaps,
      calculate_zram_autoswap_size ( num_swaps, fractional, sys_memsize_m )
   );
}

size_t calculate_zram_autoswap_size (
   const int num_swaps, const ushort fractional, const size_t sys_memsize_m
) {
   if ( (num_swaps < 1) || (fractional < 1) ) {
      print_error (
         NULL,
         "cannot calculate autoswap size: invalid num_swaps/fractional."
      );

      errno = EINVAL;
      return 0;
   }

   if ( sys_memsize_m < 1 ) {
      print_error (
         NULL,
         "cannot calculate autoswap size: system memory == 0"
      );

      errno = EINVAL;
      return 0;
   }

   return ( sys_memsize_m / ((unsigned)num_swaps * fractional) );
}

int zram_do_autoswap ( const int num_swaps, const size_t swap_size ) {
   if ( (swap_size < 1) || (num_swaps < 1) ) {
      print_error (
         NULL,
         "cannot set up autoswap: invalid swap size/# of swap devices"
      );

      errno = EINVAL;
      return -1;
   }

   return _zram_do_autoswap ( (unsigned)num_swaps, swap_size );
}

static int _zram_do_autoswap_mkswap (
   const size_t num_swaps, struct zram_dev_info* const* const devices
) {
   ushort k;

   for ( k = 0; k < num_swaps; k++ ) {
      if ( zram_mkswap ( devices[k] ) != 0 ) {
         /* should be logged in swap.c */
         printf_error (
            NULL,
            "mkswap(%s) failed!", "\n", devices[k]->devpath
         );

         zram_release_devices ( (uint8_t)k, devices, 1 );

         return -1;
      }
   }

   print_debug ( NULL, "zram autoswap: mkswap succeeded." );
   return 0;
}

static int _zram_do_autoswap_swapon (
   const size_t num_swaps, struct zram_dev_info* const* const devices
) {
   ushort k;
   ushort j;

   for ( k = 0; k < num_swaps; k++ ) {
      if ( zram_swapon ( devices[k] ) != 0 ) {
         /* should be logged in swap.c */
         printf_error (
            NULL,
            "swapon(%s) failed!", "\n", devices[k]->devpath
         );

         for ( j = 0; j < k; j++ ) { zram_swapoff ( devices[j] ); }
         zram_release_devices ( (uint8_t)num_swaps, devices, 0 );

         return -1;
      }
   }

   print_debug ( NULL, "zram autoswap: swapon succeeded." );
   return 0;
}


static int _zram_do_autoswap (
   const size_t num_swaps, const size_t swap_size
) {
   ushort k;
   int    retcode;
   struct zram_dev_info* devices [num_swaps];

   for ( k = 0; k < num_swaps; k++ ) { devices[k] = NULL; }

   printf_debug (
      NULL,
      "Trying to allocate %zu zram swap devices of %zu size", "\n",
      num_swaps, swap_size
   );

   retcode = -1;

   if (
      zram_claim_several_next_free ( (uint8_t)num_swaps, swap_size, devices ) != 0
   ) {
      print_error ( NULL, "zram autoswap: failed to claim enough devices" );
      /* retcode = -1; */

   } else if ( _zram_do_autoswap_mkswap ( num_swaps, devices ) != 0 ) {
      /* retcode = -1; */

   } else if ( _zram_do_autoswap_swapon ( num_swaps, devices ) != 0 ) {
      /* retcode = -1; */

   } else {
      print_debug ( NULL, "zram autoswap: success." );
      retcode = 0;
   }

   for ( k = 0; k < num_swaps; k++ ) {
      if ( devices[k] != NULL ) { zram_dev_info_free ( devices[k] ); }
   }

   return retcode;
}
