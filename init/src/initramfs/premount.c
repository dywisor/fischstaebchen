/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <mntent.h>
#include <string.h>
#include <unistd.h>

#include "domount.h"
#include "premount.h"
#include "globals.h"
#include "config.h"

#include "../common/mac.h"
#include "../common/fs/baseops.h"
#include "../common/fs/constants.h"
#include "../common/fs/mount.h"
#include "../common/fs/mount_config.h"
#include "../common/fs/mount_opts.h"
#include "../common/strutil/compare.h"
#include "../common/strutil/join.h"

#include "../zram/tmpfs.h"


enum {
   PREMOUNT_SUCCESS = 0,
   PREMOUNT_FAILED_TO_MOUNT,
   PREMOUNT_CANNOT_MOUNT
};


static int _initramfs_newroot_premount ( const char* const fstab_file ) {
   initramfs_info ( "Reading fstab file %s", "\n", fstab_file );
   return initramfs_premount_all ( NEWROOT_MOUNTPOINT, fstab_file );
}


#define _IGET_FSTAB_ANYOF(a,b) \
   my_fstab_file = ( \
      (access ( a, F_OK ) == 0) ? a : \
         ( (access ( b, F_OK ) == 0) ? b : NULL ) \
   )

#define IGET_FSTABFILE(f)  \
   _IGET_FSTAB_ANYOF ( \
      GET_NEWROOT_PATH ( f ".premount" ), GET_NEWROOT_PATH ( f ) \
   )

#define IMOUNT_FSTABFILE(f)  \
   do { \
      IGET_FSTABFILE ( f ); \
      if ( my_fstab_file != NULL ) { \
         ret = _initramfs_newroot_premount ( my_fstab_file ); \
         if ( ret != 0 ) { return ret; } \
      } \
   } while (0)


int initramfs_newroot_premount ( const char* const fstab_file ) {
   if ( fstab_file == NULL ) {
      int ret;
      const char* my_fstab_file;

      ret = 127;

      /* read <newroot>/fstab.premount or <newroot>/fstab */
      IMOUNT_FSTABFILE ( "/fstab" );

      /* read <newroot>/etc/fstab.premount or <newroot>/etc/fstab */
      IMOUNT_FSTABFILE ( "/etc/fstab" );

      /* ret != 0: no file found */
      if ( ret != 0 ) {
         initramfs_info ( "%s", "\n", "no fstab file found in newroot!" );
      }
      return 0;

   } else {
      return _initramfs_newroot_premount ( fstab_file );
   }
}

#undef IMOUNT_FSTABFILE
#undef IGET_FSTABFILE
#undef _IGET_FSTAB_ANYOF

static int _mount_that_entry (
   const char* const root,
   const char* const mp, const struct mntent* const entry
);

static int _initramfs_premount_all__mount (
   const char* const root, const char* const rel_mp,
   const struct mntent* const entry, int* const retcode
) {
   int mret;
   char* mp;

   mp = join_str_triple ( root, "/", rel_mp );
   if ( mp == NULL ) {
      *retcode = -1;
      return -1;
   }

   mret = _mount_that_entry ( root, mp, entry );

   switch ( mret ) {
      case PREMOUNT_SUCCESS:
         break;

      case PREMOUNT_FAILED_TO_MOUNT:
         *retcode |= 2;
         mret = 2;

      case PREMOUNT_CANNOT_MOUNT:
         break;

      default:
         /* unknown error, e.g. OOM */
         *retcode = -1;
         break; /* nop */
   }

   x_free ( mp );
   return mret;
}

int initramfs_premount_all_nofilter (
   const char* const root, const char* const fstab_file
) {
#define I_WANT_THAT_ENTRY()  \
   _initramfs_premount_all__mount(root,mp,entry,&retcode)


   int esav;
   int retcode;
   FILE* fstream;
   const struct mntent* entry;
   const char* mp;

   fstream = setmntent ( fstab_file, "r" );
   if ( fstream == NULL ) {
      esav = errno;
      initramfs_warn (
         "Failed to open fstab file %s: %s", "\n", fstab_file, strerror(esav)
      );
      errno = esav;
      return 1;
   }

   retcode = 0;

   while ( ( entry = getmntent ( fstream ) ) != NULL ) {
      mp = entry->mnt_dir;

      if ( STR_IS_EMPTY ( mp ) || ( *mp != '/' ) ) {

      } else {
         I_WANT_THAT_ENTRY();
      }
   }

   endmntent ( fstream );
   return retcode;

#undef I_WANT_THAT_ENTRY
}

