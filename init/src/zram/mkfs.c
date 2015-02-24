/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdlib.h>

#include "mkfs.h"
#include "data_types.h"
#include "devfs.h"
#include "config.h"
#include "../common/misc/run_command.h"
#include "../common/message.h"

#define ZRAM_MKFS_COMMON_EXTFS_OPTS   "-b", "4096"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wused-but-marked-unused"


static int _zram_mkfs_ext2  ( struct zram_dev_info* const p_dev );
static int _zram_mkfs_ext4  ( struct zram_dev_info* const p_dev );
static int _zram_mkfs_btrfs ( struct zram_dev_info* const p_dev );

static int _zram_check_have_mkfs_ext2  (void);
static int _zram_check_have_mkfs_ext4  (void);
static int _zram_check_have_mkfs_btrfs (void);

static int _zram_disk_check_can_init ( struct zram_dev_info* const p_dev );


int check_have_mkfs_ext2 (void) {
   return _zram_check_have_mkfs_ext2();
}

int check_have_mkfs_ext4 (void) {
   return _zram_check_have_mkfs_ext4();
}

int check_have_mkfs_btrfs (void) {
   return _zram_check_have_mkfs_btrfs();
}

unsigned get_best_mkfs_type (void) {
#if ZRAM_DISK_PREFER_BTRFS
   if ( check_have_mkfs_btrfs() == 0 ) { return ZRAM_TYPE_DISK_BTRFS; }
#endif
   if ( check_have_mkfs_ext4()  == 0 ) { return ZRAM_TYPE_DISK_EXT4; }
#if !ZRAM_DISK_PREFER_BTRFS
   if ( check_have_mkfs_btrfs() == 0 ) { return ZRAM_TYPE_DISK_BTRFS; }
#endif
   if ( check_have_mkfs_ext2()  == 0 ) { return ZRAM_TYPE_DISK_EXT2; }
   return ZRAM_TYPE_NONE;
}

int zram_mkfs_ext2 ( struct zram_dev_info* const p_dev ) {
   if ( _zram_disk_check_can_init ( p_dev ) != 0 ) { return -1; }

   if ( _zram_mkfs_ext2 ( p_dev ) != 0 ) { return -1; }

   zram_dev_info_set_fstype ( p_dev, ZRAM_TYPE_DISK_EXT2 );
   return 0;
}

int zram_mkfs_ext4 ( struct zram_dev_info* const p_dev ) {
   if ( _zram_disk_check_can_init ( p_dev ) != 0 ) { return -1; }

   if ( _zram_mkfs_ext4 ( p_dev ) != 0 ) { return -1; }

   zram_dev_info_set_fstype ( p_dev, ZRAM_TYPE_DISK_EXT4 );
   return 0;
}

int zram_mkfs_btrfs ( struct zram_dev_info* const p_dev ) {
   if ( _zram_disk_check_can_init ( p_dev ) != 0 ) { return -1; }

   if ( _zram_mkfs_btrfs ( p_dev ) != 0 ) { return -1; }

   zram_dev_info_set_fstype ( p_dev, ZRAM_TYPE_DISK_BTRFS );
   return 0;
}

int zram_mkfs ( struct zram_dev_info* const p_dev ) {
   if ( _zram_disk_check_can_init ( p_dev ) != 0 ) { return -1; }

   if ( (p_dev->type & ZRAM_TYPE_FSTYPE_MASK) == 0 ) {
      p_dev->type |= get_best_mkfs_type();
   }

   do {
#if ZRAM_DISK_PREFER_BTRFS
      if ( p_dev->type & ZRAM_TYPE_DISK_BTRFS ) {
         printf_debug (
            "(zram)", "Trying to initialize %s as %s", "\n", p_dev->name,
            "btrfs"
         );

         if ( _zram_mkfs_btrfs ( p_dev ) == 0 ) {
            zram_dev_info_set_fstype ( p_dev, ZRAM_TYPE_DISK_BTRFS );
            break;
         }
      }
#endif

      if ( p_dev->type & ZRAM_TYPE_DISK_EXT4 ) {
         printf_debug (
            "(zram)", "Trying to initialize %s as %s", "\n", p_dev->name,
            "ext4 fs"
         );

         if ( _zram_mkfs_ext4 ( p_dev ) == 0 ) {
            zram_dev_info_set_fstype ( p_dev, ZRAM_TYPE_DISK_EXT4 );
            break;
         }
      }

#if !ZRAM_DISK_PREFER_BTRFS
      if ( p_dev->type & ZRAM_TYPE_DISK_BTRFS ) {
         printf_debug (
            "(zram)", "Trying to initialize %s as %s", "\n", p_dev->name,
            "btrfs"
         );

         if ( _zram_mkfs_btrfs ( p_dev ) == 0 ) {
            zram_dev_info_set_fstype ( p_dev, ZRAM_TYPE_DISK_BTRFS );
            break;
         }
      }
#endif

      if ( p_dev->type & ZRAM_TYPE_DISK_EXT2 ) {
         printf_debug (
            "(zram)", "Trying to initialize %s as %s", "\n", p_dev->name,
            "ext2 fs"
         );

         if ( _zram_mkfs_ext2 ( p_dev ) == 0 ) {
            zram_dev_info_set_fstype ( p_dev, ZRAM_TYPE_DISK_EXT2 );
            break;
         }
      }

      return -1;
   } while(0);

   zram_make_devfs_symlink ( p_dev ); /* nonfatal retcode? */
   return 0;
}



