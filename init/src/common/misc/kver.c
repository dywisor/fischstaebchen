/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#include "kver.h"

const char* get_localversion ( const char* const kversion_str ) {
   static const char sep = '-';

   const char* split_pos;
   const char* s;
   int is_first;

   if ( kversion_str == NULL ) { return NULL; }

   split_pos = strchr ( kversion_str, sep );
   if ( split_pos == NULL ) { return ""; }

   split_pos++;

   if ( (*split_pos == 'r') && (*(split_pos+1) == 'c') ) {
      /* could be -rc */
      s        = split_pos + 2;
      is_first = 1; /* or use (s == (split_pos + 2)) */

      while (1) {
         switch (*s) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               is_first = 0;
               s++;
               break;

            case '+':
               return (is_first || (*(s+1) != '\0')) ? split_pos : "";

            case '-':
               return is_first ? split_pos : ++s;

            case '\0':
               return is_first ? split_pos : "";

            default:
               return split_pos;
         }
      }
   }

   return split_pos;
}

char* get_kernel_release_from_uname (
   const struct utsname* const p_uinfo
) {
   return (p_uinfo == NULL) ? NULL : strdup ( p_uinfo->release );
}

char* get_kernel_release (void) {
   static struct utsname uinfo;

   /* memleaks? */
   if ( uname ( &uinfo ) != 0 ) { return NULL; }

   return strdup ( uinfo.release );
}
