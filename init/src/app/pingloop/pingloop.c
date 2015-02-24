/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "pingloop.h"
#include "../../common/misc/run_command.h"
#include "../../common/strutil/convert.h"

#ifndef PING_PROG
#define PING_PROG  "/bin/ping"
#endif

#ifndef PING_USE_INTERVAL
#define PING_USE_INTERVAL  !ENABLE_BUSYBOX
#endif

#if PING_USE_INTERVAL
#define _PING_INTERVAL_ARGS  "-i", interval_str,
#else
#define _PING_INTERVAL_ARGS
#endif

int pingloop ( const char* const remote, const uint8_t interval ) {
   int retcode;
   char* interval_str;

   interval_str = uint8_to_str ( interval );
   if ( interval_str == NULL ) { return -1; }

   retcode = 0;
   while (retcode == 0) {
      retcode = run_command_really_quiet (
         NULL, PING_PROG,
         _PING_INTERVAL_ARGS
         "-W", interval_str, remote
      );
   }

   free ( interval_str );
   return retcode;
}
