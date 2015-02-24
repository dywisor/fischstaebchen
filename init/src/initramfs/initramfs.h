/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_INITRAMFS_H_
#define _INITRAMFS_INITRAMFS_H_

int init_initramfs (void);
int initramfs_run_shell (
   const int retcode_in, const char* const reason, const int suppress_message
);

int initramfs_run_onerror_shell (
   const int retcode_in, const char* const reason
);

int initramfs_run_user_preswitch_shell_if_requested (void);
int initramfs_run_user_shell_if_requested (void);
int initramfs_waitfor_disk_devices (void);
int initramfs_autoswap (void);

int initramfs_mount_newroot_root (void);
int initramfs_mount_newroot_usr (void);
int initramfs_mount_newroot (void);

int default_initramfs_start (void);

int initramfs_switch_root (void);

#endif /* _INITRAMFS_INITRAMFS_H_ */
