/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sysexits.h>

#define  _STR_IS_EMPTY(s) ( ((s) == NULL) || (*(s) == '\0') )

static int process_arg ( const char* const arg ) {
   static const off_t BYTES_TO_MBYTES = 1024 * 1024;

   struct stat sinfo;
   off_t size_m;

   if ( ( stat ( arg, &sinfo ) == 0 ) && ( sinfo.st_size > 0 ) ) {
      size_m = sinfo.st_size / BYTES_TO_MBYTES;

      if ( size_m == 0 ) {
         size_m = 1;
      } else if ( (size_m > 0) && (sinfo.st_size % BYTES_TO_MBYTES) ) {
         size_m++;
      }

      if ( size_m > 0 ) {
         fprintf ( stdout, "%ld\n", (long)size_m );
         return 0;
      }
   }

   return -1;
}

int main ( const int argc, const char* const* const argv ) {
   int argno;

   for ( argno = 1; argno < argc; argno++ ) {
      if ( _STR_IS_EMPTY ( argv [argno] ) ) {
         /* nop */

      } else if ( process_arg ( argv [argno] ) == 0 ) {
         return EXIT_SUCCESS;

      } else {
         return EXIT_FAILURE;
      }
   }

   return EX_USAGE;
}
