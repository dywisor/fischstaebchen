/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>

#include "compare.h"



int char_means_true ( const char chr ) {
   switch (chr) {
      case '0':
      case 'n':
      case 'N':
      case '-':
         return 1;

      case '1':
      case 'y':
      case 'Y':
      case '+':
         return 0;

      default:
         return -1;
   }
}

int str_means_true ( const char* const str ) {
   if ( (str == NULL) || (*str == '\0') ) {
      return -1;

   } else if ( *(str+1) == '\0' ) {
      return char_means_true ( *str );

   } else if ( strcaseeq_any ( str, "true", "yes" ) == 0 ) {
      return 0;

   } else if ( strcaseeq_any ( str, "false", "no" ) == 0 ) {
      return 1;

   } else {
      return -1;
   }
}

/**
 * Helper function for str*_any()/str*_switch()
 *
 * @param fcompare        str compare function
 * @param compare_result  fcompare() return value that should be interpreted
 *                        as "candidate matches str"
 * @param str             str to match
 * @param vargs           NULL-terminated list of candidates
 *
 * @return index of the first candidate that matches str, or -1
 **/
static int _get_vargs_strmatch (
   int (*fcompare)(const char*,const char*),
   const int compare_result,
   const char* const str,
   va_list vargs
) {
   int k;
   const char* p_arg;

   k = 0;

   p_arg = va_arg ( vargs, const char* );

   while ( p_arg != NULL ) {
      if ( fcompare ( str, p_arg ) == compare_result ) {
         return k;
      }
      p_arg = va_arg ( vargs, const char* );
      k++;
   }

   return -1;
}

int _streq_any ( const char* const str, ... ) {
   int result;
   va_list vargs;

   va_start ( vargs, str );
   result = _get_vargs_strmatch ( strcmp, 0, str, vargs );
   va_end ( vargs );

   return ( result < 0 ) ? -1 : 0;
}

int _strcaseeq_any ( const char* const str, ... ) {
   int result;
   va_list vargs;

   va_start ( vargs, str );
   result = _get_vargs_strmatch ( strcasecmp, 0, str, vargs );
   va_end ( vargs );

   return ( result < 0 ) ? -1 : 0;
}

int _streq_switch ( const char* const str, ... ) {
   int result;
   va_list vargs;

   va_start ( vargs, str );
   result = _get_vargs_strmatch ( strcmp, 0, str, vargs );
   va_end ( vargs );

   return result;
}

int _strcaseeq_switch ( const char* const str, ... ) {
   int result;
   va_list vargs;

   va_start ( vargs, str );
   result = _get_vargs_strmatch ( strcasecmp, 0, str, vargs );
   va_end ( vargs );

   return result;
}

const char* str_startswith (
   const char* const str, const char* const prefix
) {
   size_t prefix_len = strlen ( prefix );

   if ( strncmp ( str, prefix, prefix_len ) == 0 ) {
      return ( str + prefix_len );
   } else {
      return NULL;
   }
}

const char* str_casestartswith (
   const char* const str, const char* const prefix
) {
   size_t prefix_len = strlen ( prefix );

   if ( strncasecmp ( str, prefix, prefix_len ) == 0 ) {
      return ( str + prefix_len );
   } else {
      return NULL;
   }
}
