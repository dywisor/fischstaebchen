/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "touch.h"
#include "constants.h"
#include "baseops.h"
#include "fileio.h"


int pseudo_touch_file (
   const char* const filepath, const mode_t filemode
) {
   if ( access ( filepath, F_OK ) == 0 ) { return 0; }
   return write_text_file ( filepath, NULL, O_CREAT|O_APPEND, filemode );
}

int pseudo_touch_file_makedirs (
   const char* const filepath, const mode_t filemode
) {
   if ( access ( filepath, F_OK ) == 0 ) { return 0; }
   /* unchecked retcode, leave it up to write_text_file() */
   makedirs_parents ( filepath, DEFDIRPERM );
   return write_text_file ( filepath, NULL, O_CREAT|O_APPEND, filemode );
}
