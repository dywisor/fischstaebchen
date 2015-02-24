/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "mac.h"
#include "dynstr.h"

/**
 * Reallocates a dynstr_data struct and destroys it on error.
 *
 * @param pdata      must not be NULL
 * @param new_size   new size (# of chars)
 *
 * @return non-zero on error
 */
static int _dynstr_resize_free_on_error (
   struct dynstr_data* const pdata, const size_t new_size
) {
   void* p_new;

   if ( pdata->data == NULL ) {
      p_new = malloc ( new_size * (sizeof *(pdata->data)) );
   } else {
      p_new = realloc ( (pdata->data), new_size * (sizeof *(pdata->data)) );
   }

   if ( p_new == NULL ) {
      dynstr_data_free ( pdata );
      return -1;

   } else {
      pdata->data     = p_new;
      pdata->max_size = new_size;

      return 0;
   }
}

int dynstr_data_resize (
   struct dynstr_data* const pdata, const size_t min_size
) {
   size_t new_size;

   if ( min_size < pdata->max_size ) { return 0; }

   new_size = 2 * (pdata->max_size > 0 ? pdata->max_size : 1);
   while ( min_size >= new_size ) { new_size *= 2; }

   return _dynstr_resize_free_on_error ( pdata, new_size );
}

int dynstr_data_grow (
   struct dynstr_data* const pdata, const size_t num_items
) {
   return dynstr_data_resize ( pdata, (pdata->current_size + num_items) );
}

/** Resets a dynstr_data struct
 *
 * @param pdata   must not be NULL
 */
static inline void _dynstr_data_zap_fields (
   struct dynstr_data* const pdata
) {
   pdata->data         = NULL;
   pdata->current_size = 0;
   pdata->max_size     = 0;
}


int dynstr_data_init (
   struct dynstr_data* const pdata, size_t const initial_size
) {
   if ( pdata == NULL ) { return -2; }

   _dynstr_data_zap_fields ( pdata );
   pdata->data = malloc (
      (initial_size > 0 ? initial_size : DEFAULT_DYNSTR_SIZE)
      * (sizeof *(pdata->data))
   );

   if ( pdata->data != NULL ) {
      *(pdata->data) = '\0';
      return 0;
   } else {
      return -1;
   }
}

void dynstr_data_unref (
   struct dynstr_data* const pdata, char** const str_out
) {
   if ( str_out != NULL ) { *str_out = pdata->data; }
   _dynstr_data_zap_fields ( pdata );
}

void dynstr_data_free ( struct dynstr_data* const pdata ) {
   x_free ( pdata->data );
   _dynstr_data_zap_fields ( pdata );
}

/** Appends a char to a dynstr_data object (no reallocation)
 *
 * @param pdata  must not be NULL
 * @param chr    char to append
 */
static inline void _dynstr_data_do_append_char (
   struct dynstr_data* const pdata, const char chr
) {
   (pdata->data) [(pdata->current_size)++] = chr;
}

int dynstr_data_append_str (
   struct dynstr_data* const pdata,
   const size_t str_len, const char* const pstr,
   const char* (*convert_char) (size_t, const char*)
) {
   size_t iter;
   const char* pchr;

   if ( dynstr_data_grow ( pdata, str_len ) != 0 ) { return -1; }

   if ( convert_char == NULL ) {
      for ( iter = 0; iter < str_len; iter++ ) {
         _dynstr_data_do_append_char ( pdata, pstr [iter] );
      }

   } else {
      for ( iter = 0; iter < str_len; iter++ ) {
         pchr = convert_char ( pdata->current_size, (pstr+iter) );
         if ( pchr != NULL ) {
            _dynstr_data_do_append_char ( pdata, *pchr );
         }
      }
   }

   return 0;
}

int dynstr_data_append_str_simple (
   struct dynstr_data* const pdata,
   const char* const pstr
) {
   return dynstr_data_append_str ( pdata, strlen(pstr), pstr, NULL );
}

int dynstr_data_sjoin_append (
   struct dynstr_data* const pdata,
   const char* const join_seq, const size_t join_seq_len,
   const char* const pstr
) {
   if ( pdata->current_size > 0 ) {
      if (
         dynstr_data_append_str ( pdata, join_seq_len, join_seq, NULL ) != 0
      ) {
         return -1;
      }
   }
   return dynstr_data_append_str_simple ( pdata, pstr );
}

int dynstr_data_cjoin_append (
   struct dynstr_data* const pdata,
   const char join_chr,
   const char* const pstr
) {
   if ( pdata->current_size > 0 ) {
      if ( dynstr_data_append_char ( pdata, join_chr ) != 0 ) { return -1; }
   }
   return dynstr_data_append_str_simple ( pdata, pstr );
}

int dynstr_data_append_char (
   struct dynstr_data* const pdata, const char chr
) {
   if ( dynstr_data_grow ( pdata, 1 ) != 0 ) { return -1; }

   _dynstr_data_do_append_char ( pdata, chr );
   return 0;
}

char* dynstr_data_get ( struct dynstr_data* pdata ) {
   return pdata->data;
}

/** Helper function for dynstr_data_rstrip(). */
static int _dynstr_data_strip_replace ( char* const pchr ) {
   switch (*pchr) {
      case 0x00:
         return 1;

      case 0x09: /* horizontal tab */
      case 0x0A: /* line feed */
      case 0x0B: /* vertical tab */
      case 0x0C: /* form feed */
      case 0x0D: /* carriage return */
      case 0x20: /* space */
         *pchr = '\0';
         return 2;

      default:
         if ( *pchr <= 0x0 ) {
            return -1;
         } else if ( (*pchr < 0x21) || (*pchr >= 0x7F ) ) {
            return 3;
         } else {
            return 0;
         }
   }
}

int dynstr_data_rstrip ( struct dynstr_data* const pdata ) {
   int   result;
   char* pstr;

   if ( pdata->data == NULL ) { return -1; }

   pstr = pdata->data;

   while ( pdata->current_size > 0 ) {
      result = _dynstr_data_strip_replace ( (pstr+(pdata->current_size)-1) );
      if ( result == 0 ) {
         break;
      } else if ( result < 0 ) {
         return -1;
      }

      pdata->current_size--;
   }

   return 0;
}

int dynstr_data_append_null ( struct dynstr_data* pdata ) {
   if ( dynstr_data_grow ( pdata, 1 ) != 0 ) { return -1; }

   pdata->data [pdata->current_size] = '\0';
   return 0;
}

int dynstr_data_truncate ( struct dynstr_data* pdata ) {
   pdata->current_size = 0;
   /*if ( dynstr_data_grow ( pdata, 1 ) != 0 ) { return -1; }*/

   *(pdata->data) = '\0';
   return 0;
}
