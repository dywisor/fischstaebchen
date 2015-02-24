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
#include <sys/mount.h>
#include <fcntl.h>
#include <sysexits.h>
#include <string.h>

#include "../common/baselayout/basemounts.h"
#include "../common/baselayout/baselayout.h"
#include "../common/baselayout/busybox.h"
#include "../common/baselayout/mdev.h"

#include "../common/fs/baseops.h"
#include "../common/fs/constants.h"
#include "../common/fs/devfs.h"
#include "../common/fs/fileio.h"
#include "../common/fs/touch.h"
#include "../common/fs/mount.h"
#include "../common/fs/mount_config.h"
#include "../common/fs/umount.h"

#include "../common/misc/env.h"
#include "../common/misc/consoletype.h"
#include "../common/misc/run_command.h"

#include "../common/strutil/compare.h"
#include "../common/strutil/join.h"

#include "../common/mac.h"
#include "../common/dynarray.h"

#include "baselayout.h"
#include "config.h"
#include "globals.h"
#include "devfs.h"
#include "base.h"
#include "ldconfig.h"



static void _initramfs_baselayout_export_consoletype (void);
static void _initramfs_baselayout_export_shell (void);

static int  _initramfs_baselayout_basemounts (void);
static int  _initramfs_baselayout_mount_squashed_usr (void);
static int  _initramfs_baselayout_init_fs (void);

static void _initramfs_baselayout_setup_busybox (void);
static void _initramfs_baselayout_setup_mdev (void);

static int  _initramfs_baselayout_read_cmdline (void);


int initramfs_baselayout (void) {

   umask(0);
   export_env ( "PATH", DEFAULT_PATH );

   if ( _initramfs_baselayout_basemounts() != 0 ) { return EX_OSERR; }
   if ( initramfs_globals_open_logfile() != 0 ) {
      initramfs_err ( "%s", "\n", "Failed to open initramfs logfile!" );
      pseudo_touch_file ( "/LOGFAIL", RW_RO_RO );
   }

   /* reset MSG_NOCOLOR */
   MSG_NOCOLOR = 0;

   /* set vars / env
    *
    *  Note that this is done *before* init_fs()/setup_*()
    *
    */
   initramfs_baselayout_export_default_vars();
   initramfs_globals_balance_verbosity();
   _initramfs_baselayout_export_consoletype();

   /* mount squashed /usr (in initramfs) */
   if ( _initramfs_baselayout_mount_squashed_usr() != 0 ) {
      return EX_OSERR;
   }

   IGNORE_RETCODE ( initramfs_update_ldconfig_if_exists ( 0 ) );

   initramfs_globals->failed_to_read_cmdline = (
      _initramfs_baselayout_read_cmdline()
   );
   initramfs_globals_balance_verbosity();

   switch (initramfs_globals->failed_to_read_cmdline) {
      case 0:
         initramfs_debug ( "%s", "\n",
            "successfully read cmdline from proc."
         );
         break;

      default:
         initramfs_err ( "%s", "\n",
            "failed to read cmdline from proc!"
         );
         break; /* nop */
   }

   initramfs_debug ( "%s", "", "Setting up initial filesystem..." );
   if ( _initramfs_baselayout_init_fs() != 0 ) {
      initramfs_debug ( "%s", "\n", "FAILED" );
      initramfs_err ( "%s", "\n", "Failed to set up initial filesystem!" );
      return -1;
   }
   initramfs_debug ( "%s", "\n", "done." );

   _initramfs_baselayout_setup_busybox();
   _initramfs_baselayout_export_shell();
   _initramfs_baselayout_setup_mdev();
   IGNORE_RETCODE ( initramfs_update_ldconfig_if_exists ( 0 ) );

   return 0;
}

static void _initramfs_baselayout_export_consoletype (void) {
   int consoletype;

   consoletype = get_consoletype ( NULL );

   export_consoletype ( consoletype );

   switch ( consoletype ) {
      case CONSOLE_TYPE_VT:
      case CONSOLE_TYPE_PTY:
         /* keep NO_COLOR */
         break;

      default:
         export_env_shbool ( "NO_COLOR", 1 );
         break; /* nop */
   }
}



