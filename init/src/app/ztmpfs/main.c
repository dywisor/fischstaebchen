/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>

#include "../../common/mac.h"
#include "../../common/message.h"
#include "../../common/fs/constants.h"
#include "../../common/strutil/compare.h"
#include "../../common/dynstr.h"

#include "../../zram/globals.h"
#include "../../zram/tmpfs.h"


enum {
   PROG_MODE_UNDEF,
   PROG_MODE_ZTMPFS,
   PROG_MODE_ZDISK
};

extern int zram_tmpfs (
   const char* const name,
   const char* const mp,
   unsigned long flags,
   const char* const opts,
   const char* const size_spec,
   const size_t sys_memsize_m,
   const mode_t default_mode
);

static int name_to_prog_mode ( const char* const name ) {
   if ( STR_IS_EMPTY ( name ) ) { return PROG_MODE_UNDEF; }

   if ( strcmp ( name, "zdisk" ) == 0 ) {
      return PROG_MODE_ZDISK;
   }

   if ( strcmp ( name, "ztmpfs" ) == 0 ) {
      return PROG_MODE_ZTMPFS;
   }

   return PROG_MODE_UNDEF;
}

static int get_prog_mode ( const char* const prog ) {
   const char* name;
   const char* mname;
   name = strrchr ( prog, '/' );
   if ( STR_IS_EMPTY ( name ) ) { return PROG_MODE_ZTMPFS; }

   name++;

   mname = str_startswith ( name, "mount." );
   return name_to_prog_mode ( mname != NULL ? mname : name );
}

int main ( int argc, char** argv ) {
#define USAGE_FMT_STR  \
      "%s [-f] [-q] [-t ztmpfs|zdisk] [-o <opts>] [-S <size>] [-m <mode>] [<name>] <mountpoint>"

   static const char* const short_options = "t:o:S:m:hfq";

   int opt;
   int retcode;
   int prog_mode;
   int fake_mode;
   int quiet_mode;
   struct dynstr_data opts_dynstr;
   char* opts_str;
   const char* size;
   const char* name;
   const char* mode_str;
   const char* mp;

   opts_str = NULL;
   if ( dynstr_data_init ( &opts_dynstr, 80 ) != 0 ) { return EX_SOFTWARE; }

   prog_mode   = -1;
   retcode     = EXIT_FAILURE;
   size        = NULL;
   name        = NULL;
   mp          = NULL;
   mode_str    = NULL;
   fake_mode   = 0;
   quiet_mode  = 0;

   message_set_defaults();
   zram_init_globals();

   while ( ( opt = getopt ( argc, argv, short_options ) ) != -1 ) {
      switch (opt) {
         case 't':
            prog_mode = name_to_prog_mode ( optarg );
            if ( prog_mode == PROG_MODE_UNDEF ) {
               printf_error (
                  NULL, "Unknown zram disk type: %s", "\n", optarg
               );
               retcode = EX_USAGE;
               goto main_exit;
            }
            break;

         case 'S':
            size = STR_IS_EMPTY ( optarg ) ? NULL : optarg;
            break;

         case 'o':
            if ( STR_IS_NOT_EMPTY ( optarg ) ) {
               if (
                  dynstr_data_cjoin_append ( &opts_dynstr, ',', optarg ) != 0
               ) {
                  retcode = EX_SOFTWARE;
                  goto main_exit;
               }
            }
            break;

         case 'm':
            mode_str = STR_IS_EMPTY ( optarg ) ? NULL : optarg;
            break;

         case 'h':
            fprintf ( stdout, "Usage: " USAGE_FMT_STR "\n", argv[0] );
            retcode = EXIT_SUCCESS;
            goto main_exit;

         case 'f':
            fake_mode = 1;
            break;

         case 'q':
            quiet_mode = 1;
            break;

         default:
            retcode = EX_USAGE;
            goto main_exit;
      }
   }

   if ( prog_mode < 0 ) {
      prog_mode = get_prog_mode ( argv[0] );
      if ( prog_mode == PROG_MODE_UNDEF ) { prog_mode = PROG_MODE_ZTMPFS; }
   }

   switch ( argc - optind ) {
      case 1:
         name = NULL; /* nop */
         mp   = argv[optind];
         break;

      case 2:
         name = argv[optind];
         mp   = argv[optind+1];
         break;

      default:
         if ( optind >= argc ) {
            print_error ( NULL, "not enough args!" );
         } else {
            print_error ( NULL, "too many args!" );
         }

         retcode = EX_USAGE;
         goto main_exit;
   }

   if ( STR_IS_EMPTY ( mp ) ) {
      retcode = EX_USAGE;
      goto main_exit;
   }

   if ( mode_str != NULL ) {
      if (
         (dynstr_data_cjoin_append ( &opts_dynstr, ',', "mode=" ) != 0)
         || (dynstr_data_append_str_simple ( &opts_dynstr, mode_str ) != 0)
      ) {
         retcode = EX_SOFTWARE;
         goto main_exit;
      }
   }

   opts_str = NULL; /* nop */
   if ( opts_dynstr.current_size ) {
      if ( dynstr_data_append_null ( &opts_dynstr ) != 0 ) {
         retcode = EX_SOFTWARE;
         goto main_exit;
      }

      dynstr_data_unref ( &opts_dynstr, &opts_str );
   }
   dynstr_data_free ( &opts_dynstr ); /* double-free safe */

   if ( !quiet_mode ) {
      if ( fake_mode ) {
         message_fprintf_color (
            stdout,
            MSG_COLOR__DEF,
            "mount", NULL,
            NULL,
            "<%s> -o %s%s%s %s %s", "\n",
            ( prog_mode == PROG_MODE_ZDISK ? "zdisk" : "ztmpfs" ),
            ( opts_str == NULL ? "defaults" : opts_str ),
            ( size == NULL ? "" : ",size=" ),
            ( size == NULL ? "" : size ),
            ( name == NULL ? "<default_name>" : name ),
            mp
         );
      }
   }

   if ( fake_mode ) {
      retcode = EXIT_SUCCESS;
   } else {
      retcode = zram_tmpfs (
         name, mp, 0, opts_str, size, 0,
         ( prog_mode == PROG_MODE_ZDISK ? RWX_RX_RX : SRWX_RWX_RWX )
      );
   }

main_exit:
   zram_free_globals();
   x_free ( opts_str );
   dynstr_data_free ( &opts_dynstr );

   switch (retcode) {
      case EX_USAGE:
         printf_error ( "Usage:", USAGE_FMT_STR, "\n", argv[0] );
         break;
   }
   return retcode;
#undef USAGE_FMT_STR
}
