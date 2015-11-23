/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


#include "run-flags.h"
#include "run-flags-path.h"
#include "../common/mac.h"
#include "../common/decision_bool.h"

#include "../common/strutil/compare.h"
#include "../common/fs/constants.h"
#include "../common/fs/baseops.h"
#include "../common/fs/touch.h"



DBOOL_TYPE ishare_check_flag ( const char* const name ) {
   const char* flagfile_path;

   flagfile_path = ishare_get_flagfile_path_shared ( name );

   if ( flagfile_path == NULL ) {
      return DBOOL_UNDECIDED;

   } else if ( access ( flagfile_path, F_OK ) == 0 ) {
      return DBOOL_TRUE;

   } else {
      return DBOOL_FALSE;
   }
}


int ishare_has_flag ( const char* const restrict name ) {
   return dbool_is_true ( ishare_check_flag ( name ) );
}


int ishare_has_not_flag ( const char* const restrict name ) {
   return dbool_is_false ( ishare_check_flag ( name ) );
}


int ishare_add_flag ( const char* const restrict name ) {
   const char* flagfile_path;

   flagfile_path = ishare_get_flagfile_path_shared ( name );

   if ( flagfile_path == NULL ) {
      return -1;
   } else {
      return pseudo_touch_file_makedirs ( flagfile_path, RW_RW );
   }
}


int _ishare_del_flag ( const char* const restrict name ) {
   const char* flagfile_path;

   flagfile_path = ishare_get_flagfile_path_shared ( name );

   if ( flagfile_path == NULL ) {
      return -1;
   } else {
      return unlink_if_exists ( flagfile_path );
   }
}


int ishare_set_flag ( const char* const restrict name, const int bval ) {
   if ( bval == 0 ) {
      return _ishare_del_flag ( name );
   } else {
      return ishare_add_flag ( name );
   }
}


int ishare_set_flag_from_str (
   const char* const restrict name,
   const char* const restrict str_value
) {
   /* undecided (str_means_true(_) < 0) means false here */
   return ishare_set_flag ( name, (0 == str_means_true(str_value)) );
}


int ishare_set_flag_from_char (
   const char* const restrict name,
   const char chr_value
) {
   return ishare_set_flag ( name, (0 == char_means_true(chr_value)) );
}
