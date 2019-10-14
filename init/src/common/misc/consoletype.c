/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "consoletype.h"
#include "env.h"


const char* get_consoletype_str ( int consoletype ) {
   switch (consoletype) {
      case CONSOLE_TYPE_PTY:
         return "pty";
      case CONSOLE_TYPE_SERIAL:
         return "serial";
      case CONSOLE_TYPE_VT:
         return "vt";
      default:
         return NULL;
   }
}

void export_consoletype ( const int consoletype ) {
   const char* desc;

   desc = get_consoletype_str ( consoletype );
   export_env ( "CONSOLETYPE", (desc == NULL ? "" : desc) );
}

static int get_consoletype_from_stat (
   const int fd,
   const struct stat* const stat_info
) {
   const unsigned char twelve = 12;

   unsigned int maj;

   maj = major ( stat_info->st_dev );

   if ( ( maj != 3 ) && ( maj < 136 || maj > 143 ) ) {
      if ( ioctl ( fd, TIOCLINUX, &twelve ) < 0 ) {
         return CONSOLE_TYPE_SERIAL;
      } else {
         return CONSOLE_TYPE_VT;
      }
   } else {
      return CONSOLE_TYPE_PTY;
   }
}

int get_fd_consoletype ( const int fd ) {
   struct stat stat_info;

   if ( fstat ( fd, &stat_info ) == 0 ) {
      return get_consoletype_from_stat ( fd, &stat_info );
   } else {
      return -1;
   }
}

int get_consoletype ( const char* const path ) {
   int fd;
   int consoletype;

   fd = open (
      ( path == NULL ? "/dev/console" : path ), O_WRONLY|O_RDWR
   );

   if ( fd < 0 ) {
      perror ( "failed to open console" );
      return -2;
   }

   consoletype = get_fd_consoletype ( fd );

   if ( close ( fd ) != 0 ) { /* don't care */ }

   return consoletype;
}
