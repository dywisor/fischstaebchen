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

#define _IS_SHORTOPT(s)  \
   ((s) != NULL) && ((s)[0] == '-') && ((s)[1] != '\0') && ((s)[2] == '\0')

static int check_stat ( const mode_t want_mode, const char* const path );

static int _filescan (
   const mode_t want_filemode,
   const int argc,
   const char* const* const argv,
   const int basepath_start_idx,
   const int basepath_end_idx, /* exclusive */
   const int suffix_start_idx
);


int main ( const int argc, const char* const* const argv ) {
   mode_t want_filemode;
   int argshift;
   int argsplit;

   want_filemode = 0;
   argshift      = 1;
   argsplit      = 1;

   while (
      (argc > argshift) && (argsplit) && _IS_SHORTOPT(argv[argshift])
   ) {
      switch ( argv[argshift][1] ) {
         case 'e':
            want_filemode = S_IFMT;
            argshift++;
            break;

         case 'f':
            want_filemode |= S_IFREG;
            argshift++;
            break;

         case 'd':
            want_filemode |= S_IFDIR;
            argshift++;
            break;

/*
         case 'h': // has no effect due to use of stat() and not lstat()
            want_filemode |= S_IFLNK;
            argshift++;
            break;
*/

         case 'b':
            want_filemode |= S_IFBLK;
            argshift++;
            break;

         case 'c':
            want_filemode |= S_IFCHR;
            argshift++;
            break;

         case '-':
            argshift++;
            argsplit = 0;
            break;

         default:
            argsplit = 0;
            break; /* nop */
      }
   }
   if ( want_filemode == 0 ) { want_filemode = S_IFREG; }


   if ( (argc - argshift) < 2 ) { return EX_USAGE; }

   for (
      argsplit = argshift;
      (
         (argsplit < argc) &&
         ! ( _IS_SHORTOPT(argv [argsplit]) && argv[argsplit][1] == '-' )
      );
      argsplit++
   ) {}


   if ( argsplit >= argc ) {
      return _filescan (
         want_filemode, argc, argv, argshift, (argshift + 1), (argshift + 1)
      );
   } else {
      return _filescan (
         want_filemode, argc, argv, argshift, argsplit, (argsplit + 1)
      );
   }
}


static int check_stat ( const mode_t want_mode, const char* const path ) {
   struct stat sinfo;

   if ( _STR_IS_EMPTY(path) ) { return -1; }
   if ( stat ( path, &sinfo ) != 0 ) { return -1; }

   return ( sinfo.st_mode & want_mode ) ? 0 : 1;
}

static int _filescan (
   const mode_t want_filemode,
   const int argc,
   const char* const* const argv,
   const int basepath_start_idx,
   const int basepath_end_idx, /* exclusive */
   const int suffix_start_idx
) {
   const char* basepath;
   const char* suffix;
   char* fspath;
   int suffix_idx;
   int basepath_idx;


   if (
         (basepath_start_idx >= argc)
      || (basepath_end_idx >= argc)
      || (basepath_start_idx >= basepath_end_idx)
      || (suffix_start_idx >= argc)
      || (want_filemode == 0)
   ) {
      return EX_USAGE;
   }

   fspath = NULL; /* not necessary */
   for (
      basepath_idx = basepath_start_idx;
      basepath_idx < basepath_end_idx;
      basepath_idx++
   ) {
      basepath = argv [basepath_idx];

      for (
         suffix_idx = suffix_start_idx;
         suffix_idx < argc;
         suffix_idx++
      ) {
         suffix = argv [suffix_idx];

         if (
               _STR_IS_EMPTY(suffix)
            || ( (*suffix == '.') && (*(suffix+1) == '\0') )
         ) {
            if ( check_stat ( want_filemode, basepath ) == 0 ) {
               fprintf ( stdout, "%s\n", basepath );
               return EXIT_SUCCESS;
            }

         } else if ( asprintf ( &fspath, "%s%s", basepath, suffix ) < 0 ) {
            return EX_SOFTWARE;

         } else if ( check_stat ( want_filemode, fspath ) == 0 ) {
            fprintf ( stdout, "%s\n", fspath );
            free ( fspath );
            /*fspath = NULL;*/
            return EXIT_SUCCESS;

         } else {
            free ( fspath );
            fspath = NULL;
         }
      }
   }

   return EXIT_FAILURE;
}
