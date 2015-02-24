/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_PREMOUNT_H_
#define _INITRAMFS_PREMOUNT_H_

int initramfs_premount_all (
   const char* const root, const char* const fstab_file
);

int initramfs_premount_all_nofilter (
   const char* const root, const char* const fstab_file
);

int initramfs_newroot_premount ( const char* const fstab_file );

int initramfs_umount_all_in ( const char* const root );

#endif /* _INITRAMFS_PREMOUNT_H_ */
