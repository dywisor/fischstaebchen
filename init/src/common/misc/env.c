/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>

#include "env.h"
#include "../config.h"
#include "../mac.h"
#include "../strutil/compare.h"
#include "../strutil/join.h"

static int do_export_env (
   const char* const varname, const char* const value, const int overwrite
) {
   if ( value == NULL ) {
      return unsetenv ( varname );
   } else {
      return setenv ( varname, value, overwrite );
   }
}

int export_env_if_not_set (
   const char* const varname, const char* const value
) {
   return do_export_env ( varname, value, 0 );
}

int export_env ( const char* const varname, const char* const value ) {
   return do_export_env ( varname, value, 1 );
}

static int _add_or_append_to_path (
   const int want_append, const char* const dirpath
) {
   int   retcode;
   char* old_path;
   char* new_path;

   old_path = getenv ( "PATH" );
   if ( STR_IS_EMPTY ( old_path ) ) {
      retcode = export_env ( "PATH", dirpath );

   } else {
      if ( want_append ) {
         new_path = join_str_triple ( old_path, ":", dirpath );
      } else {
         new_path = join_str_triple ( dirpath, ":", old_path );
      }

      if ( new_path == NULL ) {
         retcode = -1;
      } else {
         retcode = export_env ( "PATH", new_path );
         x_free ( new_path );
      }
   }

   return retcode;
}

int append_to_path ( const char* const dirpath ) {
   return _add_or_append_to_path ( 1, dirpath );
}

int add_to_path ( const char* const dirpath ) {
   return _add_or_append_to_path ( 0, dirpath );
}

int export_env_shbool ( const char* const varname, const int boolval ) {
   return export_env (
      varname, (boolval ? SHELL_STR_TRUE : SHELL_STR_FALSE)
   );
}
