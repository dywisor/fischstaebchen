/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_UMOUNT_H_
#define _COMMON_FS_UMOUNT_H_

/**
 * Unmounts a filesystem (specified by its mountpoint).
 *
 * @param mp  mountpoint
 *
 * @return  0 on success,
 *         -1 on error,
 *          1 if not unmounted due to missing mount (ENOENT / EINVAL)
 *          2 if unmounted in lazy mode (MNT_DETACH)
 */
int do_umount ( const char* const mp );

/**
 * Recursively unmounts all mounts in/under the specified root dir.
 *
 * Mounts with higher depth (i.e. number of slash chars '/') get unmounted
 * first.
 *
 * NOT suitable for unmounting aufs mounts like:
 *  aufs_branch on /root/.branch1
 *  aufs_branch on /root/.branch2
 *  aufs on /root with ^those branches
 *
 * @param root
 * @param mindepth  (any value <= 0: include root)
 *
 * @return  -1 on error, else 0
 */
int do_umount_recursive ( const char* const root, const int mindepth );

/** same as do_umount_recursive ( root, 1 ) */
int umount_all_in    ( const char* const root );

/** same as do_umount_recursive ( root, 0 ) */
int umount_recursive ( const char* const root );


#endif /* _COMMON_FS_UMOUNT_H_ */
