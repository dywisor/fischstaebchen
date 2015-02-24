/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_BASELAYOUT_BASEMOUNTS_H_
#define _COMMON_BASELAYOUT_BASEMOUNTS_H_

int basemounts_mount_proc   (const char* const mp);
int basemounts_mount_sysfs  (const char* const mp);
int basemounts_mount_tmp    (const char* const mp);
int basemounts_mount_run    (const char* const mp);

int basemounts_mount_devfs_tmpfs    (const char* const mp);
int basemounts_mount_devfs_devtmpfs (const char* const mp);
int basemounts_mount_devfs          (const char* const mp);
int basemounts_mount_devfs_shm      (const char* const mp);
int basemounts_mount_devfs_pts      (const char* const mp);


int basemounts_mount_and_populate_devfs (void);
int basemounts_all (void);

#endif /* _COMMON_BASELAYOUT_BASEMOUNTS_H_ */
