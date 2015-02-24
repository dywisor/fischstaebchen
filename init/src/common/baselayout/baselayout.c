/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sysexits.h>


#include "config.h"
#include "baselayout.h"
#include "busybox.h"
#include "basemounts.h"
#include "mdev.h"
#include "../strutil/compare.h"
#include "../fs/baseops.h"
#include "../fs/constants.h"
#include "../fs/touch.h"
#include "../misc/env.h"
#include "../misc/consoletype.h"
#include "../message.h"


int baselayout_init_fs (void) {

   dodir ( "/bin" );
   if ( dosym ( "bin", "/sbin" ) != 0 ) { dodir ( "/sbin" ); }

   dodir ( "/root" );
   dodir ( "/etc"  );
   dodir ( "/var"  );
   dodir ( "/run"  );
   dodir ( "/run/lock" );
   dosym ( "../run", "/var/run" );
   dosym ( "../run/lock", "/var/lock" );

   dodir ( "/mnt" );

   dodir_mode ( "/tmp", SRWX_RWX_RWX );
   dodir_mode ( "/tmp/users", RWX_X_X );


   pseudo_touch_file ( "/etc/fstab",  RW_RO_RO );
   pseudo_touch_file ( "/etc/passwd", RW_RO_RO );
   pseudo_touch_file ( "/etc/shadow", RW_RO );

   dosym ( "../proc/self/mounts", "/etc/mtab" );

   return 0;
}
