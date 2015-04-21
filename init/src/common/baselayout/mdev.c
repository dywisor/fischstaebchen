/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "config.h"
#include "mdev.h"
#include "../message.h"
#include "../fs/baseops.h"
#include "../fs/constants.h"
#include "../fs/fspath.h"
#include "../fs/touch.h"
#include "../fs/fileio.h"
#include "../misc/run_command.h"
#include "../strutil/compare.h"

static int _do_fs_setup_mdev (
   const char* const mdev_dir,
   const char* const mdev_exe
);

int mdev_run_scan (void) {
   return run_command_really_quiet ( MDEV_EXE, MDEV_EXE, "-s" );
}

int setup_mdev (void) {
   int retcode;
   struct fspath_info finfo;

   fspath_get_dirname_basename ( &finfo, MDEV_EXE );
   retcode = _do_fs_setup_mdev ( finfo.dirname, MDEV_EXE );
   fspath_info_free ( &finfo );

   if ( retcode == 0 ) { retcode = mdev_run_scan(); }

   return retcode;
}

static int register_hotplug_agent ( const char* const path ) {
   printf_debug (
      NULL, "Registering hotplug agent: %s", "\n",
      ( STR_IS_EMPTY(path) ? "<disable>" : path )
   );

   return write_sysfs_file (
      GET_PROC_PATH("/sys/kernel/hotplug"),
      ( STR_IS_EMPTY(path) ? "\n" : path )
   );
}

int unregister_mdev (void) {
   return register_hotplug_agent ( NULL );
}


static int _do_fs_setup_mdev (
   const char* const mdev_dir,
   const char* const mdev_exe
) {
   if ( pseudo_touch_file_makedirs ( "/etc/mdev.conf", RW_RO_RO ) != 0 ) {
      /* ignore */
   }

   if ( access ( mdev_exe, F_OK ) != 0 ) {
      if ( makedirs ( mdev_dir ) != 0 ) {
         return -2;
      }

      if ( dosym ( BUSYBOX_EXE, mdev_exe ) != 0 ) {
         return -3;
      }
   }


#if MDEV_SEQ
   write_text_file ( GET_DEVFS_PATH("/mdev.seq"), "\n", O_CREAT, RW_RO );
#endif

#if MDEV_LOG
   pseudo_touch_file ( GET_DEVFS_PATH("/mdev.log"), RW_RO );
#endif

   register_hotplug_agent ( mdev_exe );
   return 0;
}
