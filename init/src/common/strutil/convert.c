/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h>

#include "convert.h"
#include "format.h"
#include "../mac.h"

static inline void _strutil_convert_to_anycase (
   int (*fconvert)(int), char* const str
);

static inline char* _strutil_get_anycase (
   int (*fconvert)(int), const char* const str
);

int str_to_long_base ( int base, const char* const str, long* const lout ) {
   char* endptr;

   if ( (str != NULL) && (*str != '\0') ) {
      endptr = NULL;
      *lout  = strtol ( str, &endptr, base );

      return ( (endptr == NULL) || (*endptr != '\0') ) ? 1 : 0;

   } else {
      *lout = 0;
      return -1;
   }
}

int str_to_long ( const char* const str, long* const lout ) {
   return str_to_long_base ( 0, str, lout );
}

void convert_to_uppercase ( char* const str ) {
   _strutil_convert_to_anycase ( toupper, str );
}

void convert_to_lowercase ( char* const str ) {
   _strutil_convert_to_anycase ( tolower, str );
}

char* get_uppercase ( const char* const str ) {
   return _strutil_get_anycase ( toupper, str );
}

char* get_lowercase ( const char* const str ) {
   return _strutil_get_anycase ( tolower, str );
}




static inline void _strutil_convert_to_anycase (
   int (*fconvert)(int), char* const str
) {
   char* p_chr;

   if ( str != NULL ) {
      for ( p_chr = str; (*p_chr) != '\0'; p_chr++ ) {
         *p_chr = (char) fconvert ( *p_chr );
      }
   }
}

static inline char* _strutil_get_anycase (
   int (*fconvert)(int), const char* const str
) {
   char*  outstr;
   size_t slen;
   size_t iter;

   if ( str == NULL ) { return NULL; }

   slen = strlen ( str );

   outstr = malloc ( slen + 1 );
   if ( outstr == NULL ) { return NULL; }

   for ( iter = 0; iter < slen; iter++ ) {
      outstr [iter] = (char) fconvert ( str [iter] );
   }

   outstr [slen] = '\0';

   return outstr;
}

char* uint8_to_str ( const uint8_t k ) {
#define BUFSIZE 4  /* 3 digits  + '\0' */
   char* str;

   str = malloc ( BUFSIZE );
   if ( str != NULL ) {
      if ( str_format_check_fail ( str, BUFSIZE, "%" PRIu8, k ) ) {
         x_free ( str );
      }
   }

   return str;
#undef BUFSIZE
}
