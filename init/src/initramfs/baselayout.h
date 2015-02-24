/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_BASELAYOUT_H_
#define _INITRAMFS_BASELAYOUT_H_

int initramfs_baselayout (void);
void initramfs_baselayout_export_default_vars (void);

int initramfs_eject_newroot_basemounts (void);
int initramfs_newroot_basemounts (void);
int initramfs_preswitch_umount (void);

int initramfs_eject_mounts (void);
int initramfs_eject_aux_mounts (void);

#endif /* _INITRAMFS_BASELAYOUT_H_ */
