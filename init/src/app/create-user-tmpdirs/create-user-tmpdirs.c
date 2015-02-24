/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#include <string.h>

#include "create-user-tmpdirs.h"
#include "../../common/mac.h"

static int _create_user_tmpdir_root ( const char* const tmpdir_root );
static int _tmpdir_filter_user ( const char* const name, const uid_t uid );
static int _create_user_tmpdir (
   const char* const tmpdir_root,
   const char* const name, const uid_t uid, const gid_t gid
);

int create_user_tmpdirs (
   const char* const passwd_filepath, const char* const tmpdir_root
) {
   int retcode;
   struct passwd* entry;
   FILE* fh;

   if ( _create_user_tmpdir_root ( tmpdir_root ) != 0 ) { return -1; }

   RETFAIL_IF_NULL (
      fh = fopen ( passwd_filepath, "r" )
   );

   retcode = 0;

   while (
      ( ( entry = fgetpwent ( fh ) ) != NULL ) && ( retcode >= 0 )
   ) {
      if ( _tmpdir_filter_user ( entry->pw_name, entry->pw_uid ) == 0 ) {
         if (
            _create_user_tmpdir (
               tmpdir_root, entry->pw_name, entry->pw_uid, entry->pw_gid
            ) != 0
         ) {
         retcode = 2;
         }
      }
   }

   fclose ( fh );
   return retcode;
}

static int _dodir (
   const char* const path,
   const mode_t mode, const uid_t uid, const gid_t gid
) {
   unlink ( path ); /* retcode ignored */

   errno = 0;
   if ( mkdir ( path, mode ) != 0 ) {
      switch (errno) {
         case EEXIST:
            /* racy continue */
            break;

         default:
            return -1;

      }

      if ( chmod ( path, mode ) != 0 ) { return -1; }
   }

   return chown ( path, uid, gid );
}



static int _create_user_tmpdir_root ( const char* const tmpdir_root ) {
   char* buf;

   if (
      _dodir ( tmpdir_root, ( S_IRWXU | S_IXGRP | S_IXOTH ), 0, 0 ) != 0
   ) {
      return -1;
   }

   buf = NULL;
   if ( asprintf ( &buf, "%s/@BY-UID", tmpdir_root ) < 0 ) {
      x_free ( buf );
      return -1;
   }

   if ( _dodir ( buf, ( S_IRWXU | S_IXGRP | S_IXOTH ), 0, 0 ) != 0 ) {
      x_free ( buf );
      return -1;
   }

   x_free ( buf );
   return _create_user_tmpdir ( tmpdir_root, "root", 0, 0 );
}


static int _tmpdir_filter_user (
   const char* const UNUSED(name), const uid_t uid
) {
   return uid == 0;
}

static int _create_user_tmpdir (
   const char* const tmpdir_root,
   const char* const name, const uid_t uid, const gid_t gid
) {
   int retcode;
   char* buf; /* omg-optimize: reuse the "tmpdir_root" part */
   char* link_buf;


   link_buf = NULL;
   if ( asprintf ( &link_buf, "@BY-UID/%u", uid ) < 0 ) {
      x_free ( link_buf );
      return -1;
   }

   buf = NULL;
   if ( asprintf ( &buf, "%s/%s", tmpdir_root, link_buf ) < 0 ) {
      x_free ( buf );
      x_free ( link_buf );
      return -1;
   }

   retcode = _dodir ( buf, S_IRWXU, uid, gid ) == 0 ? 0 : 1;

   x_free ( buf );

   if ( retcode == 0 ) {
      if ( asprintf ( &buf, "%s/%s", tmpdir_root, name ) < 0 ) {
         retcode = -1;
      } else {
         unlink ( buf ); /* retcode ignored */
         retcode = symlink ( link_buf, buf ) == 0 ? 0 : 2;
      }

      x_free ( buf );
   }

   x_free ( link_buf );
   return retcode;
}