static void _initramfs_baselayout_export_shell (void) {
#define IBASE__EXPORT_SHELL(x)  \
   if ( access ( x, X_OK ) == 0 ) { \
      (initramfs_globals->shell) [0] = x; \
      (initramfs_globals->shell) [1] = NULL; \
      export_env ( "SHELL", x ); \
      initramfs_debug ( "Using %s as SHELL", "\n", x ); \
      break; \
   }

   initramfs_debug ( "%s", "\n", "Looking for a $SHELL" );

   do {
      IBASE__EXPORT_SHELL ( "/bin/sh" );
      IBASE__EXPORT_SHELL ( "/bin/ash" );
      IBASE__EXPORT_SHELL ( "/bin/dash" );
      IBASE__EXPORT_SHELL ( "/bin/bash" );
      IBASE__EXPORT_SHELL ( "/bin/zsh" );

#if ENABLE_BUSYBOX
      if ( access ( BUSYBOX_EXE, X_OK ) == 0 ) {
         (initramfs_globals->shell) [0] = BUSYBOX_EXE;
         (initramfs_globals->shell) [1] = "ash";
         (initramfs_globals->shell) [2] = NULL;
         env_export ( "SHELL", "/bin/busybox ash" );
         initramfs_debug (
            "Using '%s %s' from as SHELL", "\n", BUSYBOX_EXE, "ash"
         );
         break;
      }
#endif

      env_export ( "SHELL", NULL );
      (initramfs_globals->shell) [0] = NULL;
   } while(0);

   if ( access ( "/bin/run-shell", X_OK ) == 0 ) {
      (initramfs_globals->shell) [0] = "/bin/run-shell";
      (initramfs_globals->shell) [1] = NULL;
      (initramfs_globals->shell) [2] = NULL;
      /* no-export $SHELL */
      initramfs_debug (
         "Using %s as interactive shell", "\n", "/bin/run-shell"
      );
   }

#undef IBASE__EXPORT_SHELL
}

static int _initramfs_baselayout_export_fbool (
   const char* const varname, const char* const path
) {
   if ( check_fs_lexists ( path ) == 0 ) {
      export_env_shbool ( varname, 1 );
      return 1;
   } else {
      export_env_shbool ( varname, 0 );
      return 0;
   }
}

static int _initramfs_baselayout_export_fbool_iftrue (
   const char* const varname, const char* const path
) {
   if ( check_fs_lexists ( path ) == 0 ) {
      export_env_shbool ( varname, 1 );
      return 1;
   } else {
      return 0;
   }
}


#define IBASE__FEXPORT(k)  _initramfs_baselayout_export_fbool(k,"/"k)
#define IBASE__FEXPORT_IF(k)  _initramfs_baselayout_export_fbool_iftrue(k,"/"k)
void initramfs_baselayout_export_default_vars (void) {
   export_env ( "PATH", DEFAULT_PATH );

   if ( IBASE__FEXPORT ( "DEBUG" ) ) {
      initramfs_globals->want_debug = 1;
   }

   if ( IBASE__FEXPORT    ( "VERBOSE" ) ) {
      initramfs_globals->want_verbose = 1;
   }

   if ( IBASE__FEXPORT ( "QUIET" ) ) {
      initramfs_globals->want_quiet = 1;
   }

   if ( IBASE__FEXPORT_IF ( "NO_COLOR" ) ) {
      MSG_NOCOLOR = 1;
   }

   if ( IBASE__FEXPORT ( "SERIAL_CONSOLE" ) ) {
      MSG_NOCOLOR = 1;
   }

   if ( STR_IS_NOT_EMPTY ( NEWROOT_MOUNTPOINT ) ) {
      export_env ( "NEWROOT", NEWROOT_MOUNTPOINT );
   }
}
#undef IBASE__FEXPORT
#undef IBASE__FEXPORT_IF


