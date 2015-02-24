/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "sysinfo.h"

static size_t _get_memsize_m_from_sysinfo (
   const struct sysinfo* const sinfo
);

size_t get_memsize_m (void) {
   struct sysinfo sinfo;

   errno = 0;
   if ( sysinfo ( &sinfo ) != 0 ) { return 0; }

   return _get_memsize_m_from_sysinfo ( &sinfo );
}

int get_cpucount (void) {
   return get_nprocs_conf();
}


static size_t _get_memsize_m_from_sysinfo (
   const struct sysinfo* const sinfo
) {
   ssize_t sizem;

   if ( sinfo->totalram > 1048576 ) {
      sizem = (sinfo->totalram / 1048576) * sinfo->mem_unit;

   } else if ( sinfo->totalram > 1024 ) {
      sizem = (sinfo->totalram / 1024) * (sinfo->mem_unit / 1024);

   } else if ( sinfo->mem_unit > 1048576 ) {
      sizem = (ssize_t)((sinfo->mem_unit / 1048576) * sinfo->totalram);

   } else {
      /* both totalram and mem_unit are < 1024 */
      return 1;
   }

   return (sizem < 0) ? 0 : (sizem == 0) ? 1 : (size_t)sizem;
}
