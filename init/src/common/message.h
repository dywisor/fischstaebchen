/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_MESSAGE_H_
#define _COMMON_MESSAGE_H_

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>

#include "message_defs.h"

/*
 * message.c depends on strutil/join.c,
 * which depends on dynstr.h and dynarray.h
 *
 * => no message functionality available in these files!
 *
 */


extern FILE* MSG_DBGSTREAM;
extern FILE* MSG_DEFSTREAM;
extern FILE* MSG_ERRSTREAM;
extern FILE* MSG_WARNSTREAM;

extern const char* MSG_PREFIX;

extern const char* MSG_COLOR__DBG;
extern const char* MSG_COLOR__DEF;
extern const char* MSG_COLOR__ERR;
extern const char* MSG_COLOR__WARN;

extern int MSG_NOCOLOR;
extern int MSG_INDENT_LEVEL;


#define MSG_PRINT_DBG(s)          print_debug     ( NULL, s )
#define MSG_PRINT(s)              print_message   ( NULL, s )
#define MSG_PRINT_ERR(s)          print_error     ( NULL, s )
#define MSG_PRINT_WARN(s)         print_warning   ( NULL, s )

#define MSG_PRINTF_DBG(fmt,...)   printf_debug    ( NULL, fmt, "", __VA_ARGS__ )
#define MSG_PRINTF(fmt,...)       printf_message  ( NULL, fmt, "", __VA_ARGS__ )
#define MSG_PRINTF_ERR(fmt,...)   printf_error    ( NULL, fmt, "", __VA_ARGS__ )
#define MSG_PRINTF_WARN(fmt,...)  printf_warning  ( NULL, fmt, "", __VA_ARGS__ )

#define MSG_PRINT_CODE_ERR(s) \
   message_fprintf_color ( \
      MSG_ERRSTREAM, \
      MSG_COLOR_MAGENTA, "CODE ERR", NULL, \
      MSG_COLOR_RED, "file %s, at line %d: %s", "\n", \
      __FILE__, __LINE__, ( ( s == NULL ) ? "<unknown error>" : s ) \
   )

#define MSG_APPLY_COLOR(cref,s) \
   ( MSG_NOCOLOR ? s : MSG_COLOR_ ## cref s MSG_COLOR_DEFAULT )


/**
 * perror() + MSG_PRINT_ERR()
 */
#define MSG_PERROR(ident,...) \
   do { \
      if ( errno ) { perror ( ident ); } \
      MSG_PRINTF_ERR ( __VA_ARGS__ ); \
   } while (0)




/**
 * Sets the default message streams and colors.
 */
static inline void message_set_defaults (void) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"
#if DEBUG_BUILD
#pragma GCC diagnostic pop
   MSG_DBGSTREAM    = stderr;
#else
   MSG_DBGSTREAM    = NULL;
#endif
   MSG_DEFSTREAM    = stdout;
   MSG_ERRSTREAM    = stderr;
   MSG_WARNSTREAM   = stderr;

   MSG_COLOR__DBG   = MSG_GET_COLOR(MAGENTA_NB);
   MSG_COLOR__DEF   = MSG_GET_COLOR(GREEN);
   MSG_COLOR__ERR   = MSG_GET_COLOR(RED);
   MSG_COLOR__WARN  = MSG_GET_COLOR(YELLOW);

   MSG_PREFIX       = "*";

   MSG_INDENT_LEVEL = 0;
   MSG_NOCOLOR      = 0;
}

/* obviously not thread-safe */
static inline void message_indent (void) {
   MSG_INDENT_LEVEL = ( MSG_INDENT_LEVEL <= 0) ? 1 : (MSG_INDENT_LEVEL + 1);
}
static inline void message_outdent (void) {
   if ( MSG_INDENT_LEVEL > 0 ) { MSG_INDENT_LEVEL--; }
}

char* message_get_color_fmt_str (
   const char* const header_color,
   const char* const colored_header_text,
   const char* const nocolor_header_text,
   const char* const fmt_color,
   const char* const fmt,
   const char* const fmt_end
);


void message_vfprintf_fmt_str (
   FILE* const stream,
   const char* const fmt_str,
   va_list vargs
);

void message_fprintf_fmt_str (
   FILE* const stream,
   const char* const fmt_str, ...
);

void message_vfprintf_color (
   FILE* const stream,
   const char* const header_color,
   const char* const colored_header_text,
   const char* const nocolor_header_text,
   const char* const fmt_color,
   const char* const fmt,
   const char* const fmt_end,
   va_list vargs
);

void message_fprintf_color (
   FILE* const stream,
   const char* const header_color,
   const char* const colored_header_text,
   const char* const nocolor_header_text,
   const char* const fmt_color,
   const char* const fmt,
   const char* const fmt_end,
   ...
);

void printf_debug (
   const char* const header,
   const char* const fmt, const char* const fmt_end, ...
);

void printf_message (
   const char* const header,
   const char* const fmt, const char* const fmt_end, ...
);

void printf_error (
   const char* const header,
   const char* const fmt, const char* const fmt_end, ...
);

void printf_warning (
   const char* const header,
   const char* const fmt, const char* const fmt_end, ...
);

void print_debug   ( const char* const header, const char* const msg );
void print_message ( const char* const header, const char* const msg );
void print_error   ( const char* const header, const char* const msg );
void print_warning ( const char* const header, const char* const msg );




#endif /* _COMMON_MESSAGE_H_ */