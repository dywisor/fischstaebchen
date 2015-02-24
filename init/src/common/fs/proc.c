/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include "proc.h"
#include "../config.h"
#include "../mac.h"


static int _proc_misc_parse_line (
   char* line, const size_t line_len,
   struct proc_misc_chardev_info* const p_info
);

static inline void _proc_misc_zap_chardev_info (
   struct proc_misc_chardev_info* p_info
) {
   p_info->name = NULL;
   p_info->min  = 0;
}

static int _proc_get_misc_chardev_minor_callback (
   const struct proc_misc_chardev_info* const p_info_read,
   void* p_info_arg
) {
   if (
      strcmp (
         p_info_read->name,
         ((struct proc_misc_chardev_info*)p_info_arg)->name
      ) == 0
   ) {
      ((struct proc_misc_chardev_info*)p_info_arg)->min = p_info_read->min;
      return 1;
   }

   return 0;
}

int proc_get_misc_chardev_minor_struct (
   struct proc_misc_chardev_info* const p_info
) {
   int retlatch;

   retlatch = proc_misc_chardev_minor_foreach (
      _proc_get_misc_chardev_minor_callback, p_info
   );

   switch (retlatch) {
      case 0:
         return 1;

      case 1:
         return 0;

      default:
         return retlatch;
   }
}

int proc_get_misc_chardev_minor (
   const char* const name,
   unsigned* const p_min
) {
   int retcode;
   struct proc_misc_chardev_info misc;

   misc.name = name;
   misc.min  = 0;
   retcode   = proc_get_misc_chardev_minor_struct ( &misc );

   *p_min = ( retcode == 0 ? misc.min : 0 );
   return retcode;
}

int proc_misc_chardev_minor_foreach (
   proc_misc_chardev_callback callback, void* const data
) {
   int       retcode;
   FILE*     fstream;
   char*     line_buffer;
   size_t    line_buffer_size;
   ssize_t   num_read;
   struct proc_misc_chardev_info misc_info;

   fstream = fopen ( GET_PROC_PATH("/misc"), "r" );
   if ( fstream == NULL ) { return -1; }

   errno            = 0;
   retcode          = 0;
   line_buffer      = NULL;
   line_buffer_size = 0;
   _proc_misc_zap_chardev_info ( &misc_info );

   num_read = getline ( &line_buffer, &line_buffer_size, fstream );
   while ( (num_read >= 0) && (errno == 0) && (retcode == 0) ) {
      if (
         _proc_misc_parse_line ( line_buffer, (size_t)num_read, &misc_info ) == 0
      ) {
         retcode = callback ( &misc_info, data );
      }

      _proc_misc_zap_chardev_info ( &misc_info );

      num_read = getline ( &line_buffer, &line_buffer_size, fstream );
   }

   if (errno) { retcode = -errno; }
   x_free ( line_buffer );
   fclose ( fstream ); /* unchecked retcode */
   return retcode;
}




static inline int _proc_misc_parse_check_is_delimiter ( const char c ) {
   switch (c) {
      case '\n':
      case '\r':
      case  ' ':
         return 0;

      default:
         return -1;
   }
}


static int _proc_misc_parse_line (
   char* line, const size_t line_len,
   struct proc_misc_chardev_info* const p_info
) {
   size_t   iter;
   unsigned min;

   min  = 0;
   iter = 0;

   while (
      (iter < line_len)
      && (_proc_misc_parse_check_is_delimiter ( line[iter] ) == 0)
   ) {
      iter++;
   }

   while (
      (iter < line_len) && (line[iter] > 0x29) && (line[iter] < 0x40)
   ) {
      min = (min * 10) + ((unsigned)line[iter] - 0x30);
      iter++;
   }


   if ( iter >= line_len ) { return -1; }
   if ( _proc_misc_parse_check_is_delimiter ( line[iter] ) != 0 ) { return -1; }

   do {
      line [iter++] = '\0';
   } while (
      (iter < line_len)
      && (_proc_misc_parse_check_is_delimiter ( line[iter] ) == 0)
   );

   if ( iter >= line_len ) { return -1; }

   p_info->name = (line+iter);
   p_info->min  = min;

   /* strip trailing delimiter(s) */
   for ( ; iter < line_len; iter++ ) {
      if ( _proc_misc_parse_check_is_delimiter ( line[iter] ) == 0 ) {
         line[iter] = '\0';
         /* don't care about the remainder */
         break;
      }
   }

   return 0;
}
