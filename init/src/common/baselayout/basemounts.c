/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "basemounts.h"
#include "config.h"
#include "../fs/constants.h"
#include "../fs/mount.h"
#include "../fs/devfs.h"
#include "../fs/baseops.h"
#include "../message.h"
#include "../mac.h"

static int _do_basemounts_all (void);


int basemounts_all (void) {
   FILE* errstream_sav;
   FILE* msgstream_sav;
   int retcode;
   int have_outstreams;

   if (
      (access ( DEVFS_MOUNTPOINT "/stdout", F_OK ) == 0) &&
      (access ( DEVFS_MOUNTPOINT "/stderr", F_OK ) == 0)
   ) {
      have_outstreams = 1;
   } else {
      have_outstreams = devfs_seed ( DEVFS_MOUNTPOINT ) == 0 ? 1 : 0;
   }

   errstream_sav = MSG_ERRSTREAM;
   msgstream_sav = MSG_DEFSTREAM;

   MSG_ERRSTREAM = have_outstreams ? stderr : NULL;
   MSG_DEFSTREAM = have_outstreams ? stdout : NULL;

   retcode = _do_basemounts_all();

   MSG_ERRSTREAM = errstream_sav;
   MSG_DEFSTREAM = msgstream_sav;

   return retcode;
}

int basemounts_mount_and_populate_devfs (void) {
   if ( basemounts_mount_devfs ( DEVFS_MOUNTPOINT ) != 0 ) { return -1; }
   if ( basemounts_mount_devfs_pts ( DEVFS_PTS_MOUNTPOINT ) != 0 ) { return -2; }
   if ( basemounts_mount_devfs_shm ( DEVFS_SHM_MOUNTPOINT ) != 0 ) { return -3; }

   return devfs_seed ( DEVFS_MOUNTPOINT );
}



int basemounts_mount_proc (const char* const mp) {
   return stdmount ( "proc", mp, "proc", PROC_OPTS, PROC_OPTS_STR );
}

int basemounts_mount_sysfs (const char* const mp) {
   return stdmount ( "sysfs", mp, "sysfs", SYSFS_OPTS, SYSFS_OPTS_STR );
}

int basemounts_mount_run (const char* const mp) {
   return stdmount ( "run", mp, "tmpfs", RUN_MOUNT_OPTS, RUN_MOUNT_OPTS_STR );
}

int basemounts_mount_tmp (const char* const mp) {
   return stdmount ( "tmp", mp, "tmpfs", TMP_MOUNT_OPTS, TMP_MOUNT_OPTS_STR );
}


int basemounts_mount_devfs_tmpfs (const char* const mp) {
   return stdmount (
      "devfs", mp, "tmpfs", DEVFS_TMPFS_OPTS, DEVFS_TMPFS_OPTS_STR
   );
}

int basemounts_mount_devfs_devtmpfs (const char* const mp) {
   return stdmount (
      "devtmpfs", mp, "devtmpfs", DEVFS_DEVTMPFS_OPTS, DEVFS_DEVTMPFS_OPTS_STR
   );
}

int basemounts_mount_devfs (const char* const mp) {
#if ENABLE_DEVTMPFS
   if ( basemounts_mount_devfs_devtmpfs ( mp ) == 0 ) { return 0; }
#endif
   return basemounts_mount_devfs_tmpfs ( mp );
}

int basemounts_mount_devfs_shm (const char* const mp) {
   return stdmount (
      "shm", mp, "tmpfs", DEVFS_SHM_OPTS, DEVFS_SHM_OPTS_STR
   );
}

int basemounts_mount_devfs_pts (const char* const mp) {
   return stdmount (
      "devpts", mp, "devpts", DEVFS_PTS_OPTS, DEVFS_PTS_OPTS_STR
   );
}

static int _do_basemounts_all (void) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"

#define BASEMOUNTS__PRINT_MOUNT(mp)  \
   message_fprintf_fmt_str(MSG_DEFSTREAM,fmt_about_to_mount,mp)
#define BASEMOUNTS__PRINT_FAIL(mp)   \
   message_fprintf_fmt_str(MSG_ERRSTREAM,fmt_failed_to_mount,mp)

   unsigned fail;
   char* fmt_about_to_mount;
   char* fmt_failed_to_mount;
   fail = 0;

   fmt_about_to_mount = message_get_color_fmt_str (
      MSG_COLOR_GREEN, ">>>", NULL,
      NULL, "Mounting %s", "\n"
   );

   fmt_failed_to_mount = message_get_color_fmt_str (
      MSG_COLOR_RED, "!!!", NULL,
      MSG_COLOR_RED, "Failed to mount %s!", "\n"
   );

   BASEMOUNTS__PRINT_MOUNT ( PROC_MOUNTPOINT );
   if ( basemounts_mount_proc ( PROC_MOUNTPOINT ) != 0 ) {
      fail |= 0x2;
      BASEMOUNTS__PRINT_FAIL ( PROC_MOUNTPOINT );
   }

   BASEMOUNTS__PRINT_MOUNT ( SYSFS_MOUNTPOINT );
   if ( basemounts_mount_sysfs ( SYSFS_MOUNTPOINT ) != 0 ) {
      fail |= 0x4;
      BASEMOUNTS__PRINT_FAIL ( SYSFS_MOUNTPOINT );
   }

   BASEMOUNTS__PRINT_MOUNT ( DEVFS_MOUNTPOINT );
   if ( basemounts_mount_and_populate_devfs() != 0 ) {
      fail |= 0x8;
      BASEMOUNTS__PRINT_FAIL ( DEVFS_MOUNTPOINT );
   } else {
      MSG_ERRSTREAM = stderr;
      MSG_DEFSTREAM = stdout;
   }

#if BASEMOUNTS_MOUNT_RUN
   BASEMOUNTS__PRINT_MOUNT ( RUN_MOUNTPOINT );
   if ( basemounts_mount_run ( RUN_MOUNTPOINT ) != 0 ) {
      fail |= 0x10;
      BASEMOUNTS__PRINT_FAIL ( RUN_MOUNTPOINT );
   }

   dodir ( (RUN_MOUNTPOINT "/lock") );

#if BASEMOUNTS_SYMLINK_VAR_RUN
   if ( dodir ( "/var" ) == 0 ) {
      dosym ( "../run",      "/var/run" );
      dosym ( "../run/lock", "/var/lock" );
   }
#endif

#endif

#if BASEMOUNTS_MOUNT_TMP
   BASEMOUNTS__PRINT_MOUNT ( TMP_MOUNTPOINT );
   if ( basemounts_mount_tmp ( TMP_MOUNTPOINT ) != 0 ) {
      fail |= 0x40;
      BASEMOUNTS__PRINT_FAIL ( TMP_MOUNTPOINT );
   }
#endif

   x_free ( fmt_about_to_mount );
   x_free ( fmt_failed_to_mount );
   return -(int)fail;

#undef BASEMOUNTS__PRINT_FAIL
#undef BASEMOUNTS__PRINT_MOUNT

#pragma GCC diagnostic pop
}
