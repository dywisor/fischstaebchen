/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _ZRAM_CONFIG_H_
#define _ZRAM_CONFIG_H_

/*#include "local_config.h"*/

#include "../common/config.h"

#ifndef ZRAM_DEFAULT_IDENTIFIER_ENVVAR
#define ZRAM_DEFAULT_IDENTIFIER_ENVVAR "__ZRAM_NEXT_FREE_IDENT"
#endif

#ifndef ZRAM_DEFAULT_COMPRESSION
#define ZRAM_DEFAULT_COMPRESSION  ZRAM_COMP_LZ4
#endif

/*
 * If set to 0, zram_claim_device() blindly writes the compression to a
 * zram device before actually claiming it.
 *
 * A non-zero value causes zram_claim_device() to claim the zram dev first,
 * immediately release it, set compression and finally reclaim it.
 *
 * Setting this to "0" is more efficient,
 * but produces warn message in the kernel log.
 */
#ifndef ZRAM_SET_COMPRESSION_AFTER_CLAIM
#define ZRAM_SET_COMPRESSION_AFTER_CLAIM  1
#endif

/*
 * Set to 1 if zram_claim_device() should check the initstate and immediately
 * fail if it is not 0.
 *
 * Racy, but less noisy.
 *
 * Note: A non-zero value is assumed if the initstate file cannot be read.
 */
#ifndef ZRAM_CLAIM_CHECK_INITSTATE
#define ZRAM_CLAIM_CHECK_INITSTATE  1
#endif

#define ZRAM_SWAP_PRIORITY  2000


#ifndef ZRAM_EXPORT_SIZE_B
#define ZRAM_EXPORT_SIZE_B  0
#endif

#ifndef ZRAM_DISK_PREFER_BTRFS
#define ZRAM_DISK_PREFER_BTRFS  0
#endif

#ifndef ZRAM_DISK_MAKESYM
#define ZRAM_DISK_MAKESYM  1
#endif

#ifndef ZRAM_DISK_SYMDIR
#define ZRAM_DISK_SYMDIR  "zdisk"
#endif

#define ZRAM_DISK_SYMDIR_PATH   GET_DEVFS_PATH("/" ZRAM_DISK_SYMDIR)

#ifndef ZRAM_DISK_ENABLE_EXT2_EXTERN
#define ZRAM_DISK_ENABLE_EXT2_EXTERN  1
#endif

#ifndef ZRAM_DISK_ENABLE_EXT2
#define ZRAM_DISK_ENABLE_EXT2  1
#endif

#ifndef ZRAM_DISK_ENABLE_EXT4
#define ZRAM_DISK_ENABLE_EXT4  1
#endif

#ifndef ZRAM_DISK_ENABLE_BTRFS
#define ZRAM_DISK_ENABLE_BTRFS  1
#endif


#ifndef MKFS_EXT2_PROG
#define MKFS_EXT2_PROG "mkfs.ext2"
#endif

#ifndef MKFS_EXT4_PROG
#define MKFS_EXT4_PROG "mkfs.ext4"
#endif

#ifndef MKFS_BTRFS_PROG
#define MKFS_BTRFS_PROG "mkfs.btrfs"
#endif


enum {
   ZRAM_CONFIG_H__NOT_EMPTY
};

#endif /* _ZRAM_CONFIG_H_ */
