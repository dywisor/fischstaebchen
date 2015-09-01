/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _APP_CMDLINE_SPLIT__BASE_H_
#define _APP_CMDLINE_SPLIT__BASE_H_

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sysexits.h>
#include <sys/types.h>


#include "../../common/strutil/compare.h"
#include "../../common/strutil/convert.h"


enum {
    SPLITPROG_SOURCE_TYPE_NONE,
    SPLITPROG_SOURCE_TYPE_FILEPATH,
    SPLITPROG_SOURCE_TYPE_PID,
    SPLITPROG_SOURCE_TYPE_FD
};

union _splitprog_source_union {
    const char* filepath;
    pid_t       pid;
    int         fd;
};

struct splitprog_source {
    int type;
    union _splitprog_source_union data;
};
#define get_splitprog_source_filepath(u_ptr)  (((u_ptr)->data).filepath)
#define get_splitprog_source_pid(u_ptr)       (((u_ptr)->data).pid)
#define get_splitprog_source_fd(u_ptr)        (((u_ptr)->data).fd)


void set_splitprog_source_none (
    struct splitprog_source* const sps
);

void set_splitprog_source_filepath (
    struct splitprog_source* const sps,
    const char* const filepath
);

void set_splitprog_source_pid (
    struct splitprog_source* const sps,
    const pid_t pid
);

void set_splitprog_source_fd (
    struct splitprog_source* const sps,
    const int fd
);

void set_splitprog_source_default (
    struct splitprog_source* const sps
);


int splitprog_parse_range_arg (
   const char* const restrict input_str,
   size_t* const restrict start_idx_out,
   size_t* const restrict max_idx_out
);

#define MAIN_GOTO_EXIT(exit_code)  \
   do { ret = exit_code; goto main_exit; } while (0)

#define MAIN_GOTO_EXIT_IFERROR(int_func_call)  \
   do { ret = int_func_call; if ( ret != 0 ) { goto main_exit; } } while (0)


#define MAIN_PRINT_USAGE(outstream, usage_fmt_str, prog)  \
   fprintf ( outstream, "Usage: " usage_fmt_str "\n", prog )


#define _GETOPT_SWITCH_LOOP(argc, argv, short_options)  \
   do { \
      int opt; \
      \
      while ( ( opt = getopt ( argc, argv, short_options ) ) >= 0 ) { \
         switch ( opt )

#define GETOPT_SWITCH_LOOP(short_options)  \
   _GETOPT_SWITCH_LOOP(argc, argv, short_options)

#define END_GETOPT_SWITCH_LOOP    } } while (0)


#define _splitprog_main_process_infiles(p_g, p_sps, sargc, sargv)  \
   do { \
      if ( (sargc) < 1 ) { \
         set_splitprog_source_default ( (p_sps) ); \
         MAIN_GOTO_EXIT_IFERROR ( main_process_input ( (p_g), (p_sps) ) ); \
      } else { \
         const char* arg;   \
         int         k;     \
         long        ibuf;  \
         \
         for ( k = 0; k < (sargc); k++ ) { \
            arg = (sargv) [k]; \
            set_splitprog_source_none ( (p_sps) ); \
            \
            if ( STR_IS_EMPTY(arg) ) { \
               MAIN_GOTO_EXIT ( EX_USAGE ); \
               \
            } else if ( (arg[0] == '-') && (arg[1] == '\0') ) { \
               set_splitprog_source_fd ( (p_sps), STDIN_FILENO ); \
               \
            } else if ( \
                (arg[0] != '/') && (str_to_long ( arg, &ibuf ) == 0) \
            ) { \
               set_splitprog_source_pid ( (p_sps), (pid_t)ibuf ); \
               \
            } else { \
               set_splitprog_source_filepath ( (p_sps), arg ); \
            } \
            \
            MAIN_GOTO_EXIT_IFERROR ( main_process_input ( (p_g), (p_sps) ) ); \
         } \
      } \
   } while (0)

#define splitprog_main_process_infiles(_argc, _argv, _optind)  \
   _splitprog_main_process_infiles(&g, &sps, _argc - _optind, _argv + _optind)


#endif /* _APP_CMDLINE_SPLIT__BASE_H_ */
