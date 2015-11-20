/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "fileio.h"
#include "../mac.h"
#include "../data_types/dynstr.h"
#include "../data_types/dynarray.h"
#include "../strutil/join.h"
#include "../strutil/format.h"
#include "../strutil/split.h"


static int _read_file_to_str (
   const char* const filepath,
   char** const str_out,
   const char* (*fconvert_char)(size_t,const char*),
   const int do_rstrip
);

static int _read_file_to_dynarray (
   const char* const filepath,
   char** const file_data_out,
   struct dynarray* const p_darr,
   const char* const delimiters,
   const char* (*fconvert_char)(size_t,const char*),
   const int do_rstrip
);

int write_text_file (
   const char* const filepath,
   const char* const text,
   const int    flags,
   const mode_t mode
) {
   int retcode;
   int fd;
   ssize_t write_ret;
   size_t bytes_written;
   size_t textlen;

   errno = 0;
   fd    = open ( filepath, O_WRONLY|flags, mode );

   if ( fd < 0 ) {
/*
      MSG_PERROR ( "write_file, open", "failed to open %s!\n", filepath );
*/
      return -1;
   }

   retcode = 0;

   if ( text != NULL ) {
      textlen       = strlen ( text );
      bytes_written = 0;

      while ( bytes_written < textlen ) {
         write_ret = write (
            fd, (const void*) (text+bytes_written), (textlen-bytes_written)
         );

         if ( write_ret < 0 ) {
/*
            MSG_PERROR (
               "write_file, write", "failed to write %s!\n", filepath
            );
*/
            retcode = -2;
            break;
         }

         bytes_written += (size_t)write_ret;
      }
   }

   if ( close ( fd ) != 0 ) {
/*
      MSG_PERROR (
         "write_file, close", "failed to close %s!\n", filepath
      );
*/
      if ( retcode == 0 ) { retcode = -3; }
   }

   return retcode;
}

int write_sysfs_file (
   const char* const filepath,
   const char* const text
) {
   errno = 0;
   return write_text_file ( filepath, text, O_TRUNC, 0 );
}

static inline const char* _read_sysfs_file_convert_char (
   const size_t str_len,
   const char* const pchr_in
) {
   static const char NEWLINE = '\n';

   switch (*pchr_in) {
      case 0x00:
         return NULL;

      case 0x09: /* horizontal tab */
      case 0x0B: /* vertical tab */
      case 0x0C: /* form feed */
      case 0x20: /* space */
         /* strip leading whitespace */
         return (str_len == 0) ? NULL : pchr_in;

      case 0x0A: /* line feed */
      case 0x0D: /* carriage return */
         /* strip leading whitespace / replace \r with \n */
         return (str_len == 0) ? NULL : &NEWLINE;

      default:
         return pchr_in;
   }
}

int read_sysfs_file (
   const char* const filepath,
   char** const str_out
) {
   return _read_file_to_str (
      filepath, str_out, _read_sysfs_file_convert_char, 1
   );
}

int sysfs_read_str (
   const char* const dirpath,
   const char* const filename,
   char** const str_out
) {
   char* filepath;
   int ret;

   filepath = join_str_triple ( dirpath, "/", filename );

   ret = read_sysfs_file ( filepath, str_out );
   x_free ( filepath );

   return ret;
}

static inline const char* _read_cmdline_file_convert_char (
   const size_t str_len,
   const char* const pchr_in
) {
   static const char DELIMITER = ' ';

   switch (*pchr_in) {
      case 0x00:
      case 0x09: /* horizontal tab */
      case 0x0B: /* vertical tab */
      case 0x0C: /* form feed */
      case 0x20: /* space */
      case 0x0A: /* line feed */
      case 0x0D: /* carriage return */
         /* strip leading (empty) fields */
         return (str_len == 0) ? NULL : &DELIMITER;

      default:
         return pchr_in;
   }
}

int read_cmdline_file (
   const char* const filepath,
   char** const str_out,
   struct dynarray* const p_darr
) {
   return _read_file_to_dynarray (
      filepath, str_out, p_darr, " ", _read_cmdline_file_convert_char, 1
   );
}

