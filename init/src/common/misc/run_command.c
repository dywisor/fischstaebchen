/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include "run_command.h"
#include "../dynarray.h"
#include "../dynstr.h"
#include "../message.h"
#include "../mac.h"
#include "../strutil/join.h"


#define RUNCMD_RET_VARNAME  retcode

/**
 * run_command macro containing the exec() code
 *
 * Replaces the current process and/or exits
 * (should be run in a child session after fork()).
 *
 * @param EXECFUNC        function; exec() func, e.g. execl() or execvp()
 * @param ex_execfail     int; exit code if EXECFUNC() returns (i.e. error occured)
 * @param close_stdin     int; controls wheter stdin  should be closed
 * @param close_stdout    int; controls wheter stdout should be closed
 * @param close_stderr    int; controls wheter stderr should be closed
 * @param path            [const] char*; filename/filepath of the executable
 * @param <va_list>       additional parameters for EXECFUNC
 *
 */
#define \
   RUN_COMMAND_CHILD_EXEC(\
      EXECFUNC, \
      ex_execfail, \
      close_stdin, close_stdout, close_stderr, \
      path, ...\
   ) \
   do { \
      run_command_do_close_streams ( \
         close_stdin, close_stdout, close_stderr \
      ); \
      EXECFUNC ( path, __VA_ARGS__ ); \
      perror ( "exec() failed" ); \
      printf_error ( "(cmd)", "%s", "\n", "failed to exec" ); \
      _exit ( ex_execfail ); \
   } while(0)


/**
 * base macro for run_command functionality.
 *
 * Handles fork()/exec()/waitpid().
 * The exec part is handled by RUN_COMMAND_CHILD_EXEC() (see above),
 * which is called with EX_OSERR as ex_execfail parameter, all other args
 * are passed as-is to this macro.
 *
 * Writes the command's exit code to RUNCMD_RET_VARNAME (retcode),
 * which has to be declared manually.
 *
 * @param EXECFUNC   function; see RUN_COMMAND_CHILD_EXEC()
 * @param <va_list>  all remaining args passed to RUN_COMMAND_CHILD_EXEC()
 */
#define _RUN_COMMAND_GENERIC(EXECFUNC,...) \
   do { \
      int status; \
      pid_t pid; \
      \
      errno   = 0; \
      RUNCMD_RET_VARNAME = -1; \
      status  = 0; \
      pid     = fork(); \
      \
      switch (pid) { \
         case -1: \
            return -2; \
            break; /* nop */ \
         \
         case 0: \
            RUN_COMMAND_CHILD_EXEC ( EXECFUNC, EX_OSERR, __VA_ARGS__ ); \
            break; /* nop */ \
         \
         default: \
            waitpid ( pid, &status, 0 ); \
            RUNCMD_RET_VARNAME = WEXITSTATUS ( status ); \
            break; /* nop */ \
      } \
   } while(0)

/**
 * Shorthand for
 *  _RUN_COMMAND_GENERIC (
 *     EXECFUNC, <close stdin>, <keep stdout>, <keep stderr>, ...
 *  )
 */
