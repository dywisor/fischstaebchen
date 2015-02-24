/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <linux/loop.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "loopdev.h"
#include "../config.h"
#include "../mac.h"
#include "../message.h"

static int _loop_control_ioctl ( const unsigned long request );
static int _get_next_free_loopdev_no (void);
static char* _get_next_free_loopdev_path (void);

static int _attach_fd_to_loopdev (
   const char* const filepath,
   const int file_fd,
   const char* const loopdev,
   const uint64_t file_offset,
   const int rw_mode
);

int setup_loop_dev (
   const char* const filepath,
   const uint64_t file_offset,
   const int want_ro,
   char** const loopdev_out
) {
   const int rw_mode = ( want_ro ? O_RDONLY : O_RDWR );

   char* loopdev;
   int   file_fd;
   int   retcode;

   printf_debug (
      NULL, "Trying to find a loop device for %s", "\n", filepath
   );

   *loopdev_out = NULL;

   file_fd = open ( filepath, rw_mode );
   if ( file_fd < 0 ) {
      printf_debug (
         NULL, "failed to open %s (want_ro=%d)", "\n", filepath, want_ro
      );
      return -1;
   }

   loopdev = _get_next_free_loopdev_path();
   if ( loopdev == NULL ) {
      print_debug ( NULL, "no free loop device found!" );
      retcode = -1;
   } else {
      retcode = _attach_fd_to_loopdev (
         filepath, file_fd, loopdev, file_offset, rw_mode
      );

      if ( retcode == 0 ) {
         *loopdev_out = loopdev;
      } else {
         x_free ( loopdev );
      }

      loopdev = NULL;
   }

   close ( file_fd );
   return retcode;
}

static int _attach_fd_to_loopdev (
   const char* const filepath,
   const int file_fd,
   const char* const loopdev,
   const uint64_t file_offset,
   const int rw_mode
) {
   struct loop_info64 linfo;
   int retcode;
   int loopdev_fd;

   memset ( &linfo, 0, sizeof linfo );

   loopdev_fd = open ( loopdev, rw_mode );
   if ( loopdev_fd < 0 ) {
      return -1;
   }

   retcode = -1;

   strncpy ( (char*)(linfo.lo_file_name), filepath, LO_NAME_SIZE );
   linfo.lo_offset = file_offset;

   if ( ioctl ( loopdev_fd, LOOP_SET_FD, file_fd ) == 0 ) {
      if ( ioctl ( loopdev_fd, LOOP_SET_STATUS64, &linfo ) != 0 ) {
         /* err */
      } else {
         /* setup complete */
         retcode = 0;
      }

      if ( retcode != 0 ) {
         ioctl ( loopdev_fd, LOOP_CLR_FD, 0 );
      }
   }


   close ( loopdev_fd );
   return retcode;
}


static int _loop_control_ioctl ( const unsigned long request ) {
   int retcode;
   int fd;

   fd = open ( GET_DEVFS_PATH("/loop-control"), ( O_RDWR | O_CLOEXEC ) );
   if ( fd < 0 ) { return -1; }

   retcode = ioctl ( fd, request );

   close ( fd );
   return retcode;
}

static int _get_next_free_loopdev_no (void) {
   return _loop_control_ioctl ( LOOP_CTL_GET_FREE );
}

static char* _get_next_free_loopdev_path (void) {
   char* devpath;
   int loopno;

   loopno = _get_next_free_loopdev_no();
   printf_debug ( NULL, "next_free_loopdev_no() -> %d", "\n", loopno );
   if ( loopno < 0 ) { return NULL; }

   devpath = NULL;
   if (
      asprintf ( &devpath, "%s%d", GET_DEVFS_PATH("/loop"), loopno ) > 0
   ) {
      printf_debug ( NULL, "next free loopdev: %s", "\n", devpath );
      return devpath;
   }

   x_free ( devpath );
   return NULL;
}
