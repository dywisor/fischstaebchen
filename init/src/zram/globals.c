/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>  /* snprintf() */
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>

#include "globals.h"
#include "config.h"
#include "../common/mac.h"
#include "../common/strutil/convert.h"
#include "../common/strutil/join.h"
#include "../common/misc/env.h"

struct zram_globals_type* zram_globals = NULL;

static inline void _zram_free_globals_data (
   struct zram_globals_type* const g
) {
   g->base_name = NULL;
   g->base_path = NULL;
}

void zram_free_globals (void) {
   if ( zram_globals != NULL ) {
      _zram_free_globals_data ( zram_globals );
      x_free ( zram_globals );
   }
}

static inline void _zram_globals_set_pagesize (
   struct zram_globals_type* const g
) {
   long psize;

   psize = -1;

#ifdef _SC_PAGESIZE
   if ( psize < 0 ) { sysconf ( _SC_PAGESIZE ); }
#endif
#ifdef PAGESIZE
   if ( psize < 0 ) { psize = sysconf ( PAGESIZE ); }
#endif
#ifdef PAGE_SIZE
   if ( psize < 0 ) { psize = sysconf ( PAGE_SIZE ); }
#endif

   g->system_page_size = (psize > 0) ? (uint64_t)psize : 0;
}


static inline struct zram_globals_type* _zram_init_new_globals (void) {
   struct zram_globals_type* g;

   g = malloc ( sizeof *zram_globals );
   if ( g == NULL ) { return NULL; }

   _zram_globals_set_pagesize ( g );
   g->base_name            = _ZRAM_BASE_NAME;
   g->base_path            = _ZRAM_BASE_PATH;
   g->next_free_identifier = zram_import_next_free_identifier ( NULL );

   return g;
}


int zram_init_globals (void) {
   zram_free_globals();
   zram_globals = _zram_init_new_globals();
   return (zram_globals == NULL) ? -1 : 0;
}


char* get_zram_name ( const char* const ident_str ) {
   return join_str_pair ( zram_globals->base_name, ident_str );
}

char* get_zram_path ( const char* const ident_str ) {
   return join_str_pair ( zram_globals->base_path, ident_str );
}

char* get_zram_devpath ( const char* const ident_str ) {
   return join_str_pair ( _ZRAM_BASE_DEVPATH, ident_str );
}

static int _fill_zram_name_path_triple (
   char** const arr, const char* const ident_str
) {
   arr[0] = get_zram_name    ( ident_str );
   if ( arr[0] == NULL ) { return -1; }

   arr[1] = get_zram_path    ( ident_str );
   if ( arr[1] == NULL ) { return -1; }

   arr[2] = get_zram_devpath ( ident_str );
   if ( arr[2] == NULL ) { return -1; }

   return 0;
}

char* get_zram_ident_str ( const uint8_t identifier ) {
   return uint8_to_str ( identifier );
}

static inline void _get_zram_name_path_triple__nullset (
   char** const a, const int n
) {
   int k;
   for ( k = 0; k < n; k++ ) { a[k] = NULL; }
}

int get_zram_name_path_triple (
   const uint8_t identifier,
   char** const data_out
) {
   int   retcode;
   char* ident_str;

   _get_zram_name_path_triple__nullset ( data_out, 3 );

   retcode   = -1;
   ident_str = get_zram_ident_str ( identifier );

   if ( ident_str != NULL ) {
      if ( _fill_zram_name_path_triple ( data_out, ident_str ) != 0 ) {
         x_free_arr_items ( data_out, 3 );
      } else {
         retcode = 0;
      }

      x_free ( ident_str );
   }

   return retcode;
}

void zram_export_next_free_identifier ( const char* const varname ) {
   char* istr;

   istr = get_zram_ident_str ( zram_globals->next_free_identifier );

   export_env (
      ( varname == NULL ? ZRAM_DEFAULT_IDENTIFIER_ENVVAR : varname ),
      istr /* may be NULL */
   );

   x_free ( istr );
}

uint8_t zram_import_next_free_identifier ( const char* const varname ) {
   long val;
   const char* istr;

   istr = getenv (
      ( varname == NULL ? ZRAM_DEFAULT_IDENTIFIER_ENVVAR : varname )
   );

   if ( istr != NULL ) {
      if ( str_to_long ( istr, &val ) == 0 ) {
         if ( (val >= 0) && (val <= 255) ) {
            return (uint8_t)val;
         }
      }
   }

   return 0;
}
