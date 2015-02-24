/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#include "misc.h"


const char* str_seek ( const char* const str, const ssize_t offset ) {
   ssize_t slen;
   ssize_t k;

   if ( str == NULL ) { return NULL; }

   /* verify that str "+" offset is part of the string */

   slen = (ssize_t)(strlen ( str ) + 1); /* include '\0' */

   if ( offset < 0 ) {
      k = slen + offset;
      return ( k >= 0 ) ? ( str + k ) : NULL;

   } else {
      return ( offset < slen ) ? ( str + offset ) : NULL;
   }
}
