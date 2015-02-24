/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "untar.h"
#include "../../common/mac.h"

static int is_file ( const char* const path ) {
   struct stat sinfo;

   if ( stat ( path, &sinfo ) != 0 ) { return -1; }

   return ( sinfo.st_mode & S_IFREG ) ? 0 : 1;
}

int main ( int argc, char** argv ) {
#define USAGE_FMT_STR  \
      "%s [-C <dir>] <tarball>"

   static const char* const short_options = "C:h";

   int retcode;
   const char* dst_root_arg;
   const char* tarball_arg;
   char* dst_root;
   char* tarball;
   int opt;

   retcode      = EXIT_FAILURE;
   dst_root     = NULL;
   tarball      = NULL;
   dst_root_arg = NULL;
   tarball_arg  = NULL;

   while ( ( opt = getopt ( argc, argv, short_options ) ) != -1 ) {
      switch (opt) {
         case 'C':
            if ( (optarg == NULL) || (*optarg == '\0') ) {
               dst_root_arg = NULL;
            } else {
               dst_root_arg = optarg;
            }
            break;

         case 'h':
            fprintf ( stdout, "Usage: " USAGE_FMT_STR "\n", argv[0] );
            retcode = EXIT_SUCCESS;
            goto main_exit;

         default:
            retcode = EX_USAGE;
            goto main_exit;
      }
   }

   switch ( argc - optind ) {
      case 1:
         if ( (argv[optind] == NULL) || (*(argv[optind]) == '\0') ) {
            tarball_arg = NULL; /* nop */
         } else {
            tarball_arg = argv[optind];
         }
         break;

      default:
         retcode = EX_USAGE;
         goto main_exit;
   }

   if ( dst_root_arg == NULL ) {
      dst_root = NULL; /* nop */

   } else {
      errno    = 0;
      dst_root = realpath ( dst_root_arg, NULL );

      if ( dst_root == NULL ) {
         dst_root = strdup ( dst_root_arg );
         if ( dst_root == NULL ) { goto main_exit; }
      }
   }

   errno   = 0;
   tarball = realpath ( tarball_arg, NULL );
   if ( tarball == NULL ) {
      fprintf ( stderr, "%s: %s\n", tarball_arg, strerror(errno) );
      goto main_exit;
   }

   if ( is_file ( tarball ) != 0 ) {
      fprintf ( stderr, "not a file: %s\n", tarball );
      goto main_exit;
   }

   retcode = untar ( dst_root, tarball );

main_exit:
   x_free ( dst_root );
   x_free ( tarball  );

   if ( retcode == EX_USAGE ) {
      fprintf ( stderr, "Usage: " USAGE_FMT_STR "\n", argv[0] );
   }
   return retcode;
}