int initramfs_premount_all (
   const char* const root, const char* const fstab_file
) {
#define I_WANT_THAT_ENTRY()  \
   _initramfs_premount_all__mount(root,mp,entry,&retcode)


   int esav;
   int retcode;
   FILE* fstream;
   const struct mntent* entry;
   const char* mp;

   fstream = setmntent ( fstab_file, "r" );
   if ( fstream == NULL ) {
      esav = errno;
      initramfs_warn (
         "Failed to open fstab file %s: %s", "\n", fstab_file, strerror(esav)
      );
      errno = esav;
      return 1;
   }

   retcode = 0;

   while ( ( entry = getmntent ( fstream ) ) != NULL ) {
      /* let's see if we're interested in this entry */
      mp = entry->mnt_dir;


      if (
         STR_IS_EMPTY ( mp )     /* empty mnt_dir?  */
         || ( *(mp)   != '/'  )  /* not an abspath ("none"?) */
         || ( *(mp+1) == '\0' )  /* always skip "/" */
      ) {
         /* ignore */

      } else {
         STREQ_SWITCH ( mp,
            "/tmp",
            "/run",
            "/mnt", "/media", "/data",
            "/UNION_ROOT",
            "/usr",
            "/var",
            "/etc"
         ) {
            case -1:
               if (
                     ( str_startswith ( mp, "/usr/" ) != NULL )
                  || ( str_startswith ( mp, "/var/" ) != NULL )
                  || ( str_startswith ( mp, "/etc/" ) != NULL )
                  || ( str_startswith ( mp, "/UNION_ROOT/" ) != NULL )
               ) {
                  I_WANT_THAT_ENTRY();
               }
               break;

            case 0:
               if ( I_WANT_THAT_ENTRY() == 0 ) {
                  initramfs_globals->premount_status |= INITRAMFS_HAVEMNT_TMP;
               }
               break;

            default:
               I_WANT_THAT_ENTRY();
               break; /* nop */
         }
      }
   }

   endmntent ( fstream );
   return retcode;
#undef I_WANT_THAT_ENTRY
}

static int _init_mount_config_from_opts_str_and_autoset (
   const char* const root,
   struct mount_config* const mc,
   const char* const fs,
   const char* const mp,
   const char* const fstype,
   const char* const opts_str
) {
   char* fs_source;

   if (
      mount_config_init (
         mc, NULL, mp, fstype, 0, NULL, MOUNT_CFG_IS_ENABLED
      ) != 0
   ) {
      return -1;
   }

   fs_source = NULL;

   if ( STR_IS_NOT_EMPTY(fs) && (*fs == '/') ) {
      /* could be a loop file */
      if ( str_startswith ( fs, "/dev/" ) == NULL ) {
         /* get absolute filepath;
          * fs starts with "/", so join_pair() is sufficient
          */

         RETFAIL_IF_NULL (
            fs_source = join_str_pair ( root, fs )
         );

         if ( access ( fs_source, F_OK ) == 0 ) {
            /* refcopy */
            mc->cfg    |= MOUNT_CFG_NEED_LOOP;
            mc->source  = fs_source;
            fs_source   = NULL;

         } else {
            initramfs_warn (
               "%s seems to be a loopback image, but it doesn't exist.",
               "\n", fs_source
            );

            x_free ( fs_source );
         }
      }
   }


   if ( mc->source == NULL ) {
      MOUNT_CONFIG_SET_OR_RETFAIL(mc,source,fs);
   }

   if ( mount_config_assign_opts_str ( mc, opts_str ) != 0 ) {
      return -1;
   }

   if ( mount_config_autoset ( mc ) != 0 ) {
      return -1;
   }

   return 0;
}


static int _mount_ztmpfs_entry (
   const char* const name,
   const char* const mp,
   const char* const opts,
   const int is_tmpfs
) {
   int ret;

   ret = zram_tmpfs (
      ( ( STR_IS_EMPTY(name) || *name == '/' ) ? NULL : name ),
      mp, 0, opts, NULL,
      initramfs_globals->memsize_m,
      ( is_tmpfs ? SRWX_RWX_RWX : RWX_RX_RX )
   );

   switch (ret) {
      case 0:
         return PREMOUNT_SUCCESS;

      case 1:
         initramfs_warn (
            "had to mount zram disk/tmpfs %s as normal tmpfs", "\n", mp
         );
         return PREMOUNT_SUCCESS;

      default:
         initramfs_err ( "failed to mount zram disk/tmpfs %s", "\n", mp );
         return PREMOUNT_FAILED_TO_MOUNT;
   }
}

static int _mount_has_loop_opt (
   const struct mntent* const entry
) {
   const char* substr;

   substr = hasmntopt ( entry, "loop" );
   if ( substr == NULL ) { return 1; }

   /* must be preceeded by an "," if not the first option in mnt_opts */
   if ( substr != entry->mnt_opts ) {
      switch ( *(substr-1) ) {
         case ',':
            break;

         default:
            return 1;
      }
   }

   /* must end with either '\0' or ',' */
   switch ( *(substr+4) ) {
      case '\0':
      case ',':
         return 0;
   }

   return 1;
}