#if ENABLE_BUSYBOX
static void _initramfs_baselayout_setup_busybox (void) {
   initramfs_debug ( "%s", "", "Installing busybox to /busybox..." );

   if ( install_busybox ( "/busybox" ) == 0 ) {
      initramfs_debug ( "%s", "\n", "done." );

      append_to_path ( "/busybox" );
      export_env        ( "BB",      BUSYBOX_EXE );
      export_env        ( "BUSYBOX", BUSYBOX_EXE );
      export_env_shbool ( "HAVE_BUSYBOX", 1 );
      export_env_if_not_set ( "SHELL", "/bin/busybox ash" );

   } else {
      initramfs_debug ( "%s", "\n", "FAILED" );
      initramfs_err ( "%s", "\n", "Failed to set up busybox!" );

      export_env_shbool ( "HAVE_BUSYBOX", 0 );
   }
}
#else
static void _initramfs_baselayout_setup_busybox (void) { ; }
#endif

#if ENABLE_MDEV
static void _initramfs_baselayout_setup_mdev (void) {
   initramfs_debug ( "%s", "", "Setting up mdev..." );

   if ( setup_mdev() == 0 ) {
      initramfs_debug ( "%s", "\n", "done." );

      export_env ( "MDEV",      MDEV_EXE );
      export_env ( "HAVE_MDEV", SHELL_STR_TRUE );

   } else {
      initramfs_debug ( "%s", "\n", "FAILED" );
      initramfs_err ( "%s", "\n", "Failed to set up mdev!" );

      export_env ( "HAVE_MDEV", SHELL_STR_FALSE );
   }
}
#else
static void _initramfs_baselayout_setup_mdev (void) { ; }
#endif

static int _initramfs_baselayout_basemounts (void) {
   return basemounts_all();
}

static int _initramfs_baselayout_mount_squashed_usr (void) {
   /* NOT IMPLEMENTED.
    *
    * - create a struct mount_config with the necessary data && flags
    * - mount it
    *
    * but:
    * * how to unmount it properly before switch_root?
    */
   return 0;
}

static void _initramfs_baselayout_init_fs_tmpdir_links (void) {
#define LX__TMPDIR_OR_DODIR_MODE(t,trel,d,mode) \
   DO_ONCE ( \
      if ( \
         ( dodir_mode ( t, mode ) != 0 ) || \
         ( dosym ( trel, d ) != 0 ) \
      ) { \
         dodir_mode ( d, mode ); \
      } \
   );

#define LX__TMPDIR_OR_DODIR(t,trel,d)  \
   LX__TMPDIR_OR_DODIR_MODE(t,trel,d,RWX_RX_RX)

#define LX__VTMPDIR_MODE(name,mode) \
   LX__TMPDIR_OR_DODIR_MODE ( \
      ("/tmp/" name), ("../tmp/" name), ("/var/" name), mode )

#define LX__VTMPDIR(name)  LX__VTMPDIR_MODE(name,RWX_RX_RX)

   /* function body starts here. */

   LX__VTMPDIR ( "log" );

   /* create additional dirs for full-featured builds
    *   (non-busybox/standalone, that is)
    */
#if BUILDROOT || FULL_SKELETON
   LX__VTMPDIR ( "cache" );
   LX__VTMPDIR ( "spool" );

   LX__TMPDIR_OR_DODIR_MODE (
      "/tmp/vartmp", "../tmp/vartmp", "/var/tmp", SRWX_RWX_RWX
   );
#endif

#undef LX__VTMPDIR
#undef LX__VTMPDIR_MODE
#undef LX__TMPDIR_OR_DODIR
#undef LX__TMPDIR_OR_DODIR_MODE
}

