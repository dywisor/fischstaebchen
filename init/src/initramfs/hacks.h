/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_HACKS_H_
#define _INITRAMFS_HACKS_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

#include "config.h"
#include "../common/misc/env.h"

#ifndef INITRAMFS_AUX_MNTROOT
#define INITRAMFS_AUX_MNTROOT  "/mnt/aux"
#endif

#ifndef INITRAMFS_FAIL_CONTINUE
#define INITRAMFS_FAIL_CONTINUE  250
#endif


#define IGET_CFGPAIR(name)  \
   GET_INITRAMFS_RUN_CONFDIR(name), GET_INITRAMFS_CONFDIR(name)

int initramfs_init_rundir (void);

int initramfs_mount_newroot_tmpdir (void);
int initramfs_setup_newroot_tmpdir (void);

void initramfs_additional_env_exports (void);

int initramfs_run_hook (
   const char* const phase, const int is_critical, int* const found_hook
);
char* initramfs_get_hook_file ( const char* const phase );

const char* get_any_file_va ( const int mode, va_list vargs );
const char* get_any_file    ( const int mode, ... );

int _premount_essential ( const char* const root, ... );
#define premount_essential(...)   _premount_essential(__VA_ARGS__,NULL)

#define premount_by_name(root,name) \
   _premount_essential(root,IGET_CFGPAIR(name),NULL)

#define initramfs_premount(name)   \
   premount_by_name("/","initramfs-"name".fstab")

#define aux_premount(name)   \
   premount_by_name(INITRAMFS_AUX_MNTROOT,"aux-"name".fstab")

#define newroot_premount(name)   \
   premount_by_name(NEWROOT_MOUNTPOINT,"newroot-"name".fstab")



#define _INITRAMFS_FAIL_LATCH(hdr,action_desc,func_call) \
   do { \
      int __latchret; \
      \
      initramfs_infof ( hdr, "%s", "\n", action_desc ); \
      __latchret = func_call; \
      \
      while ( __latchret != 0 ) { \
         initramfs_errf ( \
            "!!!", "%s returned %d", "\n\n", action_desc, __latchret \
         );  \
         __latchret = initramfs_run_onerror_shell ( __latchret, action_desc ); \
         \
         if ( __latchret == 0 ) { \
            \
         } else if ( __latchret == INITRAMFS_FAIL_CONTINUE ) { \
            break; \
         } else { \
            initramfs_errf ( "BOOT-FAIL", "%s", "\n", action_desc ); \
            return __latchret; \
         } \
         \
         initramfs_infof ( "retry", "%s", "\n", action_desc ); \
         __latchret = func_call; \
         \
         if ( __latchret == 0 ) { \
            initramfs_infof ( \
               "BOOT-RESUME", "%s", "\n", \
               "Resuming boot process - assuming you've fixed the issue..." \
            ); \
         } \
      } \
   } while(0)

#define INITRAMFS_FAIL_LATCH(...)  _INITRAMFS_FAIL_LATCH(">>>",__VA_ARGS__)

#endif /* _INITRAMFS_HACKS_H_ */
