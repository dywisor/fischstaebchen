/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "baseops.h"
#include "fspath.h"
#include "constants.h"
#include "../mac.h"


static int _check_fs_access_and_type_and_imauser_or_mode (
   const int         access_mode,
   const mode_t      type,
   const mode_t      mode,
   const char* const fspath
) {
   struct stat sb;

   if      ( access ( fspath, access_mode )  != 0 ) { return 1; }
   else if ( stat ( fspath, &sb )            != 0 ) { return 2; }
   else if ( ((sb.st_mode & S_IFMT) & type)  == 0 ) { return 3; }
   else if ( getuid()                        != 0 ) { return 0; }
   else if ( (sb.st_mode & mode)             == 0 ) { return 4; }
   else                                             { return 0; }
}


int check_fs_lexists ( const char* const fspath ) {
   struct stat stat_info;

   return lstat ( fspath, &stat_info );
}

int check_fs_exists ( const char* const fspath ) {
   return access ( fspath, F_OK );
}

int check_fs_executable ( const char* const fspath ) {
   return _check_fs_access_and_type_and_imauser_or_mode (
      X_OK, ~0, X_ALL, fspath
   );
}

int check_fs_readable ( const char* const fspath ) {
   return _check_fs_access_and_type_and_imauser_or_mode (
      R_OK, ~0, R_ALL, fspath
   );
}

int check_fs_writable ( const char* const fspath ) {
   return _check_fs_access_and_type_and_imauser_or_mode (
      W_OK, ~0, W_ALL, fspath
   );
}

int check_fs_exefile ( const char* const fspath ) {
   /* uses stat(), not lstat() */
   return _check_fs_access_and_type_and_imauser_or_mode (
      X_OK, S_IFREG, X_ALL, fspath
   );
}

int check_dir_exists ( const char* const fspath ) {
   struct stat stat_info;

   if ( stat ( fspath, &stat_info ) != 0 ) { return -1; }

   return S_ISDIR(stat_info.st_mode) ? 0 : 1;
}


int dodir_mode ( const char* const fspath, mode_t mode ) {
   if ( check_dir_exists ( fspath ) == 0 ) { return 0; }

   unlink ( fspath );
   errno = 0;

   return mkdir ( fspath, mode );
}

int dodir ( const char* const fspath ) {
   return dodir_mode ( fspath, DEFDIRPERM );
}

static inline int _dodir_callback (
   const char* const fspath,
   const char* const UNUSED(basename),
   void* p_data
) {
   /* if ( p_data == NULL ) { // segfault incoming // } */
   return ( dodir_mode ( fspath, *((mode_t*) p_data) ) == 0 ) ? 0 : -5;
}


int makedirs_mode (
   const char* const fspath,
   mode_t parent_mode, mode_t mode
) {
   if ( check_dir_exists ( fspath ) == 0 ) { return 0; }

   return fspath_downwards_do (
      fspath,
      _dodir_callback, &parent_mode,
      _dodir_callback, &mode
   );
}

int makedirs ( const char* const fspath ) {
   return makedirs_mode ( fspath, DEFDIRPERM, DEFDIRPERM );
}

int makedirs_parents ( const char* const fspath, mode_t parent_mode ) {
   return fspath_downwards_do (
      fspath,
      _dodir_callback, &parent_mode,
      NULL, NULL
   );
}

int unlink_if_exists ( const char* const fspath ) {
   if ( unlink ( fspath ) == 0 ) { return 0; }
   if ( check_fs_lexists ( fspath ) != 0 ) { return 0; }

   return -1;
}

int dosym ( const char* const target, const char* const linkpath ) {
   if ( unlink_if_exists ( linkpath ) != 0 ) { return -2; }
   return symlink ( target, linkpath );
}

static char* _get_parent_path ( const char* const fspath ) {
   char* parent_path;
   char* split_pos;

   parent_path = strdup ( fspath );

   if ( parent_path == NULL ) { return NULL; }

   split_pos = strrchr ( parent_path, '/' );

   if ( (split_pos == NULL) || (*(split_pos+1) == '\0') ) {
      x_free ( parent_path );
      errno = EINVAL;
      return NULL;

   } else if ( split_pos == parent_path ) {
      /* parent path is "/" */
      *(parent_path+1) = '\0';

   } else {
      *split_pos = '\0';
   }

   return parent_path;
}

int is_mounted ( const char* const fspath ) {
   int retcode;
   struct stat path_info;
   struct stat parent_info;
   char* parent_path;

   if ( (fspath == NULL) || (*fspath != '/') ) { return -1; }

   /* rootfs is always assumed to be mounted */
   if ( *(fspath+1) == '\0' ) { return 0; }

   errno = 0;
   if ( stat ( fspath, &path_info ) != 0 ) {
      switch (errno) {
         case ENOENT:
            /* does not exist -> _probably_ is not mounted (or hidden) */
            return 1;

         default:
            return -1;
      }
   }

   parent_path = _get_parent_path ( fspath );
   if ( parent_path == NULL ) { return -1; }

   errno = 0;
   if ( stat ( parent_path, &parent_info ) != 0 ) {
      switch (errno) {
         case ENOENT:
            retcode = 1;
            break;

         default:
            retcode = -1;
            break;
      }

   } else {
      /* same dev ID <=> is not mounted */
      retcode = (
         parent_info.st_dev == path_info.st_dev ? 1 : 0
      );
   }

   x_free ( parent_path );
   return retcode;
}
