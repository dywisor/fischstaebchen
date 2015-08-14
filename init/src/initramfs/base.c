/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include "../common/misc/run_command.h"

#include "base.h"
#include "config.h"
#include "globals.h"

int initramfs_modprobe ( const int quiet, const char* const name ) {
   int retcode;

   if ( quiet == 0 ) {
      initramfs_debug ( "Trying to load module %s", "\n", name );
   }
   retcode = run_command ( BUSYBOX_PROGV_OR("modprobe"), name );

   if ( quiet == 0 ) {
      switch (retcode) {
         case 0:
            initramfs_info ( "Successfully loaded module %s", "\n", name );
            break;

         default:
            initramfs_err ( "Failed to load module %s!", "\n", name );
            break;
      }
   }

   return retcode;
}
