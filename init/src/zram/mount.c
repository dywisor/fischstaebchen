/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mount.h>

#include "mount.h"
#include "../common/fs/mount.h"


static int _zram_mount_any (
   struct zram_dev_info* const p_dev,
   const char* const mp, const unsigned long extra_flags,
   const char* const fstype, const char* const opts
);

static int _zram_umount_mp ( const char* const mp );

int zram_umount (
  struct zram_dev_info* const p_dev, const char* const mp
) {
   if ( _zram_umount_mp ( mp ) != 0 ) { return -1; }

   p_dev->status &= ~ZRAM_STATUS_INUSE;
   return 0;
}

int zram_mount (
   struct zram_dev_info* const p_dev,
   const char* const mp, const unsigned long extra_flags
) {
   switch (p_dev->type & ZRAM_TYPE_FSTYPE_MASK) {
      case ZRAM_TYPE_DISK_EXT2:
         return zram_mount_ext2 ( p_dev, mp, extra_flags );

      case ZRAM_TYPE_DISK_EXT4:
         return zram_mount_ext4 ( p_dev, mp, extra_flags );

      case ZRAM_TYPE_DISK_BTRFS:
         return zram_mount_btrfs ( p_dev, mp, extra_flags );

      default:
         errno = EINVAL;
         return -1;
   }
}


int zram_mount_ext2 (
   struct zram_dev_info* const p_dev,
   const char* const mp, const unsigned long extra_flags
) {
   return _zram_mount_any (
      p_dev, mp, extra_flags, "ext2",
      NULL
   );
}

int zram_mount_ext4 (
   struct zram_dev_info* const p_dev,
   const char* const mp, const unsigned long extra_flags
) {
   return _zram_mount_any (
      p_dev, mp, extra_flags, "ext4",
      "commit=240,discard,nobarrier,data=writeback"
   );
}

int zram_mount_btrfs (
   struct zram_dev_info* const p_dev,
   const char* const mp, const unsigned long extra_flags
) {
   return _zram_mount_any (
      p_dev, mp, extra_flags, "btrfs",
      "commit=240,discard,nobarrier"
   );
}


static int _zram_mount_any (
   struct zram_dev_info* const p_dev,
   const char* const mp, const unsigned long extra_flags,
   const char* const fstype, const char* const opts
) {
   if (
      stdmount (
         p_dev->devpath, mp, fstype, ( MS_NOATIME | extra_flags ), opts
      ) == 0
   ) {
      p_dev->status |= ZRAM_STATUS_INUSE;
      return 0;
   } else {
      return -1;
   }
}

static int _zram_umount_mp ( const char* const mp ) {
   errno = 0;
   if ( umount ( mp ) == 0 ) { return 0; }

   switch (errno) {
      case EINVAL:
         return 0;

      default:
         return -1;
   }
}