static int _mount_that_entry (
   const char* const root,
   const char* const mp, const struct mntent* const entry
) {
   int retcode;
   struct mount_config mc;

   if ( hasmntopt ( entry, "x-nopremount" ) != NULL ) {
      initramfs_debug (
         "Not premounting %s, disabled by x-nopremount option.", "\n", mp
      );
      return PREMOUNT_CANNOT_MOUNT;
   }

   STREQ_SWITCH ( entry->mnt_type, "swap", "ignore" ) {
      case -1:
         break;

      default:
         /*
         initramfs_debug (
            "Not premounting %s, fstype cannot be mounted: %s.", "\n",
            mp, entry->mnt_type
         );
         */
         return PREMOUNT_CANNOT_MOUNT;
   }

   initramfs_debug ( "Trying to premount %s", "\n", mp );

   switch ( is_mounted ( mp ) ) {
      case 0:
         initramfs_info ( "%s is already mounted.", "\n", mp );
         return PREMOUNT_SUCCESS;

      case 1:
         break;

      default:
         initramfs_warn (
            "could not detect whether %s is already mounted - trying to mount it",
            "\n",
            mp
         );
         break;
   }

   /* special entries */
   STREQ_SWITCH ( entry->mnt_type,
      "swap", "ignore", "ztmpfs", "zdisk", "auto"
   ) {
      case -1:
         break;

      case 0: /* swap */
         initramfs_warn (
            "swap entries should have already been filtered out: %s", "\n",
            entry->mnt_dir
         );
         /* fallthrough */

      case 1: /* ignore */
         return PREMOUNT_CANNOT_MOUNT;

      case 2: /* ztmpfs := ztmpfs with default mode=1777 */
         initramfs_info ( "Mounting ztmpfs at %s", "\n", mp );
         return _mount_ztmpfs_entry (
            entry->mnt_fsname, mp, entry->mnt_opts, 1
         );

      case 3: /* zdisk := ztmpfs with mode=0755 */
         initramfs_info ( "Mounting zdisk at %s", "\n", mp );
         return _mount_ztmpfs_entry (
            entry->mnt_fsname, mp, entry->mnt_opts, 0
         );

      case 4: /* auto */
         STREQ_SWITCH ( entry->mnt_fsname, "ztmpfs", "zdisk" ) {
            case 0:
               initramfs_info ( "Mounting ztmpfs at %s", "\n", mp );
               return _mount_ztmpfs_entry ( NULL, mp, entry->mnt_opts, 1 );
            case 1:
               initramfs_info ( "Mounting zdisk at %s", "\n", mp );
               return _mount_ztmpfs_entry ( NULL, mp, entry->mnt_opts, 0 );
         }
         break;

      default:
         initramfs_err (
            "unhandled fstype in switch-case-fallthrough: %s",
            "\n", entry->mnt_type
         );
         return -1;
   }

   if (
      _init_mount_config_from_opts_str_and_autoset (
         root, &mc, entry->mnt_fsname, mp, entry->mnt_type, entry->mnt_opts
      ) != 0
   ) {
      mount_config_free_members ( &mc );
      initramfs_err ( "Failed to initialize mount config for %s", "\n", mp );
      return -1;
   }


   if ( _mount_has_loop_opt ( entry ) == 0 ) {
      mc.cfg |= MOUNT_CFG_NEED_LOOP;
   }

   if ( hasmntopt ( entry, "_netdev" ) != NULL ) {
      mc.cfg |= MOUNT_CFG_NEED_NETWORK;
   }

   if ( mc.cfg & MOUNT_CFG_NEED_NETWORK ) {
      if ( initramfs_globals->have_network < 1 ) {
         initramfs_info (
            "cannot mount %s - network is not set up", "\n", mp
         );

         mount_config_free_members ( &mc );
         return PREMOUNT_CANNOT_MOUNT;
      }
   }

   initramfs_info (
      "Mounting %s on %s (%s)", "\n", mc.source, mc.target, mc.fstype
   );

   retcode = initramfs_domount ( &mc );

   if ( retcode == 0 ) {
      initramfs_info (
         "Successfully mounted %s on %s (%s)", "\n",
         mc.source, mc.target, mc.fstype
      );
      retcode = PREMOUNT_SUCCESS;

   } else {
      initramfs_err (
         "Failed to mount %s on %s (%s) [rc=%d]", "\n",
         mc.source, mc.target, mc.fstype, retcode
      );
      retcode = PREMOUNT_FAILED_TO_MOUNT;

   }

   mount_config_free_members ( &mc );
   return retcode;
}