static int _initramfs_baselayout_init_fs (void) {

   dodir ( "/bin" );
   if ( dosym ( "bin", "/sbin" ) != 0 ) { dodir ( "/sbin" ); }

   dodir ( "/root" );
   dodir ( "/etc"  );
   dodir ( "/var"  );
   dodir ( "/run"  );
   dodir ( "/run/lock" );
   dosym ( "../run", "/var/run" );
   dosym ( "../run/lock", "/var/lock" );

   if ( dodir ( "/lib" ) == 0 ) {
      dosym ( "lib", "/lib32" );
      dosym ( "lib", "/lib64" );
   }

   dodir ( "/mnt" );

   dodir_mode ( "/tmp", SRWX_RWX_RWX );
   dodir_mode ( "/tmp/users", RWX_X_X );
   _initramfs_baselayout_init_fs_tmpdir_links();

   pseudo_touch_file ( "/etc/fstab",  RW_RO_RO );
   pseudo_touch_file ( "/etc/passwd", RW_RO_RO );
   pseudo_touch_file ( "/etc/shadow", RW_RO );

   dosym ( "../proc/self/mounts", "/etc/mtab" );


   if ( STR_IS_NOT_EMPTY ( NEWROOT_MOUNTPOINT ) ) {
      makedirs_mode ( NEWROOT_MOUNTPOINT, DEFDIRPERM, DEFMPDIRPERM );
   }

   return 0;
}

/* _initramfs_baselayout_read_cmdline_files ( dst_array ) */
#define LX__DO_READ_CMDLINE_FILE(cfile)  \
   do { \
      initramfs_debug ( "reading cmdline file %s", "\n", cfile ); \
      if ( read_cmdline_file ( cfile, NULL, argv ) != 0 ) { return 1; } \
   } while (0)

#define LX__DO_READ_CMDLINE_FILE_IFEXIST(cfile)  \
   do { \
      if ( access ( cfile, F_OK ) == 0 ) { \
         LX__DO_READ_CMDLINE_FILE ( cfile ); \
      } \
   } while (0)

static int _initramfs_baselayout_read_cmdline_files (
   struct dynarray* const argv
) {
   static const char* optional_files[] = {
      "/cmdline", "/cmdline.extend", "/cmdline.local",
      NULL
   };
   const char* const* canditer;

   LX__DO_READ_CMDLINE_FILE_IFEXIST ( "/cmdline.defaults" );

   LX__DO_READ_CMDLINE_FILE ( GET_PROC_PATH("/cmdline") );

   for ( canditer = optional_files; *canditer != NULL; canditer++ ) {
      LX__DO_READ_CMDLINE_FILE_IFEXIST ( *canditer );
   }

   return 0;
}
#undef LX__DO_READ_CMDLINE_FILE_IFEXIST
#undef LX__DO_READ_CMDLINE_FILE


static int _initramfs_baselayout_read_cmdline (void) {
   const char* arg;
   const char* val;
   char*       cmdline_str;
   size_t      k;

   if (
      _initramfs_baselayout_read_cmdline_files (
         initramfs_globals->cmdline_argv
      ) != 0
   ) {
      return 1;
   }

   /* reassemble cmdline str */
   cmdline_str = join_str_dynarray ( " ", initramfs_globals->cmdline_argv );

   if ( cmdline_str != NULL ) {
      export_env ( "CMDLINE", cmdline_str );
      x_free ( cmdline_str );
   } else {
      export_env ( "CMDLINE", NULL );
   }
   cmdline_str = NULL;


   /* peek at cmdline_argv, set quiet mode (not parsing cmdline here) */
   for (
      k = 0; k < dynarray_argc ( initramfs_globals->cmdline_argv ); k++
   ) {
      arg = dynarray_get_str ( initramfs_globals->cmdline_argv, k );

      if ( STR_IS_NOT_EMPTY ( arg ) ) {
         /*
          * only parse kernel verbosity options that disable output,
          * i.e. "quiet", but _not_ "debug"
          */

         if ( (val = str_startswith ( arg, "console=" )) != NULL ) {

            if (
                  ( str_casestartswith ( val, "ttyS" )   != NULL )
               || ( str_casestartswith ( val, "ttyAMA" ) != NULL )
               || ( str_casestartswith ( val, "ttyMXC" ) != NULL )
            ) {
               MSG_NOCOLOR = 1;
               /* ignoring non-serial console= args here */
            }

         } else {
            STREQ_SWITCH ( arg,
               "quiet", "iquiet", "inoquiet", "iverbose", "idebug"
            ) {
               case -1: /* did not match */
                  break;

               case 0:  /* quiet */
               case 1:  /* iquiet */
                  initramfs_globals_set_quiet();
                  break;

               case 2:  /* inoquiet */
                  initramfs_globals_set_default_verbosity();
                  break;

               case 3:  /* iverbose */
                  initramfs_globals_set_verbose();
                  break;

               case 4: /* idebug */
                  initramfs_globals_set_debug();
                  break;
            }
         }
      }
   }

   return 0;
}

