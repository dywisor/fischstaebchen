/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <sysexits.h>
#include <string.h>

#include "../common/fs/constants.h"
#include "../common/fs/mount_config.h"
#include "../zram/tmpfs.h"

#include "domount.h"
#include "globals.h"

int initramfs_domount (
   const struct mount_config* const p_mount
) {
   int retlatch;

   errno = 0;

   if ( p_mount->cfg & MOUNT_CFG_IS_ZRAM ) {
      retlatch = zram_tmpfs (
         p_mount->source,
         p_mount->target,
         p_mount->flags,
         p_mount->opts,
         NULL,
         initramfs_globals->memsize_m,
         RWX_RX_RX
      );

   } else {
      retlatch = domount_from_config ( p_mount );
   }

   if ( retlatch != DOMOUNT_SUCCESS ) {
      initramfs_err (
         "Failed to mount %s: %s [errno: %s]", "\n",
         ( p_mount->target == NULL ? "<null>" : p_mount->target ),
         domount_strerror(retlatch),
         errno ? strerror(errno) : "<none>"
      );

      return retlatch;
   } else {
      return 0;
   }
}
