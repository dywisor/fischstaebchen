/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "../initramfs.h"
#include "../globals.h"
#include "../../common/mac.h"


static int get_any_exe ( const char** const dst, ... ) {
   va_list vargs;
   const char* cand;

   *dst = NULL;
   va_start ( vargs, dst );

   cand = va_arg ( vargs, const char* );
   while ( cand != NULL ) {
      if ( access ( cand, X_OK ) ) {
         *dst = cand;
         va_end ( vargs );
         return 0;
      }
      cand = va_arg ( vargs, const char* );
   }

   va_end ( vargs );
   return -1;
}

int main ( int UNUSED(argc), char** UNUSED(argv) ) {
   static const char* init_argv[2];

   if ( default_initramfs_start() != 0 ) { return EXIT_FAILURE; }

   if ( initramfs_waitfor_disk_devices() != 0 ) {
      return initramfs_run_onerror_shell ( -1, "Failed to wait for disk devices." );
   }

   if ( initramfs_mount_newroot() != 0 ) { return EXIT_FAILURE; }

   if (
      get_any_exe ( &(init_argv [0]),
         "/init.d/doinit", "/init.sh", "/_init.sh", NULL
      ) != 0
   ) {
      return initramfs_run_onerror_shell ( 0, "no init found" );
   }


   init_argv [1] = NULL;

   execv ( init_argv[0], (char**) init_argv );

   initramfs_err ( "Failed to exec: %s", "\n", init_argv[0] );
   sleep(3);
   return EXIT_FAILURE;
}
