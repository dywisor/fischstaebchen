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
#include <fnmatch.h>

#include "_base.h"
#include "../../common/mac.h"
#include "../../common/dynarray.h"
#include "../../common/fs/process_info.h"
#include "../../common/strutil/compare.h"
#include "../../common/strutil/convert.h"



struct splitprog_globals {
   unsigned no_newline;

   const char* arg_sep;
   const char* quote_arg;

   struct dynarray match_varnames;
};

static int init_splitprog_globals ( struct splitprog_globals* const g ) {
   if ( g == NULL ) { return -1; }

   g->no_newline       = 0;

   g->arg_sep          = "\n";
   g->quote_arg        = NULL;

   if ( dynarray_init ( &(g->match_varnames), 0 ) != 0 ) { return -1; }
   dynarray_set_data_readonly ( &(g->match_varnames) );

   return 0;
}

static void free_splitprog_globals ( struct splitprog_globals* const g ) {
   if ( g == NULL ) { return; }

   dynarray_free ( &(g->match_varnames) );
}


static int main_process_input (
   const struct splitprog_globals* const g,
   const struct splitprog_source* const sps
);


int main ( int argc, char* argv[restrict] ) {
#define USAGE_FMT_STR  \
      "%s {-n|-Q|-q <quote_str>|-F <sep>} {-V <varname_pattern>} <pid>|<file>|-"

   static const char* const short_options = "hnQq:F:V:";

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

      case 'V':
         if ( STR_IS_EMPTY(optarg) ) {
            MAIN_GOTO_EXIT ( EX_USAGE );
         } else if (
            dynarray_append_strnodup ( &(g.match_varnames), optarg ) != 0
         ) {
            MAIN_GOTO_EXIT ( EX_SOFTWARE );
         }
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


static inline void main_print_environ_item (
   const struct environ_varname_value_pair* const evp,
   const char* const quote_seq
) {
   printf (
      "%s=%s%s%s", evp->varname, quote_seq, evp->value, quote_seq
   );
}

static inline int main_print_environ_fnmatch_varname (
   const struct dynarray* const varname_patterns,
   const char* const varname
) {
#define FNMATCH_VARNAME_FLAGS  ( FNM_PATHNAME )

   const char* varname_pattern;
   size_t k;

   dynarray_foreach ( varname_patterns, k ) {
      varname_pattern = dynarray_get_str ( varname_patterns, k );

      switch (
         fnmatch ( varname_pattern, varname, FNMATCH_VARNAME_FLAGS )
      ) {
         case 0:
            return 0;

         case FNM_NOMATCH:
            break;

         default:
            return -1;
      }
   }

   return 1;
#undef FNMATCH_VARNAME_FLAGS
}


static int main_print_environ (
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
   unsigned    is_first;
   const char* quote_seq;
   struct environ_varname_value_pair evp;

   is_first  = 1;
   quote_seq = (g->quote_arg == NULL) ? "" : g->quote_arg;

   /* evp does not need to be freed when NULL-initialized */
   environ_varname_value_pair_init ( &evp, NULL );

   if ( (g->match_varnames).len > 0 ) {
      dynarray_foreach ( p_darr, k ) {
         switch (
            environ_varname_value_pair_set (
               &evp, dynarray_get_str(p_darr, k)
            )
         ) {
            case ENVIRON_VARNAME_VALUE_PAIR_PARSE_SUCCESS:
               if (
                  main_print_environ_fnmatch_varname (
                     &(g->match_varnames),
                     evp.varname
                  ) == 0
               ) {
                  LX__PRINT_ARG_SEP;
                  main_print_environ_item ( &evp, quote_seq );
               }

               environ_varname_value_pair_free ( &evp );
               break;

            default:
               environ_varname_value_pair_free ( &evp );
               return -1;
         }
      }

   } else if ( STR_IS_EMPTY(quote_seq) ) {
      dynarray_foreach ( p_darr, k ) {
         LX__PRINT_ARG_SEP;
         printf ( "%s", dynarray_get_str(p_darr, k) );
      }

   } else {
      dynarray_foreach ( p_darr, k ) {
         switch (
            environ_varname_value_pair_set (
               &evp, dynarray_get_str(p_darr, k)
            )
         ) {
            case ENVIRON_VARNAME_VALUE_PAIR_PARSE_SUCCESS:
               LX__PRINT_ARG_SEP;
               main_print_environ_item ( &evp, quote_seq );

               environ_varname_value_pair_free ( &evp );
               break;

            default:
               environ_varname_value_pair_free ( &evp );
               return -1;
         }


      }
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
         ret = proc_read_environ_from_fd (
            get_splitprog_source_fd(sps), &darr
         );
         break;

      case SPLITPROG_SOURCE_TYPE_FILEPATH:
         ret = proc_read_environ_from_file (
            get_splitprog_source_filepath(sps), &darr
         );
         break;

      case SPLITPROG_SOURCE_TYPE_PID:
         ret = proc_read_environ_from_pid (
            get_splitprog_source_pid(sps), &darr
         );
         break;
   }

   if ( ret == 0 ) {
      ret = main_print_environ ( g, &darr );
   }

   dynarray_free ( &darr );
   return ret;
}
