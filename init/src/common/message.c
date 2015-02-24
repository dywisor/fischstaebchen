/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "mac.h"
#include "message.h"
#include "strutil/join.h"

FILE* MSG_DBGSTREAM         = NULL;
FILE* MSG_DEFSTREAM         = NULL;
FILE* MSG_ERRSTREAM         = NULL;
FILE* MSG_WARNSTREAM        = NULL;

const char* MSG_COLOR__DBG  = NULL;
const char* MSG_COLOR__DEF  = NULL;
const char* MSG_COLOR__ERR  = NULL;
const char* MSG_COLOR__WARN = NULL;

const char* MSG_PREFIX      = NULL;

int MSG_NOCOLOR             = 1;
int MSG_INDENT_LEVEL        = 0;



#define _MSG_PRINTF_FUNC_BODY(ref) \
   do { \
      int esav; \
      va_list vargs; \
      \
      esav = errno; \
      va_start ( vargs, fmt_end ); \
      message_vfprintf_color ( \
         MSG_ ## ref ## STREAM, \
         MSG_COLOR__ ## ref, \
         ( header != NULL ? header : MSG_PREFIX ), \
         header, \
         NULL, \
         fmt, \
         fmt_end, \
         vargs \
      ); \
      va_end ( vargs ); \
      errno = esav; \
   } while(0)


void printf_debug (
   const char* const header,
   const char* const fmt, const char* const fmt_end, ...
) {
   _MSG_PRINTF_FUNC_BODY ( DBG );
}

void printf_message (
   const char* const header,
   const char* const fmt, const char* const fmt_end, ...
) {
   _MSG_PRINTF_FUNC_BODY ( DEF );
}

void printf_error (
   const char* const header,
   const char* const fmt, const char* const fmt_end, ...
) {
   _MSG_PRINTF_FUNC_BODY ( ERR );
}

void printf_warning (
   const char* const header,
   const char* const fmt, const char* const fmt_end, ...
) {
   _MSG_PRINTF_FUNC_BODY ( WARN );
}

#undef _MSG_PRINTF_FUNC_BODY


void print_debug ( const char* const header, const char* const msg ) {
   printf_debug ( header, "%s", "\n", msg );
}

void print_message ( const char* const header, const char* const msg ) {
   printf_message ( header, "%s", "\n", msg );
}

void print_error ( const char* const header, const char* const msg ) {
   printf_error ( header, "%s", "\n", msg );
}

void print_warning ( const char* const header, const char* const msg ) {
   printf_warning ( header, "%s", "\n", msg );
}

char* message_get_color_fmt_str (
   const char* const header_color,
   const char* const colored_header_text,
   const char* const nocolor_header_text,
   const char* const fmt_color,
   const char* const fmt,
   const char* const fmt_end
) {
   const int num_indent = (MSG_INDENT_LEVEL > 0) ? (MSG_INDENT_LEVEL*3) : 0;
   char  indent [num_indent+1];
   int   k;
   char* ret_fmt_str;
   const char* hdr;
   const char* hdr_str;
   const char* msg_fmt_str;


   for ( k = 0; k < num_indent; k++ ) { indent[k] = ' '; }
   indent [num_indent] = '\0';

   hdr = (
      (MSG_NOCOLOR && (nocolor_header_text != NULL)) ? \
         nocolor_header_text : colored_header_text
   );

   msg_fmt_str = (fmt == NULL) ? "" : fmt;

   if ( (hdr == NULL) || (*hdr == '\0') ) {
      hdr     = NULL;
      hdr_str = "";
      if ( fmt == NULL ) { indent[0] = '\0'; }

   } else {
      hdr_str = hdr;
   }


   if ( MSG_NOCOLOR ) {
      ret_fmt_str = join_str (
         indent,
         hdr_str,
         ( ( (hdr == NULL) || (*msg_fmt_str == '\0')  ) ? "" : " " ),
         msg_fmt_str,
         ( fmt_end == NULL ? "\n" : fmt_end )
      );

   } else {
      ret_fmt_str = join_str (
         indent,
         ( header_color == NULL ? MSG_COLOR_DEFAULT : header_color ),
         hdr_str,
         MSG_COLOR_DEFAULT,
         ( ( (hdr == NULL) || (*msg_fmt_str == '\0')  ) ? "" : " " ),
         ( fmt_color == NULL ? MSG_COLOR_DEFAULT : fmt_color ),
         msg_fmt_str,
         MSG_COLOR_DEFAULT,
         ( fmt_end == NULL ? "\n" : fmt_end )
      );
   }

   return ret_fmt_str;
}

void message_vfprintf_color (
   FILE* const stream,
   const char* const header_color,
   const char* const colored_header_text,
   const char* const nocolor_header_text,
   const char* const fmt_color,
   const char* const fmt,
   const char* const fmt_end,
   va_list vargs
) {
   int   esav;
   char* fmt_str;

   if ( stream == NULL ) { return; }

   esav = errno;

   fmt_str = message_get_color_fmt_str (
      header_color, colored_header_text, nocolor_header_text,
      fmt_color, fmt, fmt_end
   );

   if ( fmt_str == NULL ) {
      fprintf (
         stderr, "!!! message_vprintf_color(): failed to create fmt_str\n"
      );
      errno = esav;
      return;
   }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
   vfprintf ( stream, fmt_str, vargs );
#pragma GCC diagnostic pop
   fflush ( stream );
   x_free ( fmt_str );
   errno = esav;
}

void message_fprintf_color (
   FILE* const stream,
   const char* const header_color,
   const char* const colored_header_text,
   const char* const nocolor_header_text,
   const char* const fmt_color,
   const char* const fmt,
   const char* const fmt_end,
   ...
) {
   int esav;
   va_list vargs;

   esav = errno;
   va_start ( vargs, fmt_end );
   message_vfprintf_color (
      stream, header_color, colored_header_text, nocolor_header_text,
      fmt_color, fmt, fmt_end,
      vargs
   );
   va_end ( vargs );
   errno = esav;
}

void message_vfprintf_fmt_str (
   FILE* const stream,
   const char* const fmt_str,
   va_list vargs
) {
   int esav;

   if ( (fmt_str == NULL) || (stream == NULL) ) { return; }

   esav = errno;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
   vfprintf ( stream, fmt_str, vargs );
#pragma GCC diagnostic pop
   fflush ( stream );
   errno = esav;
}

void message_fprintf_fmt_str (
   FILE* const stream,
   const char* const fmt_str, ...
) {
   int esav;
   va_list vargs;

   esav = errno;
   va_start ( vargs, fmt_str );
   message_vfprintf_fmt_str ( stream, fmt_str, vargs );
   va_end ( vargs );
   errno = esav;
}
