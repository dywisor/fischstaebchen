/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "fspath.h"
#include "../mac.h"


static void _fspath_info_zap ( struct fspath_info* const p_info ) {
   p_info->_path    = NULL;
   p_info->basename = NULL;
   p_info->dirname  = NULL;
}


void fspath_info_init ( struct fspath_info* const p_info, char* const path ) {
   _fspath_info_zap ( p_info );
   p_info->_path    = path;
}

void fspath_info_free ( struct fspath_info* const p_info ) {
   x_free ( p_info->_path );
   _fspath_info_zap ( p_info );
}


int fspath_get_dirname_basename (
   struct fspath_info* const p_info, const char* const path
) {
   char* p_split;

   fspath_info_init ( p_info, fspath_strip ( path ) );

   if ( p_info->_path == NULL ) {
      if ( path == NULL ) {
         return -1;
      } else {
         p_info->basename = "";
         p_info->dirname  = ".";
         return 0;
      }
   }

   p_split = strrchr ( p_info->_path, '/' );
   if ( p_split == NULL ) {
      p_info->basename = p_info->_path;
      p_info->dirname  = ".";

   } else if ( p_split == (p_info->_path) ) {
      /* assert (p_info->_path)[1] == '\0' */
      p_info->basename = p_info->_path;
      p_info->dirname  = p_info->_path;

   } else {
      *p_split = '\0';
      p_info->basename = (p_split + 1);
      p_info->dirname  = p_info->_path;
   }

   p_split = NULL; /* not necessary */
   return 0;
}



int fspath_downwards_do (
   const char* const path,
   fspath_callback_function parent_callback,
   void* parent_callback_data,
   fspath_callback_function fspath_callback,
   void* fspath_callback_data
) {
   static const char DELIMITER = '/';

   char*       buf;
   int         retcode;
   char*       p_buf_delim;
   const char* p_buf_basename;

   /* could be done in one iteration */


   /* loop #1: copy/preparse path */
   if ( (path == NULL) || (*path == '\0') ) { return 0; }
   buf = fspath_strip ( path );
   if ( buf == NULL ) { return -1; }


   /* loop #2: parent_callback() (intermediate paths) */
   p_buf_delim = buf;
   if ( *p_buf_delim == DELIMITER ) { p_buf_delim++; }

   retcode        = 0;
   p_buf_basename = p_buf_delim;
   p_buf_delim    = strchr ( p_buf_basename, DELIMITER );

   while ( (p_buf_delim != NULL) && (retcode == 0) ) {
      *p_buf_delim  = '\0';
      if ( parent_callback != NULL ) {
         retcode = parent_callback (
            buf, p_buf_basename, parent_callback_data
         );
      }
      *p_buf_delim  = DELIMITER;

      p_buf_basename = p_buf_delim + 1;
      p_buf_delim    = strchr ( p_buf_basename, DELIMITER );
   }

   /* fspath_callback() */
   if ( retcode == 0 ) {
      if ( fspath_callback != NULL ) {
         retcode = fspath_callback (
            buf, p_buf_basename, fspath_callback_data
         );
      }
   }

   p_buf_delim    = NULL; /* not necessary */
   p_buf_basename = NULL; /* not necessary */

   return retcode;
}

char* fspath_strip ( const char* const path ) {
   static const char DELIMITER = '/';
   const size_t path_len = ( path == NULL ? 0 : strlen ( path ) );

   char*       buf;
   const char* p_path_iter;
   char*       p_buf;

   if ( path_len < 1 ) { return NULL; }

   buf = malloc ( path_len + 1 );
   if ( buf == NULL ) { return NULL; }

   p_buf       = buf;
   p_path_iter = path;
   while ( *p_path_iter != '\0' ) {
      if ( *p_path_iter == DELIMITER ) {
         do { p_path_iter++; }  while ( *p_path_iter == DELIMITER );
         if ( *p_path_iter != '\0' ) { *(p_buf++) = DELIMITER; }
      } else {
         *(p_buf++) = *(p_path_iter++);
      }
   }
   *p_buf = '\0';

   if ( (*buf == '\0') && (*path == DELIMITER) ) {
      /* restore "/" */
      buf [0] = DELIMITER;
      buf [1] = '\0';
   }

   p_path_iter = NULL;
   p_buf       = NULL;

   return buf;
}
