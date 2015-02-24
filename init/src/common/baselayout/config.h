/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_BASELAYOUT_CONFIG_H_
#define _COMMON_BASELAYOUT_CONFIG_H_

#include <stdlib.h>
#include <sys/mount.h>

/* or include baselayout/config.h in ../config.h */
#include "../config.h"

#ifndef ENABLE_BUSYBOX
#error BUSYBOX not configured
#endif

#ifndef BUSYBOX_EXE
#error BUSYBOX_EXE not configured
#endif

/* PATH */
#ifndef DEFAULT_PATH
#define DEFAULT_PATH "/sbin:/usr/sbin:/bin:/usr/bin"
#endif


/*
 * basemounts:
 * +- /dev
 * |  +- /dev/pts
 * |  +- /dev/shm
 * |  +- mdev
 * +- /proc
 * +- /sys
 * +- /run
 * +- /tmp
 * +- /newroot
 *
 */

/* /dev */
#ifndef DEVFS_MOUNTPOINT
#define DEVFS_MOUNTPOINT  DEVFS_PATH
#endif

#ifdef  DISABLE_DEVTMPFS
#undef  ENABLE_DEVTMPFS
#define ENABLE_DEVTMPFS  0
#endif

#ifndef ENABLE_DEVTMPFS
#define ENABLE_DEVTMPFS  1
#endif

#ifndef DEVFS_TMPFS_OPTS
#define DEVFS_TMPFS_OPTS  MS_NOSUID|MS_RELATIME
#endif

#ifndef DEVFS_TMPFS_OPTS_STR
#define DEVFS_TMPFS_OPTS_STR "size=10240k,nr_inodes=64012,mode=0755"
#endif

#ifndef DEVFS_DEVTMPFS_OPTS
#define DEVFS_DEVTMPFS_OPTS  DEVFS_TMPFS_OPTS
#endif

#ifndef DEVFS_DEVTMPFS_OPTS_STR
#define DEVFS_DEVTMPFS_OPTS_STR DEVFS_TMPFS_OPTS_STR
#endif

/* /dev/pts */
#ifndef DEVFS_PTS_MOUNTPOINT
#define DEVFS_PTS_MOUNTPOINT  DEVFS_MOUNTPOINT "/pts"
#endif

#ifndef DEVFS_PTS_OPTS
#define DEVFS_PTS_OPTS  MS_NOSUID|MS_NOEXEC|MS_RELATIME
#endif

#ifndef DEVFS_PTS_OPTS_STR
#define DEVFS_PTS_OPTS_STR  "gid=5,mode=620"
#endif

/* /dev/shm */
#ifndef DEVFS_SHM_MOUNTPOINT
#define DEVFS_SHM_MOUNTPOINT  DEVFS_MOUNTPOINT "/shm"
#endif

#ifndef DEVFS_SHM_OPTS
#define DEVFS_SHM_OPTS  MS_NODEV|MS_NOEXEC
#endif

#ifndef DEVFS_SHM_OPTS_STR
#define DEVFS_SHM_OPTS_STR  NULL
#endif


/* /dev: mdev */
#ifdef  DISABLE_MDEV
#undef  ENABLE_MDEV
#define ENABLE_MDEV  0
#endif

#ifndef ENABLE_MDEV
#define ENABLE_MDEV  1
#endif

#ifndef MDEV_EXE
#define MDEV_EXE "/sbin/mdev"
#endif

#ifndef MDEV_SEQ
#define MDEV_SEQ  1
#endif

#ifndef MDEV_LOG
#define MDEV_LOG  1
#endif

/* /proc */
#ifndef PROC_MOUNTPOINT
#define PROC_MOUNTPOINT  PROC_PATH
#endif

#ifndef PROC_OPTS
#define PROC_OPTS MS_NODEV|MS_NOSUID|MS_NOEXEC|MS_RELATIME
#endif

#ifndef PROC_OPTS_STR
#define PROC_OPTS_STR  NULL
#endif

/* /sys */
#ifndef SYSFS_MOUNTPOINT
#define SYSFS_MOUNTPOINT  SYSFS_PATH
#endif

#ifndef SYSFS_OPTS
#define SYSFS_OPTS  MS_NODEV|MS_NOSUID|MS_NOEXEC|MS_RELATIME
#endif

#ifndef SYSFS_OPTS_STR
#define SYSFS_OPTS_STR  NULL
#endif

/* /run */
#ifndef BASEMOUNTS_MOUNT_RUN
#define BASEMOUNTS_MOUNT_RUN  1
#endif

#ifndef BASEMOUNTS_SYMLINK_VAR_RUN
#define BASEMOUNTS_SYMLINK_VAR_RUN  1
#endif

#ifndef RUN_MOUNTPOINT
#define RUN_MOUNTPOINT  "/run"
#endif

#ifndef RUN_MOUNT_OPTS
#define RUN_MOUNT_OPTS MS_NOSUID|MS_NODEV
#endif

#ifndef RUN_MOUNT_OPTS_STR
#define RUN_MOUNT_OPTS_STR "mode=0755"
#endif

/* /tmp */
#ifndef BASEMOUNTS_MOUNT_TMP
#define BASEMOUNTS_MOUNT_TMP  1
#endif

#ifndef TMP_MOUNTPOINT
#define TMP_MOUNTPOINT  "/tmp"
#endif

#ifndef TMP_MOUNT_OPTS
#define TMP_MOUNT_OPTS MS_NOSUID|MS_NOEXEC|MS_NODEV|MS_RELATIME
#endif

#ifndef TMP_MOUNT_OPTS_STR
#define TMP_MOUNT_OPTS_STR  NULL
#endif

/* /newroot */

#ifndef NEWROOT_MOUNTPOINT
#define NEWROOT_MOUNTPOINT  "/newroot"
#endif

#define GET_NEWROOT_PATH(p)  NEWROOT_MOUNTPOINT p

#endif /* _COMMON_BASELAYOUT_CONFIG_H_ */
