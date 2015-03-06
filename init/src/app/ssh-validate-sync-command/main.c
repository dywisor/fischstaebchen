/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

/*
 * This little program filters any ssh invocation whose command is not
 * "rsync --server", "scp -t" or "mkdir".
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <sys/types.h>

#include "../../common/mac.h"


static int is_whitespace ( const char c ) {
   switch (c) {
      case ' ':
      case '\t':
      case '\n':
      case '\v':
      case '\f':
         return 1;

      default:
         return 0;
   }
}
#define is_whitespace_or_empty(c)  ( (c == '\0') || is_whitespace(c) )

static inline int _check_command_allowed (
   const char* const cmd_str,
   const char* const p_args,
   const size_t prog_name_len
) {
   switch ( prog_name_len ) {
      case 3:
         if ( p_args != NULL ) {
            /* "scp -t" */
            if ( strncmp ( cmd_str, "scp", 3 ) == 0 ) {
               if ( (p_args[0] == '-') && (p_args[1] == 't') ) {
                  return 0;
               }
            }
         }
         break;

      case 5:
         if ( p_args != NULL ) {
            /* "rsync --server" */
            if ( strncmp ( cmd_str, "rsync", 5 ) == 0 ) {
               if ( strncmp ( p_args, "--server", 8 ) == 0 ) {
                  return is_whitespace_or_empty(p_args[8]) ? 0 : -1;
               }
            }

            /* "mkdir" */
            else if ( strncmp ( cmd_str, "mkdir", 5 ) == 0 ) {
               return 0;
            }
         }
         break; /* nop */
   }
   return -1;
}

static int check_command_str ( const char* const cmd_str ) {
   const char* p_chr;
   const char* p_args;
   int is_first_word;
   size_t prog_name_len;

   is_first_word = 1;
   prog_name_len = 0;
   p_args        = NULL;

   for ( p_chr = cmd_str; *p_chr != '\0'; p_chr++ ) {
      switch ( *p_chr ) {
         case '&':
         case '(':
         case ')':
         case '{':
         case '}':
         case ';':
         case '<':
         case '>':
         case '`':
         case '|':
            return 2;

         case ' ':
         case '\t':
         case '\n':
         case '\v':
         case '\f':
            is_first_word = 0;
            break;

         default:
            if ( is_first_word ) {
               prog_name_len++;
            } else if ( p_args == NULL ) {
               p_args = p_chr;
            }
            break; /* nop */
      }
   }

   return _check_command_allowed ( cmd_str, p_args, prog_name_len );
}

int main ( const int UNUSED(argc), const char* const* const UNUSED(argv) ) {
   const char* cmd_str;
   int retcode;

   cmd_str = getenv ( "SSH_ORIGINAL_COMMAND" );
   if ( (cmd_str == NULL) || (*cmd_str == '\0') ) {
      fprintf ( stderr, "No command specified!\n" );
      return EX_USAGE;

   } else if ( check_command_str ( cmd_str ) != 0 ) {
      fprintf ( stderr, "Command rejected: invalid.\n" );
      return 2;

   } else {
      retcode = system ( cmd_str );
      return retcode < 0 ? EX_OSERR : retcode;
   }
}
