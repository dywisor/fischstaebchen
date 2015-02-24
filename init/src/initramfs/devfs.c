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

#include "globals.h"
#include "base.h"
#include "config.h"
#include "devfs.h"

static int _initramfs_wait_for_disk__lvm (
   const unsigned timeout_decisecs, const char* const devpath
);

static int _initramfs_wait_for_disk__mdadm (
   const unsigned timeout_decisecs, const char* const devpath
);

static int _initramfs_wait_for_disk__dev (
   const unsigned timeout_decisecs, const char* const devpath,
   const char* const name
);

static int _initramfs_wait_for_disk__label_or_uuid (
   const unsigned timeout_decisecs, const char* const disk_spec
);


int initramfs_scan_bcache (void) {
   return -1;
}

int initramfs_scan_lvm (void) {
/*
   if ( access ( GET_SYSFS_PATH("/class/misc/device-mapper"), F_OK ) != 0 ) {
      if ( initramfs_modprobe ( 0, "dm-mod" ) != 0 ) { return -1; }
   }
*/

/*
   if ( access ( GET_DEVFS_PATH("/mapper/control"), F_OK ) != 0 ) {
      return -1;
   }
*/

   initramfs_info ( "%s", "\n", "Scanning for volume groups" );

   makedirs ( "/etc/lvm/cache" );
   run_command ( NULL, "vgscan", "--mknodes", "--cache" ); /* retcode ignored */
   return run_command ( NULL, "vgchange", "-a", "-y" );
}

int initramfs_scan_mdadm ( const char* const uuid ) {
   int retcode;
   char* vbuf;

   if ( STR_IS_EMPTY(uuid) ) {
      initramfs_info ( "%s", "\n", "Scanning for software raid arrays" );
      retcode = run_command ( NULL, "mdadm", "--assemble", "--scan" );

   } else {
      if ( asprintf ( &vbuf, "--uuid=%s", uuid ) != 0 ) {
         /* content of vbuf is undefined, possible memleak */
         /* FIXME: POSSIBLE MEMLEAK */
         return -1;
      }

      initramfs_info (
         "Scanning for software raid array with uuid '%s'", "\n", uuid
      );
      retcode = run_command ( NULL, "mdadm", "--assemble", "--scan", vbuf );
      x_free ( vbuf );
   }

   if ( retcode ) {
      initramfs_warn ( "mdadm --assemble failed (rc=%d)", "\n", retcode );
   }

   return retcode;
}



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
         return _initramfs_wait_for_disk__lvm ( timeout_decisecs, disk );

      } else if ( (val = str_startswith ( rem, "md" )) != NULL ) {
         /* mdadm */
         return _initramfs_wait_for_disk__mdadm ( timeout_decisecs, disk );

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


static int _initramfs_wait_for_disk__lvm (
   const unsigned UNUSED(timeout_decisecs), const char* const devpath
) {
   initramfs_scan_lvm();
   return access ( devpath, F_OK );
}


static int _initramfs_wait_for_disk__mdadm (
   const unsigned UNUSED(timeout_decisecs), const char* const devpath
) {
   initramfs_scan_mdadm ( NULL );
   mdev_run_scan();
   return access ( devpath, F_OK );
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
#define BUFSIZE 50

   char msg [BUFSIZE+1];
   ssize_t sret;

   sret = snprintf ( msg, BUFSIZE, "Waiting for disk %s", disk_spec );

   if ( sret < 0 ) {
      *msg = '\0';
   } else if ( sret >= BUFSIZE ) {
      /* BUFSIZE > 2 */
      msg [BUFSIZE-2] = '<';
      msg [BUFSIZE-1] = '>';
   }

   return run_command_until_success_quiet (
      msg, timeout_decisecs, FINDFS_PROGV, disk_spec
   );
#undef BUFSIZE
}
