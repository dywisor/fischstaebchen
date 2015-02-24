/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <sysexits.h>

#include "pingloop.h"
#include "../../common/config.h"

#ifndef DEFAULT_PINGLOOP_REMOTE
#define DEFAULT_PINGLOOP_REMOTE "8.8.8.8"
#endif

#ifndef PINGLOOP_INTERVAL
#define PINGLOOP_INTERVAL 3
#endif

int main ( int argc, char* argv[] ) {
   const char* remote;

   switch ( argc ) {
      case 0:  /* ? */
      case 1:
         remote = DEFAULT_PINGLOOP_REMOTE;
         break;

      case 2:
         remote = argv[1];
         break;

      default:
         return EX_USAGE;
   }

   return pingloop ( remote, PINGLOOP_INTERVAL );
}
