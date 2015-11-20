/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_MISC_RUN_COMMAND_H_
#define _COMMON_MISC_RUN_COMMAND_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-macros"

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../data_types/dynarray.h"
#include "../message.h"
#include "../config.h"

/**
 * Closes stdin/stdout/stderr.
 *
 * Note that the stream get redirected to /dev/null rather than closing them. *
 *
 * @param close_stdin   controls whether stdin  should be closed (true if < 0)
 * @param close_stdout  controls whether stdout should be closed (true if < 0)
 * @param close_stderr  controls whether stderr should be closed (true if < 0)
 */
void run_command_do_close_streams ( const int, const int, const int );

int run_command_argv_with_stdin (
   const char* const path,
   const char* const argv[]
);

int run_command_argv (
   const char* const path,
   const char* const argv[],
   const int close_stdout,
   const int close_stderr
);

int run_command_dynarray (
   const char* const path,
   const struct dynarray* const p_darr,
   const int close_stdout,
   const int close_stderr
);

int run_command_va_list (
   const char* const path,
   va_list vargs,
   const int close_stdout,
   const int close_stderr
);

int _run_command (
   const int close_stdout,
   const int close_stderr,
   const char* const path, ...
);
#define run_command(...)               _run_command (  0,  0, __VA_ARGS__, NULL )
#define run_command_quiet(...)         _run_command ( -1,  0, __VA_ARGS__, NULL )
#define run_command_really_quiet(...)  _run_command ( -1, -1, __VA_ARGS__, NULL )
#define run_command_maybe_quiet(...)   \
   _run_command ( -1, (MSG_DEFSTREAM == NULL), __VA_ARGS__, NULL )

int run_command_until_success_tickrate_argv (
   const char* const path,
   const char* const argv[],
   const int close_stdout,
   const int close_stderr,
   FILE* const message_stream,
   const char* const message,
   const unsigned timeout_decisecs,
   const unsigned numticks
);

int run_command_until_success_argv (
   const char* const path,
   const char* const argv[],
   const int close_stdout,
   const int close_stderr,
   FILE* const message_stream,
   const char* const message,
   const unsigned timeout_decisecs
);

int run_command_until_success_dynarray (
   const char* const path,
   const struct dynarray* const p_darr,
   const int close_stdout,
   const int close_stderr,
   FILE* const message_stream,
   const char* const message,
   const unsigned timeout_decisecs
);

int run_command_until_success_va_list (
   const char* const path,
   va_list vargs,
   const int close_stdout,
   const int close_stderr,
   FILE* const message_stream,
   const char* const message,
   const unsigned timeout_decisecs
);

int _run_command_until_success (
   const int close_stdout,
   const int close_stderr,
   FILE* const message_stream,
   const char* const message,
   const unsigned timeout_decisecs,
   const char* const path, ...
);

#define run_command_until_success(...) \
   _run_command_until_success (  0,  0, stdout, __VA_ARGS__, NULL )

#define run_command_until_success_quiet(...) \
   _run_command_until_success ( -1,  0, stdout, __VA_ARGS__, NULL )

#define run_command_until_success_really_quiet(...)  \
   _run_command_until_success ( -1, -1, stdout, __VA_ARGS__, NULL )


#pragma GCC diagnostic pop


#endif /* _COMMON_MISC_RUN_COMMAND_H_ */

