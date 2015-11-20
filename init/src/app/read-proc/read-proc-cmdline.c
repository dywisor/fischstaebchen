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
#include <sys/types.h>
#include <string.h>

#include "_base.h"
#include "../../common/mac.h"
#include "../../common/data_types/dynarray.h"
#include "../../common/fs/process_info.h"
#include "../../common/strutil/compare.h"
#include "../../common/strutil/convert.h"
#include "../../common/strutil/lazy_basename.h"



struct splitprog_globals {
   unsigned no_newline;

   const char* arg_sep;
   const char* quote_arg;

   size_t   arg_begin;
   size_t   arg_max;
   unsigned want_prog_basename;
};

static int init_splitprog_globals ( struct splitprog_globals* const g ) {
   if ( g == NULL ) { return -1; }

   g->no_newline       = 0;

   g->arg_sep          = "\n";
   g->quote_arg        = NULL;
   g->arg_begin        = 0;
   g->arg_max          = 0;

   g->want_prog_basename = 0;

   return 0;
}

static void free_splitprog_globals ( struct splitprog_globals* const g ) {
   if ( g == NULL ) { return; }
}


static int main_process_input (
   const struct splitprog_globals* const g,
   const struct splitprog_source* const sps
);




int main ( int argc, char* argv[restrict] ) {
#define USAGE_FMT_STR  \
      "%s {-n|-Q|-q <quote_str>|-F <sep>|-f <range>|-b|-B} <pid>|<file>|-"

   static const char* const short_options = "hnQq:F:f:bB";

   struct splitprog_globals g;
   struct splitprog_source sps;
   int ret;

   if ( init_splitprog_globals ( &g ) != 0 ) { return EX_SOFTWARE; }

   ret = -1;
   GETOPT_SWITCH_LOOP(short_options) {
      case 'F':
         g.arg_sep = optarg;
         break;

      case 'Q':
         g.quote_arg = "\"";
         break;

      case 'q':
         g.quote_arg = optarg;
         break;

      case 'n':
         g.no_newline = 1;
         break;

      case 'h':
         MAIN_PRINT_USAGE ( stdout, USAGE_FMT_STR, argv[0] );
         MAIN_GOTO_EXIT ( EXIT_SUCCESS );

      case 'f':
         if (
            splitprog_parse_range_arg (
               optarg, &(g.arg_begin), &(g.arg_max)
            ) != 0
         ) {
            MAIN_GOTO_EXIT ( EX_USAGE );
         }
         break;

      case 'b':
         g.want_prog_basename = 1;
         break;

      case 'B':
         g.want_prog_basename = 1;
         g.arg_begin          = 0;
         g.arg_max            = 1;
         break;

      default:
         MAIN_GOTO_EXIT ( EX_USAGE );
   } END_GETOPT_SWITCH_LOOP;

   splitprog_main_process_infiles(argc, argv, optind);


main_exit:
   if ( ret == EX_USAGE ) {
      MAIN_PRINT_USAGE ( stderr, USAGE_FMT_STR, argv[0] );
   }

   free_splitprog_globals ( &g );
   return (ret >= 0) ? ret : EXIT_FAILURE;

#undef USAGE_FMT_STR
}


static inline void main_print_cmdline_arg_item (
   const char* const arg_str,
   const char* const quote_seq
) {
   printf ( "%s%s%s", quote_seq, arg_str, quote_seq );
}

static int main_print_cmdline_argv (
   const struct splitprog_globals* const g,
   const struct dynarray* const p_darr
) {
#define LX__PRINT_ARG_SEP  \
   do { \
      if ( is_first ) { \
         is_first = 0; \
      } else { \
         printf ( "%s", g->arg_sep ); \
      } \
   } while (0)


   size_t      k;
   size_t      iter_max;
   unsigned    is_first;
   const char* quote_seq;

   iter_max = dynarray_argc(p_darr);
   if ( (g->arg_max > 0) && (g->arg_max < iter_max) ) {
      iter_max = g->arg_max;
   }

   quote_seq = STR_IS_EMPTY(g->quote_arg) ? "" : g->quote_arg;
   is_first  = 1;

   k = g->arg_begin;
   if ( (g->want_prog_basename != 0) && (k < iter_max) ) {
      is_first = 0;
      main_print_cmdline_arg_item (
         lazy_basename ( dynarray_get_str(p_darr, k) ), quote_seq
      );
      k++;
   }

   for ( ; k < iter_max; k++ ) {
      LX__PRINT_ARG_SEP;
      main_print_cmdline_arg_item ( dynarray_get_str(p_darr, k), quote_seq );
   }

   if ( (is_first == 0) && (g->no_newline == 0) ) { printf ( "\n" ); }
   fflush ( stdout );
   return 0;

#undef LX__PRINT_ARG_SEP
}

static int main_process_input (
   const struct splitprog_globals* const g,
   const struct splitprog_source* const sps
) {
   struct dynarray darr;
   int ret;

   switch ( sps->type ) {
      case SPLITPROG_SOURCE_TYPE_NONE:
         return 0;
   }

   if ( dynarray_init ( &darr, 0 ) != 0 ) { return -1; }

   ret = EX_SOFTWARE;
   switch ( sps->type ) {
      case SPLITPROG_SOURCE_TYPE_FD:
         ret = proc_read_cmdline_from_fd (
            get_splitprog_source_fd(sps), &darr
         );
         break;

      case SPLITPROG_SOURCE_TYPE_FILEPATH:
         ret = proc_read_cmdline_from_file (
            get_splitprog_source_filepath(sps), &darr
         );
         break;

      case SPLITPROG_SOURCE_TYPE_PID:
         ret = proc_read_cmdline_from_pid (
            get_splitprog_source_pid(sps), &darr
         );
         break;
   }

   if ( ret == 0 ) {
      ret = main_print_cmdline_argv ( g, &darr );
   }

   dynarray_free ( &darr );
   return ret;
}



