/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "stat.h"
#include "constants.h"


int stat_permissions_eq (
   const struct stat* const p_stat, const mode_t mode
) {
   return ( ( p_stat->st_mode & PERMISSION_MASK ) == mode ) ? 0 : -1;
}

int stat_uid_eq (
   const struct stat* const p_stat, const uid_t uid
) {
   return ( (uid == (uid_t)-1) || (p_stat->st_uid == uid) ) ? 0 : -1;
}

int stat_gid_eq (
   const struct stat* const p_stat, const gid_t gid
) {
   return ( (gid == (gid_t)-1) || (p_stat->st_gid == gid) ) ? 0 : -1;
}

int stat_owner_eq (
   const struct stat* const p_stat,
   const uid_t uid, const gid_t gid
) {
   return (
      (stat_uid_eq ( p_stat, uid ) == 0)
      && (stat_gid_eq ( p_stat, gid ) == 0)
   ) ? 0 : 1;
}


int chmod_stat (
   const char* const fspath,
   const struct stat* const p_stat,
   const mode_t mode
) {
   if ( stat_permissions_eq ( p_stat, mode ) == 0 ) { return 0; }

   return chmod ( fspath, mode );
}

int chown_stat (
   const char* const fspath,
   const struct stat* const p_stat,
   const uid_t uid, const gid_t gid
) {
   if ( stat_owner_eq ( p_stat, uid, gid ) == 0 ) { return 0; }

   return chown ( fspath, uid, gid );
}

int chmod_chown_stat (
   const char* const fspath,
   const struct stat* const p_stat,
   const mode_t mode, const uid_t uid, const gid_t gid
) {
   int fail;

   fail = 0;

   if ( mode != 0 ) {
      if ( chmod_stat ( fspath, p_stat, mode ) != 0 ) { fail |= 1; }
   }
   if ( chown_stat ( fspath, p_stat, uid, gid ) != 0 ) { fail |= 2; }

   return -fail;
}
