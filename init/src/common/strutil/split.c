/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include "split.h"
#include "../mac.h"
#include "../data_types/dynarray.h"


static int _do_str_split (
   struct dynarray* const p_darr,
   char* work_str,
   const char* const initial_delimiter,
   const char* const subseq_delimiters,
   const ssize_t max_split,
   ssize_t* const num_fields
);

static inline int _str_split_sanity_checks (
   struct dynarray* const UNUSED(p_darr),
   const char* const str,
   const char* const initial_delimiter,
   const char* const subseq_delimiters,
   const ssize_t max_split
) {
   if ( initial_delimiter != NULL ) {
      if (
         ( (*initial_delimiter) == '\0' )
         || ( (*(initial_delimiter+1)) != '\0' )
      ) {
         errno = EINVAL;
         return -1;
      }
   } else if ( subseq_delimiters == NULL ) {
      errno = EINVAL;
      return -1;
   }

   if ( str == NULL ) {
      errno = EINVAL;
      return -1;
   }

   if ( max_split > 0 ) {
      /* not implemented */
      errno = EINVAL;
      return -1;
   }

   return 0;
}

ssize_t str_split (
   struct dynarray* const p_darr,
   const char* const str,
   const char* const initial_delimiter,
   const char* const subseq_delimiters,
   const ssize_t max_split
) {
   ssize_t num_fields;
   char*   tmp_str;

   if (
      _str_split_sanity_checks (
         p_darr, str, initial_delimiter, subseq_delimiters, max_split
      ) != 0
   ) {
      return -1;
   }

   tmp_str = strdup ( str );
   if ( tmp_str == NULL ) {
      errno = ENOMEM;
      return -1;
   }

   if (
      _do_str_split (
         p_darr, tmp_str,
         initial_delimiter, subseq_delimiters, max_split, &num_fields
      ) != 0
   ) {
      num_fields = -1;
   }

   x_free ( tmp_str );
   return num_fields;
}

ssize_t str_split_unsafe (
   struct dynarray* const p_darr,
   char* const work_str,
   const char* const initial_delimiter,
   const char* const subseq_delimiters,
   const ssize_t max_split
) {
   ssize_t num_fields;

   if (
      _str_split_sanity_checks (
         p_darr, work_str, initial_delimiter, subseq_delimiters, max_split
      ) != 0
   ) {
      return -1;
   }
   if (
      _do_str_split (
         p_darr, work_str,
         initial_delimiter, subseq_delimiters, max_split, &num_fields
      ) != 0
   ) {
      num_fields = -1;
   }

   return num_fields;
}


static int _do_str_split (
   struct dynarray* const p_darr,
   char* work_str,
   const char* const initial_delimiter,
   const char* const subseq_delimiters,
   const ssize_t UNUSED(max_split),
   ssize_t* const num_fields
) {
#define LX__PEDAR_APPEND(s)  \
   do { \
      if ( dynarray_append_strdup(p_darr,s) != 0 ) { return -1; } \
      *num_fields += 1; \
   } while(0)

   char*   saveptr;
   char*   p_substr;
   /*ssize_t num_splits;*/
   /*ignore max_split*/

   *num_fields = 0;

   if ( subseq_delimiters == NULL ) {
      /* initial_delimiter != NULL, by assumption, see str_split() */

      p_substr = strchr ( work_str, (*initial_delimiter) );

      if ( p_substr == NULL ) {
         LX__PEDAR_APPEND ( work_str );

      } else {
         *p_substr = '\0';
         p_substr++;

         LX__PEDAR_APPEND ( work_str );
         if ( *p_substr != '\0' ) { LX__PEDAR_APPEND ( p_substr ); }
      }

   } else {
      /* subseq_delimiters != NULL */

      p_substr = strtok_r (
         work_str,
         ( initial_delimiter == NULL ? subseq_delimiters : initial_delimiter ),
         &saveptr
      );

      while ( p_substr != NULL ) {
         LX__PEDAR_APPEND ( p_substr );
         p_substr = strtok_r ( NULL, subseq_delimiters, &saveptr );
      }
   }

   return 0;
#undef LX__PEDAR_APPEND
}
