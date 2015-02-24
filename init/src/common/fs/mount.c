/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mount.h"
#include "constants.h"
#include "baseops.h"
#include "mount_opts.h"
#include "../mac.h"
#include "../message.h"
#include "../misc/run_command.h"
#include "../strutil/compare.h"

static int _stdmount_makedirs ( const char* const mp ) {
   errno = 0;
   if ( makedirs_mode ( mp, DEFDIRPERM, DEFMPDIRPERM ) != 0 ) {
      if (errno) { perror ( "stdmount->makedirs" ); }
      printf_error (
         NULL, "failed to create mountpoint dir %s", "\n", mp
      );
      return -1;
   }
   return 0;
}

int stdmount (
   const char* const fs,
   const char* const mp,
   const char* const fstype,
   const unsigned long flags,
   const char* const extra_opts
) {
   int retcode;

   if ( _stdmount_makedirs ( mp ) != 0 ) { return -1; }

   errno   = 0;
   retcode = mount ( fs, mp, fstype, flags, (const void*) extra_opts );
   if ( retcode != 0 ) {
      if (errno) { perror ( "stdmount->mount" ); }
      printf_error (
         NULL, "Failed to mount %s on %s type %s", "\n",
         fs, mp, (fstype == NULL ? "none" : fstype)
      );
   }

   return retcode;
}


int stdmount_external (
   const char* const    fs,
   const char* const    mp,
   const char* const    fstype,
   const unsigned long  flags,
   const char* const    extra_opts
) {
   int   retcode;
   char* opts_str;

   opts_str = get_mount_opts_str ( flags, extra_opts );
   /* a valid opts_str is never empty (contains at least "rw") */
   if ( STR_IS_EMPTY(opts_str) ) { return -1; }

   retcode = -1;

   if ( _stdmount_makedirs ( mp ) == 0 ) {
      retcode = run_command (
         BUSYBOX_PROGV_OR("mount"), "-t", fstype, "-o", opts_str, fs, mp
      );
   }

   x_free ( opts_str );
   return retcode;
}

int mount_move ( const char* const src, const char* const dst ) {
   if ( _stdmount_makedirs ( dst ) != 0 ) { return -1; }

   errno = 0;
   return mount ( src, dst, NULL, MS_MOVE, NULL );
}

int remount (
   const char* const mp, const unsigned long flags, const char* const opts
) {
   errno = 0;
   return mount ( NULL, mp, NULL, flags, opts );
}

int remount_ro (
   const char* const mp, const unsigned long flags, const char* const opts
) {
   return remount ( mp, ( flags | MS_RDONLY ), opts );
}

int remount_rw (
   const char* const mp, const unsigned long flags, const char* const opts
) {
   return remount ( mp, ( flags & (unsigned long)~MS_RDONLY ), opts );
}

int bind_mount (
   const char* const fs, const char* const bind_mp, const unsigned long flags
) {
   if ( _stdmount_makedirs ( bind_mp ) != 0 ) { return -1; }

   errno = 0;
   return mount ( fs, bind_mp, NULL, ( flags | MS_BIND ), NULL );
}
