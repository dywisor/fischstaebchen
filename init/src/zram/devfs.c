/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>

#include "devfs.h"
#include "data_types.h"
#include "../common/mac.h"
#include "../common/strutil/join.h"
#include "../common/fs/baseops.h"
#include "../common/fs/devfs.h"

/**
 * Creates a zram device node if necessary.
 *
 * @param p_dev  zram dev info (must not be NULL)
 *
 * @return 0 on success, else non-zero
 */
int zram_mknod ( const struct zram_dev_info* const p_dev ) {
   return devfs_create_disk_node ( p_dev->path, p_dev->devpath );
}

/**
 * Creates a zdisk symlink for the given zram device.
 *
 * @param p_dev  zram dev info (must not be NULL)
 *
 * @return 0 on success, else non-zero
 */
int zram_make_devfs_symlink ( struct zram_dev_info* const p_dev ) {
   int   retcode;
   char* symlink_path;

   if ( makedirs ( ZRAM_DISK_SYMDIR_PATH ) != 0 ) { return -1; }

   RETFAIL_IF_NULL (
      symlink_path = join_str_pair (
         ZRAM_DISK_SYMDIR_PATH "/",
         ( (p_dev->fslabel != NULL) ? p_dev->fslabel : p_dev->name )
      )
   );

   retcode = dosym ( p_dev->devpath, symlink_path );

   x_free ( symlink_path );
   return retcode;
}
