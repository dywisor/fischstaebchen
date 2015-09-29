/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_DEVFS_H_
#define _INITRAMFS_DEVFS_H_

/*
int initramfs_scan_lvm (void);
int initramfs_scan_mdadm ( const char* const uuid );
int initramfs_scan_bcache (void);
*/

int initramfs_wait_for_disk (
   const unsigned timeout_decisecs, const char* const disk
);

#endif /* _INITRAMFS_DEVFS_H_ */
