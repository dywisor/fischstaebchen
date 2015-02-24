/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <unistd.h>
#include <sysexits.h>
#include <sys/swap.h>


#include "../common/misc/run_command.h"
#include "swap.h"
#include "devfs.h"
#include "data_types.h"


int zram_swapon ( struct zram_dev_info* const p_dev ) {
   errno = 0;
   if (
      ( swapon ( p_dev->devpath, _ZRAM_SWAP_FLAGS ) == 0 )
      || ( errno == EBUSY )
   ) {
      p_dev->status |= ZRAM_STATUS_INUSE;
      return 0;
   }

   return -1;
}

int zram_swapoff ( struct zram_dev_info* const p_dev ) {
   errno = 0;
   if (
      ( swapoff ( p_dev->devpath ) == 0 )
      || ( errno = EINVAL )
   ) {
      p_dev->status &= ~ZRAM_STATUS_INUSE;
      return 0;
   }

   return -1;
}

int zram_mkswap ( struct zram_dev_info* const p_dev ) {
   int retcode;

   if (
      (p_dev->type != ZRAM_TYPE_NONE) && (p_dev->type != ZRAM_TYPE_SWAP)
   ) {
      return -5;
   }

   if ( p_dev->status & ZRAM_STATUS_INUSE ) { return -1; }

   if ( zram_mknod ( p_dev ) != 0 ) { return -1; }

   retcode = run_command_quiet (
      BUSYBOX_PROGV_OR("mkswap"), p_dev->devpath
   );


   if ( retcode == 0 ) { p_dev->type = ZRAM_TYPE_SWAP; }

   return retcode;
}
