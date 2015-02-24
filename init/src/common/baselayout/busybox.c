/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>

#include "config.h"
#include "busybox.h"
#include "../fs/baseops.h"
#include "../message.h"
#include "../misc/run_command.h"


int install_busybox ( const char* const dirpath ) {
   if ( makedirs ( dirpath ) != 0 ) { return -1; }

   printf_debug ( NULL, "Installing busybox applets in %s", "\n", dirpath );

   return run_command (
      BUSYBOX_EXE,
      BUSYBOX_EXE, "--install", "-s", dirpath
   );
}
