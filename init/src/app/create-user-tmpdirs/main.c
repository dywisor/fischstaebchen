/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <errno.h>
#include <sysexits.h>
#include <stdio.h>


#include "create-user-tmpdirs.h"

int main ( int argc, char* argv[] ) {
   if (
      (argc != 3)
      || (argv[1] == NULL) || (*(argv[1]) == '\0')
      || (argv[2] == NULL) || (*(argv[2]) == '\0')
   ) {
      fprintf (
         stderr, "Usage: create-user-tmpdirs <passwd_file> <tmpdir root>\n"
      );
      return EX_USAGE;
   }

   return create_user_tmpdirs ( argv[1], argv[2] );
}
