/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include "ldconfig.h"
#include "config.h"
#include "globals.h"
#include "../common/fs/baseops.h"
#include "../common/misc/run_command.h"


int initramfs_update_ldconfig ( const int quiet ) {
   int retcode;

   if ( check_fs_lexists ( LD_SO_CACHE_FILE ) == 0 ) {
      if ( !quiet ) {
         initramfs_debug (
            "Not running ldconfig - %s exists", "\n", LD_SO_CACHE_FILE
         );
      }
      return 0;
   }

   if ( !quiet ) { initramfs_info ( "%s", "\n", "Running ldconfig" ); }
   retcode = run_command_quiet (
      NULL, LDCONFIG_EXE,
      "-f", LD_SO_CONFIG_FILE,
      "-C", LD_SO_CACHE_FILE
   );

   if ( ! quiet ) {
      if ( retcode ) {
         initramfs_warn (
            "Failed to run ldconfig (returned %d)", "\n", retcode
         );
      } else {
         initramfs_debug ( "%s", "\n", "ldconfig succeeded." );
      }
   }

   return retcode;
}

int initramfs_update_ldconfig_if_exists ( const int quiet ) {
   if ( check_fs_lexists ( LDCONFIG_EXE ) == 0 ) {
      return initramfs_update_ldconfig ( quiet );
   } else {
      if ( !quiet ) {
         initramfs_debug (
            "%s", "\n", "Not running ldconfig - not available."
         );
      }
      return 0;
   }
}
