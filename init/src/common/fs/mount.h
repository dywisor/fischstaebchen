/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_MOUNT_H_
#define _COMMON_FS_MOUNT_H_

/**
 * Mounts a filesystem using mount(2).
 * Creates the mountpoint directory if necessary.
 *
 * @param fs
 * @param mp
 * @param fstype
 * @param flags
 * @param extra_opts
 *
 * @return 0 on success, else non-zero
 */
int stdmount (
   const char* const    fs,
   const char* const    mp,
   const char* const    fstype,
   const unsigned long  flags,
   const char* const    extra_opts
);

/**
 * Mounts a filesystem using an external mount program.
 * Creates the mountpoint directory if necessary.
 *
 * @param fs
 * @param mp
 * @param fstype
 * @param flags
 * @param extra_opts
 *
 * @return 0 on success, else non-zero
 */
int stdmount_external (
   const char* const    fs,
   const char* const    mp,
   const char* const    fstype,
   const unsigned long  flags,
   const char* const    extra_opts
);

/**
 * Moves a mount.
 * This is identical to "stdmount(src, dst, NULL, MS_MOVE, NULL)".
 *
 * @param src
 * @param dst
 *
 * @return 0 on success, else non-zero.
 */
int mount_move ( const char* const src, const char* const dst );

/**
 * Remounts a filesystem.
 *
 * @param mp
 * @param flags
 * @param opts
 *
 * @return 0 on success, else non-zero.
 */
int remount (
   const char* const mp, const unsigned long flags, const char* const opts
);

/**
 * Remounts a filesystem in readonly mode.
 *
 * @param mp
 * @param flags   (MS_RDONLY gets added to the effective mount flags)
 * @param opts
 *
 * @return 0 on success, else non-zero.
 */
int remount_ro (
   const char* const mp, const unsigned long flags, const char* const opts
);

/**
 * Remounts a filesystem in read-write mode.
 *
 * @param mp
 * @param flags   (~MS_RDONLY gets added to the effective mount flags)
 * @param opts
 *
 * @return 0 on success, else non-zero.
 */
int remount_rw (
   const char* const mp, const unsigned long flags, const char* const opts
);

/**
 * Bind-mounts a filesystem.
 * This is identical to "stdmount(fs, bind_mp, NULL, (flags | MS_BIND), NULL)".
 *
 * @param fs
 * @param bind_mp
 * @param flags
 */
int bind_mount (
   const char* const fs, const char* const bind_mp, const unsigned long flags
);

#endif /* _COMMON_FS_MOUNT_H_ */
