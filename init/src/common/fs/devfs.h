/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_DEVFS_H_
#define _COMMON_FS_DEVFS_H_

#include <sys/types.h>
#include <sys/stat.h>

int devfs_check_is_char_dev  ( const char* const path );
int devfs_check_is_block_dev ( const char* const path );

/**
 * Creates a device node.
 * Does not recreate nodes that already exist with the correct major/minor pair.
 *
 * @param path
 * @param devtype
 * @param maj
 * @param min
 * @param mode
 * @param uid
 * @param gid
 *
 * @return 0 on success, 1 on minor issues (chmod failed), -1 on error.
 */
int devfs_donod (
   const char* const  path,
   const mode_t       devtype,
   const unsigned     maj,
   const unsigned     min,
   const mode_t       mode,
   const uid_t        uid,
   const gid_t        gid
);

/** devfs_dochr(path,...) -> devfs_donod(path,S_IFCHR,...) */
int devfs_dochr (
   const char* const  path,
   const unsigned     maj,
   const unsigned     min,
   const mode_t       mode,
   const uid_t        uid,
   const gid_t        gid
);

/** devfs_doblk(path,...) -> devfs_donod(path,S_IFBLK,...) */
int devfs_doblk (
   const char* const  path,
   const unsigned     maj,
   const unsigned     min,
   const mode_t       mode,
   const uid_t        uid,
   const gid_t        gid
);

/**
 * Creates a (block) device node at dev_path for a disk.
 * Reads disk information (major/minor) from sysfs_path.
 * Mode and owner are constants (RW_RW, 0:DEVFS_DISK_GID).
 *
 * @param sysfs_path
 * @param dev_path
 *
 * @return see devfs_donod()
 */
int devfs_create_disk_node (
   const char* const sysfs_path,
   const char* const dev_path
);

/**
 * Reads major/minor from a str and stores them in maj/min.
 *
 * @param str
 * @param maj
 * @param min
 *
 * @return 0 on success, else non-zero
 */
int devfs_parse_maj_min (
   const char* const  str,
   unsigned* const    maj,
   unsigned* const    min
);

/**
 * Reads major/minor from a file and stores them in maj/min
 *
 * @param filepath
 * @param maj
 * @param min
 *
 * @return 0 on success, else non-zero
 */
int devfs_parse_maj_min_from_file (
   const char* const  filepath,
   unsigned* const    maj,
   unsigned* const    min
);

/**
 * Reads major/minor from <sysfs dirpath> + "/dev" and stores them in maj/min.
 *
 * @param dirpath
 * @param maj
 * @param min
 *
 * @return 0 on success, else non-zero
 */
int devfs_parse_maj_min_from_sysfs_dir (
   const char* const  dirpath,
   unsigned* const    maj,
   unsigned* const    min
);

/**
 * Reads major/minor from <sysfs root> + "/block/" + <name> + "/dev"
 * and stores them in maj/min.
 *
 * @param name
 * @param maj
 * @param min
 *
 * @return 0 on success, else non-zero
 */
int devfs_parse_maj_min_from_sysfs_name (
   const char* const  name,
   unsigned* const    maj,
   unsigned* const    min
);

/**
 * Creates some default device nodes (and symlinks) in devfs_dir:
 *
 *  - /dev/fd      -> /proc/self/fd
 *  - /dev/stdin   -> /proc/self/fd/0
 *  - /dev/stdout  -> /proc/self/fd/1
 *  - /dev/stderr  -> /proc/self/fd/2
 *  - /dev/console
 *  - /dev/null
 *  - /dev/ttyS0
 *  - /dev/tty1
 *  - /dev/tty
 *  - /dev/urandom
 *  - /dev/random
 *  - /dev/zero
 *  - /dev/kmsg (non-fatal)
 *  - /dev/pktcdvd/control
 *  - /dev/mapper/control
 *  - /dev/loop-control
 *  - /dev/core
 *  (replace "/dev" with devfs_dir)
 *
 *
 * @param devfs_dir
 *
 * @param 0 on success, else non-zero
 */
int devfs_seed ( const char* const devfs_dir );

#endif /* _COMMON_FS_DEVFS_H_ */
