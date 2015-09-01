/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <sys/types.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "join.h"
#include "../mac.h"
#include "../dynstr.h"
#include "../dynarray.h"

int join_str_pair_to (
   char** const restrict out_str,
   const char* const left, const char* const right
) {
   char* buf;

   if ( out_str == NULL ) { return -1; }
   *out_str = NULL;
   buf      = join_str_pair ( left, right );
   if ( buf == NULL ) { return -1; }
   *out_str = buf;
   return 0;
}

int join_str_triple_to (
   char** const restrict out_str,
   const char* const left, const char* const middle, const char* const right
) {
   char* buf;

   if ( out_str == NULL ) { return -1; }
   *out_str = NULL;
   buf      = join_str_triple ( left, middle, right );
   if ( buf == NULL ) { return -1; }
   *out_str = buf;
   return 0;
}


char* join_str_array (
   const char* const seq,
   const size_t arr_len,
   const char* const* const arr
) {
   struct dynstr_data dynstr;
   char* p_str_out;
   size_t seq_len;
   size_t iter;

   /* skip [m]alloc if arr is empty */
   if ( arr_len <= 0 ) { return strdup(""); } /* should be compiled as == 0 */

   if ( dynstr_data_init ( &dynstr, 0 ) != 0 ) { return NULL; }

   if ( (seq == NULL) || (*seq == '\0') ) {
      for ( iter = 0; iter < arr_len; iter++ ) {
         if ( arr[iter] != NULL ) {
            if (
               dynstr_data_append_str_simple ( &dynstr, arr[iter] ) != 0
            ) {
               return NULL;
            }
         }
      }

   } else {
      seq_len = strlen ( seq );

      /* we've already checked for arr_len > 0 */
      for ( iter = 0; iter < arr_len; iter++ ) {
         if ( arr[iter] != NULL ) {
            if (
               dynstr_data_sjoin_append (
                  &dynstr, seq, seq_len, arr[iter]
               ) != 0
            ) {
               return NULL;
            }
         }
      }
   }

   if ( dynstr_data_append_null ( &dynstr ) != 0 ) { return NULL; }
   dynstr_data_unref ( &dynstr, &p_str_out );
   return p_str_out;
}

char* join_str_dynarray (
   const char* const seq,
   const struct dynarray* const p_darr
) {
   return join_str_array (
      seq, p_darr->len, (const char* const*)(p_darr->arr)
   );
}

char* _join_str ( const char* const seq, ... ) {
#define LX__JOIN_STR_APPEND_P_ARG  \
   dynstr_data_append_str ( &dynstr, strlen(p_arg), p_arg, NULL )
#define LX__JOIN_STR_BAIL_OUT   do { va_end ( vargs ); return NULL; } while(0)

   struct dynstr_data dynstr;
   va_list vargs;
   const char* p_arg;
   char* p_str_out;
   size_t seq_len;

   if ( dynstr_data_init ( &dynstr, 0 ) != 0 ) { return NULL; }

   va_start ( vargs, seq );

   if ( (seq == NULL) || (*seq == '\0') ) {
      p_arg = va_arg ( vargs, const char* );

      while ( p_arg != NULL ) {
         if ( LX__JOIN_STR_APPEND_P_ARG != 0 ) {
            LX__JOIN_STR_BAIL_OUT;
         }
         p_arg = va_arg ( vargs, const char* );
      }

   } else {
      seq_len = strlen ( seq );
      p_arg   = va_arg ( vargs, const char* );

      if ( p_arg != NULL ) {
         if ( LX__JOIN_STR_APPEND_P_ARG != 0 ) {
            LX__JOIN_STR_BAIL_OUT;
         }

         p_arg = va_arg ( vargs, const char* );
         while ( p_arg != NULL ) {
            if ( dynstr_data_append_str ( &dynstr, seq_len, seq, NULL ) != 0 ) {
               LX__JOIN_STR_BAIL_OUT;
            }
            if ( LX__JOIN_STR_APPEND_P_ARG != 0 ) {
               LX__JOIN_STR_BAIL_OUT;
            }
            p_arg = va_arg ( vargs, const char* );
         }
      }
   }

   va_end ( vargs );
   if ( dynstr_data_append_char ( &dynstr, '\0' ) != 0 ) { return NULL; }
   dynstr_data_unref ( &dynstr, &p_str_out );
   return p_str_out;

#undef LX__JOIN_STR_BAIL_OUT
#undef LX__JOIN_STR_APPEND_P_ARG
}

static char* _do_join_str_pair (
   const char* const left, const char* const right
);

char* join_str_pair ( const char* const left, const char* const right ) {
   /* try to reduce to single str */
   if ( left == NULL ) {
      if ( right == NULL ) {
         return NULL;
      } else {
         return strdup ( right );
      }

   } else if ( right == NULL ) {
      return strdup ( left );

   } else {
      /* have 2 non-NULL strings */
      return _do_join_str_pair ( left, right );
   }
}

static char* _do_join_str_triple (
   const char* const left, const char* const middle, const char* const right
);

char* join_str_triple (
   const char* const left, const char* const middle, const char* const right
) {
   /* reduce to join_str_pair() if possible */
   if ( left == NULL ) {
      return join_str_pair ( middle, right );

   } else if ( middle == NULL ) {
      return join_str_pair ( left, right );

   } else if ( right == NULL ) {
      return join_str_pair ( left, middle );

   } else {
      /* have 3 non-NULL strings */
      return _do_join_str_triple ( left, middle, right );
   }
}



static char* _do_join_str_pair (
   const char* const left, const char* const right
) {
   size_t size_left;
   size_t size_right;
   size_t iter;
   char* out_str;

   size_left  = strlen ( left );
   size_right = strlen ( right );
   out_str    = malloc ( size_left + size_right + 1 );

   if ( out_str != NULL ) {
      for ( iter = 0; iter < size_left; iter++ ) {
         out_str [iter] = left [iter];
      }

      for ( iter = 0; iter < size_right; iter++ ) {
         out_str [size_left+iter] = right [iter];
      }

      out_str [size_left+size_right] = '\0';
   }

   return out_str;
}

static char* _do_join_str_triple (
   const char* const left, const char* const middle, const char* const right
) {
   size_t size_left;
   size_t size_middle;
   size_t size_right;
   size_t iter;
   size_t offset;
   char* out_str;

   size_left   = strlen ( left );
   size_middle = strlen ( middle );
   size_right  = strlen ( right );
   out_str     = malloc ( size_left + size_right + 1 );

   if ( out_str != NULL ) {
      for ( iter = 0; iter < size_left; iter++ ) {
         out_str [iter] = left [iter];
      }

      offset = size_left;
      for ( iter = 0; iter < size_middle; iter++ ) {
         out_str [offset+iter] = middle [iter];
      }

      offset = size_left + size_middle;
      for ( iter = 0; iter < size_right; iter++ ) {
         out_str [offset+iter] = right [iter];
      }

      out_str [size_left+size_middle+size_right] = '\0';
   }

   return out_str;
}