static inline int _zram_mkfs_operation_not_supported (void) __attribute__((unused));

static inline int _zram_mkfs_operation_not_supported (void) {
   errno = ENOTSUP;
   return -10;
}

static inline int _zram_mkfs_not_supported_check (void) __attribute__((unused));

static inline int _zram_mkfs_not_supported_check (void) {
   return -10;
}

static int _zram_default_mkfs_supported_check ( const char* const prog ) __attribute__((unused));

static int _zram_default_mkfs_supported_check ( const char* const prog ) {
   return run_command_really_quiet ( NULL, prog, "-V" );
}

static inline int _zram_disk_check_can_init (
   struct zram_dev_info* const p_dev
) {
   if (
      (p_dev->type != ZRAM_TYPE_NONE) && (p_dev->type != ZRAM_TYPE_DISK)
   ) {
      return -5;
   }

   if ( p_dev->status & ZRAM_STATUS_INUSE ) { return -1; }

   return zram_mknod ( p_dev );
}

/* ext2 functionality */
#if ZRAM_DISK_ENABLE_EXT2
#if ENABLE_BUSYBOX
static inline int _check_have_mkfs_ext2_busybox (void) {
   /*
    * could check  "busybox mkfs.ext2 --help"  here, but it's not reliable
    * => simply assume "have mkfs.ext2"
    */
   return 0;
}

static inline int _zram_mkfs_ext2_busybox (
   struct zram_dev_info* const p_dev
) {
   return run_command_really_quiet (
      BUSYBOX_EXE, "mkfs.ext2",
      ZRAM_MKFS_COMMON_EXTFS_OPTS, "-L", p_dev->fslabel, p_dev->devpath
   );
}

#else
static inline int _check_have_mkfs_ext2_busybox (void) {
   return _zram_mkfs_not_supported_check();
}

static inline int _zram_mkfs_ext2_busybox (
   struct zram_dev_info* const p_dev
) {
   return _zram_mkfs_operation_not_supported();
}
#endif

#if ZRAM_DISK_ENABLE_EXT2_EXTERN
static inline int _check_have_mkfs_ext2_nonbusybox (void) {
   return _zram_default_mkfs_supported_check ( MKFS_EXT2_PROG );
}

static inline int _zram_mkfs_ext2_nonbusybox (
   struct zram_dev_info* const p_dev
) {
   return run_command_really_quiet (
      NULL, MKFS_EXT2_PROG,
      ZRAM_MKFS_COMMON_EXTFS_OPTS, "-L", p_dev->fslabel, p_dev->devpath
   );
}
#else
static inline int _check_have_mkfs_ext2_nonbusybox (void) {
   return _zram_mkfs_not_supported_check();
}

static inline int _zram_mkfs_ext2_nonbusybox (
   struct zram_dev_info* const p_dev
) {
   return _zram_mkfs_operation_not_supported();
}
#endif


static int _zram_check_have_mkfs_ext2 (void) {
   if ( _check_have_mkfs_ext2_busybox() == 0 ) { return 0; }
   return _check_have_mkfs_ext2_nonbusybox();
}

static int _zram_mkfs_ext2 (
   struct zram_dev_info* const p_dev
) {
   if ( _zram_mkfs_ext2_busybox ( p_dev ) == 0 ) { return 0; }
   return _zram_mkfs_ext2_nonbusybox ( p_dev );
}
#else

static int _zram_check_have_mkfs_ext2 (void) {
   return _zram_mkfs_not_supported_check();
}

static int _zram_mkfs_ext2 (
   struct zram_dev_info* const p_dev
) {
   return _zram_mkfs_operation_not_supported();
}
#endif

/* ext4 */
#if ZRAM_DISK_ENABLE_EXT4
static int _zram_check_have_mkfs_ext4 (void) {
   return _zram_default_mkfs_supported_check ( MKFS_EXT4_PROG );
}

static int _zram_mkfs_ext4 ( struct zram_dev_info* const p_dev ) {
   return run_command_really_quiet (
      NULL, MKFS_EXT4_PROG,
      ZRAM_MKFS_COMMON_EXTFS_OPTS,
      "-O", "dir_index,extents,filetype,^has_journal,sparse_super,^uninit_bg",
      "-E", "nodiscard",
      "-L", p_dev->fslabel,
      p_dev->devpath
   );
}

#else
static int _zram_check_have_mkfs_ext4 (void) {
   return _zram_mkfs_not_supported_check();
}

static int _zram_mkfs_ext4 ( struct zram_dev_info* const p_dev ) {
   return _zram_mkfs_operation_not_supported();
}
#endif

/* btrfs */
#if ZRAM_DISK_ENABLE_BTRFS
static int _zram_check_have_mkfs_btrfs (void) {
   return _zram_default_mkfs_supported_check ( MKFS_BTRFS_PROG );
}

static int _zram_mkfs_btrfs ( struct zram_dev_info* const p_dev ) {
   return run_command_really_quiet (
      NULL, MKFS_BTRFS_PROG, "-L", p_dev->fslabel, p_dev->devpath
   );
}

#else
static int _zram_check_have_mkfs_btrfs (void) {
   return _zram_mkfs_not_supported_check();
}

static int _zram_mkfs_btrfs ( struct zram_dev_info* const p_dev ) {
   return _zram_mkfs_operation_not_supported();
}
#endif

#pragma clang diagnostic pop
