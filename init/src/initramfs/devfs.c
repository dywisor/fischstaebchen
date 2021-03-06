/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "../common/mac.h"

#include "../common/baselayout/mdev.h"

#include "../common/fs/baseops.h"
#include "../common/fs/constants.h"
#include "../common/fs/devfs.h"

#include "../common/misc/run_command.h"

#include "../common/strutil/compare.h"
#include "../common/strutil/format.h"

#include "globals.h"
#include "base.h"
#include "config.h"
#include "devfs.h"

static int _initramfs_wait_for_disk__dev (
   const unsigned timeout_decisecs, const char* const devpath,
   const char* const name
);

static int _initramfs_wait_for_disk__label_or_uuid (
   const unsigned timeout_decisecs, const char* const disk_spec
);


int initramfs_wait_for_disk (
   const unsigned timeout_decisecs, const char* const disk
) {
   const char* rem;
   const char* val;

   if ( STR_IS_EMPTY(disk) ) { return -1; }

   if ( *disk == '/' ) {
      if ( access ( disk, F_OK ) == 0 ) { return 0; }

      rem = str_startswith ( disk, "/dev/" );

      if ( rem == NULL ) {
         /* netshare or invalid path */
         return -1;
      }

      /*
       * could check for disk/by-*,
       * but there's no functionality to create symlinks in these dirs,
       * so mounting disk would fail anyway
       */

      if ( (val = str_startswith ( rem, "vg" )) != NULL ) {
         /* LVM */
         return -1;

      } else if ( (val = str_startswith ( rem, "md" )) != NULL ) {
         /* mdadm */
         return -1;

      } else if ( strchr ( rem, '/' ) != NULL ) {
         /* not a device node we could handle here */
         return -1;

      } else if (
            ( (val = str_startswith ( rem, "sd" )) != NULL )
         || ( (val = str_startswith ( rem, "hd" )) != NULL )
      ) {
         /* could be a device node */
         return _initramfs_wait_for_disk__dev ( timeout_decisecs, disk, rem );
      }

   } else if (
         ( (val = str_startswith ( disk, "LABEL=" )) != NULL )
      || ( (val = str_startswith ( disk, "UUID=" )) != NULL )
      || ( (val = str_startswith ( disk, "PARTUUID=" )) != NULL )
   ) {
      return _initramfs_wait_for_disk__label_or_uuid (
         timeout_decisecs, disk
      );
   }

   return -1;
}





static int _initramfs_wait_for_disk__dev (
   const unsigned UNUSED(timeout_decisecs), const char* const devpath,
   const char* const UNUSED(name)
) {
   mdev_run_scan();
   return access ( devpath, F_OK );
}

static int _initramfs_wait_for_disk__label_or_uuid (
   const unsigned timeout_decisecs, const char* const disk_spec
) {
#define BUFSIZE 51
   char msg [BUFSIZE];

   switch (
      str_format ( NULL, msg, BUFSIZE, "Waiting for disk %s", disk_spec )
   ) {
      case STRFORMAT_RET_SUCCESS:
         break;

      case STRFORMAT_RET_BUF_TO_SMALL:
         /* BUFSIZE > 2 */
         msg [BUFSIZE-3] = '<';
         msg [BUFSIZE-2] = '>';
         break;

      default:
         msg [0] = '\0';
         break;
   }

   return run_command_until_success_quiet (
      msg, timeout_decisecs, FINDFS_PROGV, disk_spec
   );
#undef BUFSIZE
}