int initramfs_newroot_basemounts (void) {
#define LX__BIND(s,d)  \
   do { \
      if ( bind_mount ( s, GET_NEWROOT_PATH(d), 0 ) != 0 ) { return -1; } \
   } while(0)

   LX__BIND ( DEVFS_MOUNTPOINT, "/dev"  );
   LX__BIND ( SYSFS_MOUNTPOINT, "/sys"  );
   LX__BIND ( PROC_MOUNTPOINT,  "/proc" );

   if ( basemounts_mount_devfs_shm ( GET_NEWROOT_PATH("/dev/shm") ) != 0 ) {
      return 1;
   }

   if ( basemounts_mount_devfs_pts ( GET_NEWROOT_PATH("/dev/pts") ) != 0 ) {
      return 1;
   }

   return 0;
#undef LX__BIND
}

static inline void _do_umount_nonfatal ( const char* const mp ) {
   if ( do_umount ( mp ) ) {}
}

int initramfs_eject_newroot_basemounts (void) {
   _do_umount_nonfatal ( GET_NEWROOT_PATH("/dev/shm") );
   _do_umount_nonfatal ( GET_NEWROOT_PATH("/dev/pts") );
   _do_umount_nonfatal ( GET_NEWROOT_PATH("/dev") );
   _do_umount_nonfatal ( GET_NEWROOT_PATH("/sys") );
   _do_umount_nonfatal ( GET_NEWROOT_PATH("/proc") );

   return 0;
}

int initramfs_eject_aux_mounts (void) {

   sync();

   if ( umount_all_in ( "/mnt" ) < 0 ) { return -1; }

   return 0;
}

int initramfs_eject_mounts (void) {
   sync();
   umount_recursive ( NEWROOT_MOUNTPOINT );
   initramfs_eject_newroot_basemounts(); /* nop */
   initramfs_eject_aux_mounts();
   return 0;
}



static inline void _do_move_or_umount (
   const char* const src, const char* const dst
) {
   if ( mount_move ( src, dst ) != 0 ) {
      initramfs_warn (
         "Could not move mount %s to %s: %s", "\n", src, dst, strerror(errno)
      );
      _do_umount_nonfatal ( src );
   }
}


int initramfs_preswitch_umount (void) {
   initramfs_globals_close_logfile();

   initramfs_eject_newroot_basemounts();

   _do_umount_nonfatal ( RUN_MOUNTPOINT );
   _do_umount_nonfatal ( TMP_MOUNTPOINT );

   unregister_mdev();

   _do_umount_nonfatal ( DEVFS_SHM_MOUNTPOINT );
   _do_umount_nonfatal ( DEVFS_PTS_MOUNTPOINT );

   /* maybe re-use the bind mounts */
   _do_move_or_umount  ( DEVFS_MOUNTPOINT, GET_NEWROOT_PATH("/dev") );
   _do_move_or_umount  ( SYSFS_MOUNTPOINT, GET_NEWROOT_PATH("/sys") );
   _do_move_or_umount  ( PROC_MOUNTPOINT,  GET_NEWROOT_PATH("/proc") );

   return 0;
}
