/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdarg.h>

#include "mac.h"
#include "message.h"
#include "dynarray.h"

int dynarray_resize (
   struct dynarray* const p_darr, const size_t want_len
) {
   size_t new_size;
   void** p_new;

   if ( p_darr->arr == NULL ) {
      MSG_PRINT_DBG ( "attempted to resize uninitialized dynarray!\n" );
      return -10;
   }

   if ( (want_len < 1) || (want_len <= (p_darr->size)) ) {
      /* nothing to do */
      return 0;
   }

   new_size = 2 * (p_darr->size);
   while ( want_len > new_size ) { new_size *= 2; }

   p_new = realloc ( (p_darr->arr), new_size * (sizeof *(p_darr->arr)) );
   if ( p_new != NULL ) {
      p_darr->arr  = p_new;
      p_darr->size = new_size;

      return 0;
   }

   dynarray_free ( p_darr );
   return -1;
}

int dynarray_grow ( struct dynarray* const p_darr ) {
   return dynarray_resize ( p_darr, (p_darr->len)+1 );
}

int dynarray_init (
   struct dynarray* const p_darr, const size_t initial_size
) {
   size_t real_init_size;

   real_init_size = (
      initial_size > 0 ? initial_size : DYNARRAY_DEFAULT_INITIAL_CAPACITY
   );

   p_darr->arr = malloc ( real_init_size * (sizeof *(p_darr->arr)) );
   if ( p_darr->arr == NULL ) { return -10; }

   p_darr->size     = real_init_size;
   p_darr->len      = 0;
   p_darr->flags    = 0x0;

   return 0;
}

struct dynarray* new_dynarray ( const size_t initial_size ) {
   struct dynarray* p_darr;

   p_darr = malloc ( sizeof *p_darr );
   if ( p_darr == NULL ) { return NULL; }

   if ( dynarray_init ( p_darr, initial_size ) != 0 ) {
      x_free ( p_darr );
      return NULL;
   }

   return p_darr;
}

void dynarray_set_data_readonly (
   struct dynarray* const p_darr, const int status
) {
   if ( status ) {
      p_darr->flags |= (unsigned)DYNARRAY_IS_CONST;
   } else {
      p_darr->flags &= (unsigned)~DYNARRAY_IS_CONST;
   }
}


int dynarray_release ( struct dynarray* const p_darr ) {
   size_t i;
   void** parr;

   if ( p_darr == NULL ) { return 0; }

   parr = p_darr->arr;

   if ( parr != NULL ) {
      if ( (p_darr->flags & DYNARRAY_IS_CONST) == 0 ) {
         /* array items are owned by this data struct (non-const) => free() */
         x_free_arr_items ( parr, p_darr->len );
      } else {
         /* array items are const => set to NULL */
         for ( i = 0; i < p_darr->len; i++ ) {
            parr[i] = NULL;
         }
      }
   }

   p_darr->len = 0;

   return 0;
}

int dynarray_free ( struct dynarray* const p_darr ) {
   if ( p_darr == NULL ) { return 0; }

   if ( dynarray_release ( p_darr ) != 0 ) { return -1; }

   x_free ( p_darr->arr );

   p_darr->size = 0;

   return 0;
}

void dynarray_free_ptr ( struct dynarray** const p_darr ) {
   if ( p_darr == NULL ) { return; }
   dynarray_free ( *p_darr );
   *p_darr = NULL;
}

int dynarray_append ( struct dynarray* const p_darr, void* const data ) {
   if ( dynarray_grow ( p_darr ) != 0 ) { return -1; }
   if ( p_darr->size < 1 ) { return -50; }

   (p_darr->arr) [(p_darr->len)++] = data;

   return 0;
}

static inline int _dynarray_append_copied (
   struct dynarray* p_darr, char* scopy
) {
   if ( scopy == NULL ) {
      return -1;

   } else if ( dynarray_append ( p_darr, scopy ) == 0 ) {
      return 0;

   } else {
      x_free ( scopy );
      return -2;
   }
}

int dynarray_append_strdup (
   struct dynarray* const p_darr, const char* const str
) {
   return _dynarray_append_copied ( p_darr, strdup ( str ) );
}

int dynarray_extend_strdup (
   struct dynarray* const p_darr,
   const int argc, const char* const* const argv
) {
   int k;

   for ( k = 0; k < argc; k++ ) {
      if ( argv[k] != NULL ) {
         if ( dynarray_append_strdup ( p_darr, argv[k] ) != 0 ) { return -1; }
      }
   }

   return 0;
}



int dynarray_append_strnodup (
   struct dynarray* const p_darr, const char* const str
) {
   /* totally unsafe */
   return dynarray_append ( p_darr, (void*) str );
}

int dynarray_extend_strnodup (
   struct dynarray* const p_darr,
   const int argc, const char* const* const argv
) {
   int k;

   for ( k = 0; k < argc; k++ ) {
      if ( argv[k] != NULL ) {
         if ( dynarray_append_strnodup ( p_darr, argv[k] ) != 0 ) { return -1; }
      }
   }

   return 0;
}

int dynarray_pop ( struct dynarray* const p_darr, void** const data_out ) {
   void* pdata;

   if (
      (p_darr == NULL) || (p_darr->arr == NULL) ||
      (p_darr->size == 0) || (p_darr->len == 0)
   ) {
      return -5;
   }

   pdata = p_darr->arr [(p_darr->len)-1];
   p_darr->arr [--(p_darr->len)] = NULL;

   if ( data_out != NULL ) {
      *data_out = pdata;

   } else if ( (p_darr->flags & DYNARRAY_IS_CONST) == 0 ) {
      x_free ( pdata );
   }

   return 0;
}

int va_list_to_dynarray (
   struct dynarray* const p_darr, va_list vargs, const int copy_items
) {
   const char* p_arg;
   /*int (*do_append)(struct dynarray* const p_darr, const char* const item);*/

   p_arg = va_arg ( vargs, const char* );

   if ( copy_items ) {
      while ( p_arg != NULL ) {
         if ( dynarray_append_strdup ( p_darr, p_arg ) != 0 ) { return -1; }
         p_arg = va_arg ( vargs, const char* );
      }
   } else {
      while ( p_arg != NULL ) {
         if ( dynarray_append_strnodup ( p_darr, p_arg ) != 0 ) { return -1; }
         p_arg = va_arg ( vargs, const char* );
      }
   }

   return 0;
}

int _varargs_to_dynarray (
   struct dynarray* const p_darr, const int copy_items, ...
) {
   int retcode;
   va_list vargs;

   va_start ( vargs, copy_items );
   retcode = va_list_to_dynarray ( p_darr, vargs, copy_items );
   va_end ( vargs );

   return retcode;
}
