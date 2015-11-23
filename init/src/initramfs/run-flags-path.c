/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "run-flags-path.h"
#include "config.h"
#include "../common/mac.h"
#include "../common/data_types/dynstr_buffer.h"
#include "../common/strutil/join.h"
#include "../common/strutil/compare.h"


struct fspath_prefix_buffer* \
   _fisch_initramfs_run_flags_flagpath_buffer = NULL;


void _fisch_initramfs_run_flags_libfini (void) {
   fspath_prefix_buffer_free (
      &_fisch_initramfs_run_flags_flagpath_buffer
   );
}


const char* ishare_peek_flagfile_path_shared (void) {
   return fspath_prefix_buffer_peek (
      _fisch_initramfs_run_flags_flagpath_buffer
   );
}


const char* ishare_get_flagfile_path_shared (
   const char* const restrict name
) {
   if ( STR_IS_EMPTY(name) ) { return NULL; }

   if ( _fisch_initramfs_run_flags_flagpath_buffer == NULL ) {
      if (
         fspath_prefix_buffer_init (
            &_fisch_initramfs_run_flags_flagpath_buffer,
            (INITRAMFS_RUN_FLAGSDIR "/"),
            B_FALSE /* already suffixed w/ "/" */
         ) != 0
      ) {
         return NULL;
      }
   }

   return fspath_prefix_buffer_set (
      _fisch_initramfs_run_flags_flagpath_buffer,
      name
   );
}


char* ishare_get_flagfile_path ( const char* const name ) {
   return join_str_pair ( ( INITRAMFS_RUN_FLAGSDIR "/" ), name );
}
