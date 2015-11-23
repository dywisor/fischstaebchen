/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "dynstr.h"
#include "dynstr_buffer.h"
#include "../mac.h"



static int _fspath_prefix_buffer_init_set_data (
   struct fspath_prefix_buffer* const buf,
   const char* const fspath,
   const int is_dirpath
);



void fspath_prefix_buffer_free_data (
   struct fspath_prefix_buffer* const buf
) {
   if ( buf == NULL ) { return; }
   dynstr_data_free ( &(buf->dynstr) );
   buf->prefix_len_sav = 0;
}


void fspath_prefix_buffer_free (
   struct fspath_prefix_buffer** const p_buf
) {
   if ( p_buf == NULL ) { return; }
   if ( *p_buf != NULL ) {
      fspath_prefix_buffer_free_data ( *p_buf );
      x_free ( *p_buf );
   }
}


int fspath_prefix_buffer_init (
   struct fspath_prefix_buffer** const p_buf,
   const char* const fspath,
   const int is_dirpath
) {
   struct fspath_prefix_buffer* buf;

   *p_buf = NULL;
   buf = malloc ( sizeof *buf );
   if ( buf == NULL ) { return -1; }

   if ( dynstr_data_init ( &(buf->dynstr), 0 ) != 0 ) {
      x_free ( buf );
      return -1;
   }

   if (
      _fspath_prefix_buffer_init_set_data ( buf, fspath, is_dirpath ) != 0
   ) {
      fspath_prefix_buffer_free_data ( buf );
      x_free ( buf );
      return -1;
   }

   *p_buf = buf;
   return 0;
}

int fspath_prefix_buffer_reset (
   struct fspath_prefix_buffer* const buf
) {
   if (
      (buf->prefix_len_sav) > ((buf->dynstr).current_size)
   ) {
      return -1;
   }

   (buf->dynstr).current_size = buf->prefix_len_sav;
   return dynstr_data_append_null ( &(buf->dynstr) );
}


const char* fspath_prefix_buffer_append (
   struct fspath_prefix_buffer* const buf,
   const char* const relpath
) {
   if (
      ( dynstr_data_append_str_simple ( &(buf->dynstr), relpath ) != 0 )
      || ( dynstr_data_append_null ( &(buf->dynstr) ) != 0 )
   ) {
      return NULL;
   }

   return (buf->dynstr).data;
}


const char* fspath_prefix_buffer_set (
   struct fspath_prefix_buffer* const buf,
   const char* const filename
) {
   if ( fspath_prefix_buffer_reset ( buf ) != 0 ) { return NULL; }

   return fspath_prefix_buffer_append ( buf, filename );
}


const char* fspath_prefix_buffer_peek (
   const struct fspath_prefix_buffer* const buf
) {
   return (
      ( buf != NULL )
      && ( ((buf->dynstr).current_size) > (buf->prefix_len_sav) )
   ) ? (buf->dynstr).data : NULL;
}



static int _fspath_prefix_buffer_init_set_data (
   struct fspath_prefix_buffer* const buf,
   const char* const fspath,
   const int is_dirpath
) {
   /* buf->prefix_len_sav = 0; */

   if ( fspath != NULL ) {
      if ( dynstr_data_append_str_simple ( &(buf->dynstr), fspath ) != 0 ) {
         return -1;
      }

      /* append "/" */
      if (
         is_dirpath
         && ( (buf->dynstr).current_size > 0 )
         && (
            (((buf->dynstr).data) [(((buf->dynstr).current_size) - 1)]) != '/'
         )
      ) {
         if ( dynstr_data_append_char ( &(buf->dynstr), '/' ) != 0 ) {
            return -1;
         }
      }
   }


   buf->prefix_len_sav = (buf->dynstr).current_size;
   return dynstr_data_append_null ( &(buf->dynstr) );
}