#define RUN_COMMAND_GENERIC(EXECFUNC,...) \
   _RUN_COMMAND_GENERIC ( EXECFUNC, -1, 0, 0, ##__VA_ARGS__ )

/**
 * Shorthand for
 *  _RUN_COMMAND_GENERIC (
 *     EXECFUNC, <close stdin>, <close stdout>, <keep stderr>, ...
 *  )
 */
#define RUN_COMMAND_GENERIC_QUIET(EXECFUNC,...) \
   _RUN_COMMAND_GENERIC ( EXECFUNC, -1, -1, 0, ##__VA_ARGS__ )

/**
 * Shorthand for
 *  _RUN_COMMAND_GENERIC (
 *     EXECFUNC, <close stdin>, <close stdout>, <close stderr>, ...
 *  )
 */
#define RUN_COMMAND_GENERIC_EXTRAQUIET(EXECFUNC,...) \
   _RUN_COMMAND_GENERIC ( EXECFUNC, -1, -1, -1, ##__VA_ARGS__ )


/**
 * execlp() variants of the RUN_COMMAND_GENERIC*() macros defined above.
 * The NULL parameter at the end of the var args list doesn't need to be
 * passed (// gets automatically appended).
 */
#define RUNCMD(...)  \
   RUN_COMMAND_GENERIC (execlp, __VA_ARGS__, NULL )
#define RUNCMD_QUIET(...)  \
   RUN_COMMAND_GENERIC_QUIET (execlp, __VA_ARGS__, NULL )
#define RUNCMD_EXTRAQUIET(...)  \
   RUN_COMMAND_GENERIC_EXTRAQUIET (execlp, __VA_ARGS__, NULL )


void run_command_do_close_streams (
   const int close_stdin,
   const int close_stdout,
   const int close_stderr
) {
   if ( MSG_DBGSTREAM != NULL ) {
      printf_debug (
         "(cmd)",
         "redirecting streams stdin=%d stdout=%d stderr=%d", "\n",
         close_stdin, close_stdout, close_stderr
      );
   }

   if ( close_stdin < 0 ) {
      IGNORE_RETCODE ( freopen ( DEVNULL_FSPATH, "r", stdin ) );
   }

   if ( close_stdout < 0 ) {
      IGNORE_RETCODE ( freopen ( DEVNULL_FSPATH, "a", stdout ) );
   }

   if ( close_stderr < 0 ) {
      IGNORE_RETCODE ( freopen ( DEVNULL_FSPATH, "a", stderr ) );
   }
}

static int _do_run_command_argv (
   const char* const path,
   const char* const argv[],
   const int close_stdin,
   const int close_stdout,
   const int close_stderr
);

static void _run_command_print_argv (
   FILE* const outstream, const char* const argv[]
);

static void _run_command_print_argv (
   FILE* const outstream, const char* const argv[]
) {
   struct dynstr_data dynstr;
   const char* const* iter;

   if ( argv == NULL ) { return; }
   if ( outstream == NULL ) { return; }
   if ( dynstr_data_init ( &dynstr, 0 ) != 0 ) { return; }

   for ( iter = argv; *iter != NULL; iter++ ) {
      if ( dynstr_data_cjoin_append ( &dynstr, ' ', *iter ) != 0 ) { return; }
   }

   if ( dynstr_data_append_null ( &dynstr ) != 0 ) { return; }

   message_fprintf_color (
      outstream, MSG_COLOR__DBG, "(cmd)", NULL,
      MSG_COLOR_WHITE, "%s", "\n", dynstr.data
   );

   dynstr_data_free ( &dynstr );
}

int run_command_argv_with_stdin (
   const char* const path,
   const char* const argv[]
) {
   if ( (argv == NULL) || (*argv == NULL) || (**argv == '\0') ) { return -1; }

   printf_debug ( "(cmd)", "running %s", "\n", argv[0] );
   return _do_run_command_argv ( path, argv, 0, 0, 0 );
}

int run_command_argv (
   const char* const path,
   const char* const argv[],
   const int close_stdout,
   const int close_stderr
) {
   if ( (argv == NULL) || (*argv == NULL) || (**argv == '\0') ) { return -1; }

   printf_debug ( "(cmd)", "running %s", "\n", argv[0] );
   return _do_run_command_argv ( path, argv, -1, close_stdout, close_stderr );
}

int run_command_dynarray (
   const char* const path,
   const struct dynarray* const p_darr,
   const int close_stdout,
   const int close_stderr
) {
   return run_command_argv (
      path,
      (const char* const*)(p_darr->arr),
      close_stdout, close_stderr
   );
}

int run_command_va_list (
   const char* const path,
   va_list vargs,
   const int close_stdout,
   const int close_stderr
) {
   int retcode;
   struct dynarray darr;

   if ( dynarray_init ( &darr, 3 ) != 0 ) { return -1; }
   darr.flags |= DYNARRAY_IS_CONST;

   if ( va_list_to_dynarray ( &darr, vargs, 0 ) != 0 ) {
      retcode = -1;
   } else if ( dynarray_append ( &darr, NULL ) != 0 ) {
      retcode = -1;
   } else {
      retcode = run_command_dynarray (
         path, &darr, close_stdout, close_stderr
      );
   }

   dynarray_free ( &darr );
   return retcode;
}

int _run_command (
   const int close_stdout,
   const int close_stderr,
   const char* const path, ...
) {
   int retcode;
   va_list vargs;

   va_start ( vargs, path );
   retcode = run_command_va_list ( path, vargs, close_stdout, close_stderr );
   va_end ( vargs );

   return retcode;
}


static int _do_run_command_argv (
   const char* const path,
   const char* const argv[],
   const int close_stdin,
   const int close_stdout,
   const int close_stderr
) {
   int RUNCMD_RET_VARNAME;

   _run_command_print_argv ( MSG_DBGSTREAM, argv );

   _RUN_COMMAND_GENERIC (
      execvp,
      close_stdin,
      close_stdout,
      close_stderr,
      ( path == NULL ? argv[0] : path ),
      (char* const*) argv
   );

   return RUNCMD_RET_VARNAME;
}



int run_command_until_success_tickrate_argv (
   const char* const path,
   const char* const argv[],
   const int close_stdout,
   const int close_stderr,
   FILE* const message_stream,
   const char* const message,
   const unsigned timeout_decisecs,
   const unsigned numticks
) {
   const char*      prog_path;
   int              retcode;
   unsigned         ds_rem;
   struct timespec  sleep_time;

   if ( (argv == NULL) || (*argv == NULL) || (**argv == '\0') ) { return -1; }

   prog_path = ( path == NULL ? argv[0] : path );

   if ( numticks > 9 ) {
      /* not supported */
      errno = EINVAL;
      return -1;
   }

   printf_debug ( "(cmd)", "running %s", "\n", argv[0] );
   retcode = _do_run_command_argv (
      prog_path, argv, -1, close_stdout, close_stderr
   );

   if ( (retcode <= 0) || (retcode == EX_OSERR) ) {
      /* be done now */
      return retcode;
   }

   if ( message_stream != NULL ) {
      message_fprintf_color (
         message_stream,
         MSG_COLOR__DEF, MSG_PREFIX, NULL,
         MSG_COLOR_DEFAULT, "%s%s", " ",
         "Waiting for ", ( message == NULL ? argv[0] : message )
      );
   }


   /* tickrate is 10Hz (- overhead) */
   ds_rem = timeout_decisecs + 1;
   while ( ds_rem > 0 ) {
      sleep_time.tv_sec = 0;
      if ( ds_rem < numticks ) {
         sleep_time.tv_nsec = ds_rem * 100000000;
         ds_rem = 0;
      } else {
         sleep_time.tv_nsec = numticks * 100000000;
         ds_rem -= numticks;
      }

      if ( nanosleep ( &sleep_time, NULL ) != 0 ) {
         perror ( "run_command->nanosleep" );
         retcode = -10;
         break;
      }

      retcode = _do_run_command_argv (
         prog_path, argv, -1, close_stdout, close_stderr
      );

      if ( retcode == 0 ) {
         /* message done */
         if ( message_stream != NULL ) {
            fprintf (
               message_stream, " %s\n", MSG_APPLY_COLOR(GREEN,"done")
            );
            fflush ( message_stream );
         }
         break;

      } else if ( (retcode < 0) || (retcode == EX_OSERR) ) {
         /* message OSFAIL */
         if ( message_stream != NULL ) {
            fprintf (
               message_stream, " %s\n", MSG_APPLY_COLOR(RED,"OSFAIL")
            );
            fflush ( message_stream );
         }
         break;

      } else {
         /* message "." */
         fprintf ( message_stream, "." );
         fflush ( message_stream );
      }

   }


   if ( (retcode > 0) && (retcode != EX_OSERR) ) {
      /* message giving up */
      if ( message_stream != NULL ) {
         fprintf (
            message_stream, " %s\n", MSG_APPLY_COLOR(YELLOW,"giving up")
         );
         fflush ( message_stream );
      }
   }

   return retcode;
}

int run_command_until_success_argv (
   const char* const path,
   const char* const argv[],
   const int close_stdout,
   const int close_stderr,
   FILE* const message_stream,
   const char* const message,
   const unsigned timeout_decisecs
) {
   return run_command_until_success_tickrate_argv (
      path, argv,
      close_stdout, close_stderr,
      message_stream, message,
      timeout_decisecs, 2
   );
}

int run_command_until_success_dynarray (
   const char* const path,
   const struct dynarray* const p_darr,
   const int close_stdout,
   const int close_stderr,
   FILE* const message_stream,
   const char* const message,
   const unsigned timeout_decisecs
) {
   return run_command_until_success_argv (
      path,
      (const char* const*)(p_darr->arr),
      close_stdout, close_stderr,
      message_stream, message,
      timeout_decisecs
   );
}

int run_command_until_success_va_list (
   const char* const path,
   va_list vargs,
   const int close_stdout,
   const int close_stderr,
   FILE* const message_stream,
   const char* const message,
   const unsigned timeout_decisecs
) {
   int retcode;
   struct dynarray darr;

   if ( dynarray_init ( &darr, 3 ) != 0 ) { return -1; }
   darr.flags |= DYNARRAY_IS_CONST;

   if ( va_list_to_dynarray ( &darr, vargs, 0 ) != 0 ) {
      retcode = -1;
   } else if ( dynarray_append ( &darr, NULL ) != 0 ) {
      retcode = -1;
   } else {
      retcode = run_command_until_success_dynarray (
         path, &darr,
         close_stdout, close_stderr,
         message_stream, message,
         timeout_decisecs
      );
   }

   dynarray_free ( &darr );
   return retcode;
}

int _run_command_until_success (
   const int close_stdout,
   const int close_stderr,
   FILE* const message_stream,
   const char* const message,
   const unsigned timeout_decisecs,
   const char* const path, ...
) {
   int retcode;
   va_list vargs;

   va_start ( vargs, path );
   retcode = run_command_until_success_va_list (
      path,
      vargs,
      close_stdout, close_stderr,
      message_stream, message,
      timeout_decisecs
   );
   va_end ( vargs );

   return retcode;
}
