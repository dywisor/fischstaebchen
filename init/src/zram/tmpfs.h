/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _ZRAM_TMPFS_H_
#define _ZRAM_TMPFS_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "data_types.h"
#include "disk.h"
#include "../common/fs/mount_opts.h"
#include "../common/fs/mount_config.h"

struct zram_tmpfs_config {
   /* also contains the "name" attribute */
   struct zram_disk_config zdisk;

   const char* mp;

   unsigned long flags;
   char* tmpfs_opts_str;

   /* zram-only options */
   mode_t mode;
   uid_t  uid;
   gid_t  gid;
};

int zram_do_zram_tmpfs (
   const struct zram_tmpfs_config* const p_zmount
);

int zram_do_normal_tmpfs (
   const struct zram_tmpfs_config* const p_zmount
);

/**
 * Tries to mount a zram disk at the given mountpoint and falls back
 * to plain tmpfs on error.
 *
 * The default zram disk size is 50% of the system memory.
 * Note that only the supported size specifier is "size=";
 * nr_blocks has no effect when trying to mount the zram device.
 *
 * The following zram setup options may be appear in the opts str and are
 * handled specially
 * * comp_algorithm=lzo|lz4 / comp=  [FUTURE / NOT IMPLEMENTED]
 * * max_comp_streams=N              [FUTURE / NOT IMPLEMENTED]
 *
 * All other mount options get filtered out for zram dev mounts.
 *
 * @param name             name of the disk/tmpfs (may be NULL)
 * @param mp               mountpoint  (must not be NULL)
 * @param flags            mount flags (can be 0)
 * @param opts             options str
 *                          (may be NULL and can contain flag options like "ro")
 * @param size_spec        overrides the size= option (may be NULL)
 * @param sys_memsize_m    max system memory size in megabytes
 *                          (can be 0 -> autodetect)
 * @param default_mode     default mount mode
 *
 * @return 0 if zram disk mounted, 1 if tmpfs mounted, -1 on error
 */
int zram_tmpfs (
   const char* const name,
   const char* const mp,
   unsigned long flags,
   const char* const opts,
   const char* const size_spec,
   const size_t sys_memsize_m,
   const mode_t default_mode
);

int zram_tmpfs_config_init (
   struct zram_tmpfs_config* const p_zmount,
   const char* const name,
   const char* const mp,
   const mode_t default_mode
);

void zram_tmpfs_config_free (
   struct zram_tmpfs_config* const p_zmount
);

int zram_tmpfs_config_parse (
   struct zram_tmpfs_config* const p_zmount,
   const char* const name,
   const char* const mp,
   unsigned long flags,
   const char* const opts,
   const char* const size_spec,
   const size_t sys_memsize_m,
   const mode_t default_mode
);

struct zram_tmpfs_config* new_zram_tmpfs_config (
   const char* const name,
   const char* const mp,
   unsigned long flags,
   const char* const opts,
   const char* const size_spec,
   const size_t sys_memsize_m,
   const mode_t default_mode
);

#endif /* _ZRAM_TMPFS_H_ */