int sysfs_write_int64_decimal (
   const char* const dirpath,
   const char* const filename,
   const int64_t value
) {
#define BUFSIZE 21  /* 19 digits + sign + '\0' */
   char istr [BUFSIZE];

   switch ( str_format ( NULL, istr, BUFSIZE,  "%" PRId64, value ) ) {
      case STRFORMAT_RET_SUCCESS:
         return sysfs_write_str ( dirpath, filename, istr );

      default:
         return -1;
   }
#undef BUFSIZE
}

int sysfs_write_uint64_decimal (
   const char* const dirpath,
   const char* const filename,
   const uint64_t value
) {
#define BUFSIZE 21 /* 20 digits + '\0' */
   char istr [BUFSIZE];

   switch ( str_format ( NULL, istr, BUFSIZE, "%" PRIu64, value ) ) {
      case STRFORMAT_RET_SUCCESS:
         return sysfs_write_str ( dirpath, filename, istr );

      default:
         return -1;
   }
#undef BUFSIZE
}

int sysfs_write_str (
   const char* const dirpath,
   const char* const filename,
   const char* const str
) {
   char* filepath;
   int ret;

   filepath = join_str_triple ( dirpath, "/", filename );

   ret = write_sysfs_file ( filepath, str );
   x_free ( filepath );

   return ret;
}

static int _read_file_to_str (
   const char* const filepath,
   char** const str_out,
   const char* (*fconvert_char)(size_t,const char*),
   const int do_rstrip
) {
#define BUFSIZE FILEIO_READFILE_BUFSIZE
   char buffer[BUFSIZE];

   int     fd;
   int     retcode;
   ssize_t bytes_read;
   struct dynstr_data dynstr;

   /* sanity checks */
   if (  str_out == NULL ) { return -5; }
   if ( *str_out != NULL ) { return -6; }

   /* initialize dyn.-allocated str */
   if ( dynstr_data_init ( &dynstr, 0 ) != 0 ) { return -1; }

   /* open */
   fd = open ( filepath, O_RDONLY );
   if ( fd < 0 ) {
      dynstr_data_free ( &dynstr );
      return -1;
   }

   /* read */
   retcode    = 0;
   bytes_read = read ( fd, buffer, BUFSIZE );
   while ( bytes_read > 0 ) {
      if (
         dynstr_data_append_str (
            &dynstr, (size_t)bytes_read, buffer, fconvert_char
         ) != 0
      ) {
         close (fd); /* unchecked retcode */
         return -1;
      }

      bytes_read = read ( fd, buffer, BUFSIZE );
   }
   if ( bytes_read != 0 ) { retcode = -5; }

   /* close */
   close(fd); /* unchecked retcode */

   /* terminate str, strip ending newlines */
   if ( do_rstrip ) {
      if ( dynstr_data_rstrip ( &dynstr ) != 0 ) { return -1; }
   }
   if ( dynstr_data_append_char ( &dynstr, '\0' ) != 0 ) { return -1; }

   /* transfer str to str_out */
   dynstr_data_unref ( &dynstr, str_out );

   /* done */
   return retcode;
#undef BUFSIZE
}

static int _read_file_to_dynarray (
   const char* const filepath,
   char** const file_data_out,
   struct dynarray* const p_darr,
   const char* const delimiters,
   const char* (*fconvert_char)(size_t,const char*),
   const int do_rstrip
) {
   int   retcode;
   char* file_data;

   /* sanity checks */
   if (      p_darr == NULL ) { return -5; }
   if ( p_darr->arr == NULL ) { return -6; }
   if (  delimiters == NULL ) { return -7; }
   if ( *delimiters == '\0' ) { return -8; }

   file_data = NULL;
   retcode = _read_file_to_str (
      filepath, &file_data, fconvert_char, do_rstrip
   );

   if ( (file_data_out != NULL) && (file_data != NULL) ) {
      *file_data_out = strdup ( file_data );
   }


   if ( retcode == 0 ) {
      if (
         str_split_unsafe ( p_darr, file_data, NULL, delimiters, -1 ) < 0
      ) {
         retcode = -1;
      }
   }

   x_free ( file_data );
   return retcode;
}
