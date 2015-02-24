/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "findfs.h"
#include "../config.h"
#include "../strutil/compare.h"
#include "../message.h"
#include "../mac.h"

static char* _read_first_line_from_pipe ( int* const fd );


int findfs ( const char* const disk_spec, char** const dev_out ) {
   pid_t pid;
   int retcode;
   int status;
   int comlink[2];
   char* dev;

   if ( dev_out != NULL ) { *dev_out = NULL; }
   if ( STR_IS_EMPTY ( disk_spec ) ) { return -1; }
   if ( pipe ( comlink ) != 0 ) { return -1; }

   printf_debug ( "(findfs)", "Trying to find '%s'", "\n", disk_spec );

   retcode = -1;
   pid     = fork();

   switch ( pid ) {
      case -1:
         return -1;

      case 0:
         /* child - close read end */
         IGNORE_RETCODE ( close ( comlink[0] ) );

         /* redirect stdout to write end */
         if ( dup2 ( comlink[1], STDOUT_FILENO ) < 0 ) {
            IGNORE_RETCODE ( close ( comlink[1] ) );
            _exit ( EXIT_FAILURE );
         }

         /* redirect stderr, stdin */
         IGNORE_RETCODE ( freopen ( DEVNULL_FSPATH, "r", stdin ) );
         IGNORE_RETCODE ( freopen ( DEVNULL_FSPATH, "a", stderr ) );

         execlp ( FINDFS_PROGV, disk_spec, NULL );

         _exit ( EXIT_FAILURE );

      default:
         /* parent - close write end */
         IGNORE_RETCODE ( close ( comlink[1] ) );

         if ( waitpid ( pid, &status, 0 ) >= 0 ) {
            retcode = WEXITSTATUS ( status );

            if ( retcode == 0 ) {
               /* read dev from pipe */
               dev = _read_first_line_from_pipe ( &(comlink[0]) );

               if ( STR_IS_EMPTY ( dev ) ) {
                  printf_warning (
                     "(findfs)", "%s", "\n",
                     "findfs succeeded, but did not output the device to stdout"
                  );

                  x_free ( dev );
                  retcode = -1;
               } else {
                  printf_debug ( "(findfs)", "found %s", "\n", dev );

                  if (dev_out != NULL) {
                     *dev_out = dev;
                  } else {
                     x_free ( dev );
                  }
               }
               dev = NULL;
            }
         }

         if ( comlink[0] >= 0 ) {
            IGNORE_RETCODE ( close ( comlink[0] ) );
         }
         break; /* nop */
   }


   if ( retcode != 0 ) {
      printf_debug (
         "(findfs)", "failed to resolve '%s'", "\n", disk_spec
      );
   }
   return retcode;
}

static void _process_line ( char* const line ) {
   char* iter;

   for ( iter = line; *iter != '\0'; iter++ ) {
      switch (*iter) {
         case '\n':
         case '\r':
            do {
               *(iter++) = '\0';
            } while ( iter != '\0' );

            return;

      }
   }
}

static char* _read_first_line_from_pipe ( int* const fd ) {
   FILE*    instream;
   char*    line_buf;
   size_t   line_buf_size;
   ssize_t  num_read;


   instream = fdopen ( *fd, "r" );
   if ( instream == NULL ) { return NULL; }

   line_buf       = NULL;
   line_buf_size  = 0;
   num_read       = getline ( &line_buf, &line_buf_size, instream );

   if ( num_read > 0 ) {
      _process_line ( line_buf );
   } else {
      x_free ( line_buf );
   }

   *fd = -2;
   fclose ( instream );
   return line_buf;
}
