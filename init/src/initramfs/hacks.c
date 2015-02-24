/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <stdio.h>

#include "hacks.h"
#include "config.h"
#include "globals.h"
#include "premount.h"
#include "initramfs.h"
#include "../zram/globals.h"
#include "../common/mac.h"
#include "../common/message_defs.h"
#include "../common/fs/baseops.h"
#include "../common/baselayout/basemounts.h"
#include "../common/strutil/compare.h"
#include "../app/create-user-tmpdirs/create-user-tmpdirs.h"

static void _initramfs_export_colors (void);

int initramfs_init_rundir (void) {
#define LX__MKDIR_EXPORT(vname,p) \
   DO_ONCE ( \
      if ( makedirs ( p ) == 0 ) { \
         export_env ( vname, p ); \
      } else { \
         export_env ( vname, NULL ); \
         ret |= 1; \
      } \
   )

   int ret;

   ret = 0;

   LX__MKDIR_EXPORT ( "ISHARE",       INITRAMFS_RUNDIR );
   LX__MKDIR_EXPORT ( "ISHARE_TMP",   INITRAMFS_RUN_TMPDIR );
   LX__MKDIR_EXPORT ( "ISHARE_CFG",   INITRAMFS_RUN_CONFDIR );
   LX__MKDIR_EXPORT ( "ISHARE_HOOKS", INITRAMFS_RUN_HOOKDIR );

   export_env  ( "IENV", GET_INITRAMFS_RUN_CONFDIR("env.sh") );

   _initramfs_export_colors();

   return ret;
#undef LX__MKDIR_EXPORT
}



static void _initramfs_export_colors (void) {
#define LX__CEXPORT(cref)  \
   DO_ONCE ( \
      if ( \
         fprintf ( outstream, "%s='%s'\n", \
            ( "MSG_COLOR_" #cref ), MSG_GET_COLOR(cref) \
         ) < 0 \
      ) { \
         IGNORE_RETCODE ( fprintf ( outstream, "\n" ) ); \
      } \
   )

   FILE* outstream;

   export_env ( "ICOLORS", GET_INITRAMFS_RUNDIR("colors.sh") );

   RET_VOID_IF_NULL (
      outstream = fopen ( GET_INITRAMFS_RUNDIR("colors.sh"), "w" )
   );

   LX__CEXPORT ( DEFAULT );

   LX__CEXPORT ( BLACK );
   LX__CEXPORT ( RED );
   LX__CEXPORT ( GREEN );
   LX__CEXPORT ( YELLOW );
   LX__CEXPORT ( BLUE );
   LX__CEXPORT ( MAGENTA );
   LX__CEXPORT ( CYAN );
   LX__CEXPORT ( WHITE );

   LX__CEXPORT ( BLACK_NB );
   LX__CEXPORT ( RED_NB );
   LX__CEXPORT ( GREEN_NB );
   LX__CEXPORT ( YELLOW_NB );
   LX__CEXPORT ( BLUE_NB );
   LX__CEXPORT ( MAGENTA_NB );
   LX__CEXPORT ( CYAN_NB );
   LX__CEXPORT ( WHITE_NB );

   LX__CEXPORT ( BLACK_BG );
   LX__CEXPORT ( RED_BG );
   LX__CEXPORT ( GREEN_BG );
   LX__CEXPORT ( YELLOW_BG );
   LX__CEXPORT ( BLUE_BG );
   LX__CEXPORT ( MAGENTA_BG );
   LX__CEXPORT ( CYAN_BG );
   LX__CEXPORT ( WHITE_BG );

   IGNORE_RETCODE ( fclose ( outstream ) );

#undef LX__CEXPORT
}


int initramfs_mount_newroot_tmpdir (void) {
   int retcode;

   retcode = 0;

   if (
      (initramfs_globals->premount_status & INITRAMFS_HAVEMNT_TMP) == 0
   ) {
      retcode = basemounts_mount_tmp ( GET_NEWROOT_PATH("/tmp") );

      if ( retcode == 0 ) {
         initramfs_globals->premount_status |= INITRAMFS_HAVEMNT_TMP;
      }
   }

   export_env_shbool (
      "HAVE_NEWROOT_TMP",
      (initramfs_globals->premount_status & INITRAMFS_HAVEMNT_TMP)
   );

   return retcode;
}

int initramfs_setup_newroot_tmpdir (void) {
   if (
      (initramfs_globals->premount_status & INITRAMFS_HAVEMNT_TMP) != 0
   ) {
      if ( access ( GET_NEWROOT_PATH("/etc/passwd"), F_OK ) == 0 ) {
         if (
            create_user_tmpdirs (
               GET_NEWROOT_PATH("/etc/passwd"),
               GET_NEWROOT_PATH("/tmp/users")
            ) != 0
         ) {
            initramfs_err (
               "failed to create user tmpdirs in %s", "\n", NEWROOT_MOUNTPOINT
            );
         }
      }
   }

   return 0;
}


const char* get_any_file_va ( const int mode, va_list vargs ) {
   const char* f;

   f = va_arg ( vargs, const char* );
   while ( (f != NULL) && (access ( f, mode ) != 0) ) {
      f = va_arg ( vargs, const char* );
   }

   return f;
}

const char* get_any_file ( const int mode, ... ) {
   const char* f;
   va_list vargs;

   va_start ( vargs, mode );
   f = get_any_file_va ( mode, vargs );
   va_end ( vargs );

   return f;
}


int _premount_essential ( const char* const root, ... ) {
   int ret;
   const char* fstab_file;
   va_list vargs;

   va_start ( vargs, root );
   fstab_file = get_any_file_va ( F_OK, vargs );

   if ( fstab_file != NULL ) {
      initramfs_debug ( "reading premounts from %s", "\n", fstab_file );
      ret = initramfs_premount_all_nofilter ( root, fstab_file );
   } else {
      ret = 0;
   }

   va_end ( vargs );


   if ( ret != 0 ) {
      ret = initramfs_run_onerror_shell ( ret, "essential premounts" );
   }

   return ret;
}

#define LX__asprintf_do(pbuf,if_expr,else_expr,fmt,...)  \
   DO_ONCE ( \
      pbuf = NULL; \
      if ( asprintf ( &pbuf, fmt, __VA_ARGS__ ) >= 0 ) { \
         if_expr; \
      } else { \
         else_expr; \
      } \
      x_free ( pbuf ); \
   )

#define LX__asprintf_export(pbuf,vname,fmt,...) \
   LX__asprintf_do ( pbuf, \
      { export_env ( vname, pbuf ); }, \
      { export_env ( vname, NULL ); }, \
      fmt, __VA_ARGS__ )

#define LX__ASEXPORT(...)  LX__asprintf_export ( buf, __VA_ARGS__ )

static void _initramfs_export_arch (void) {
   struct utsname uinfo;

   if ( uname ( &uinfo ) != 0 ) {
      export_env ( "ARCH", NULL );
      export_env ( "ALT_ARCH", NULL );
   } else {
      export_env ( "ARCH", uinfo.machine );

      STREQ_SWITCH ( uinfo.machine,
         "x86_64",
         "x86"
      ) {
         case 0:  /* x86_64 */
            export_env ( "ALT_ARCH", "amd64" );
            break;

         case 1:  /* x86 */
            export_env ( "ALT_ARCH", "i686" );
            break;

         default:
            export_env ( "ALT_ARCH", uinfo.machine );
            break; /* nop */
      }

   }
}

void initramfs_additional_env_exports (void) {
   char* buf;

   export_env ( "INITRAMFS_LOGFILE", INITRAMFS_LOGFILE_PATH );

   export_env ( "IHOOK", INITRAMFS_HOOKDIR );
   export_env ( "ICFG",  INITRAMFS_CONFDIR );
   export_env ( "IDATA", INITRAMFS_DATADIR );

   _initramfs_export_arch();
   export_env ( "KREL",   initramfs_globals->kernel_release_str );
   export_env ( "KLOCAL", initramfs_globals->kernel_localver_str );

   LX__ASEXPORT ( "SYSMEM",  "%zu", initramfs_globals->memsize_m );
   LX__ASEXPORT ( "CPUCOUNT", "%d", initramfs_globals->num_cpu );


   export_env_shbool ( "QUIET",   initramfs_globals->want_quiet );
   export_env_shbool ( "DEBUG",   initramfs_globals->want_debug );
   export_env_shbool ( "VERBOSE", initramfs_globals->want_verbose );

   export_env_shbool ( "ROOT_RO", initramfs_globals->root_ro_status );

   export_env_shbool ( "SHTRUE",  1 );
   export_env_shbool ( "SHFALSE", 0 );

   export_env_shbool (
      "SHELL_ON_ERROR",
      (initramfs_globals->doshell != INITRAMFS_DOSHELL_DISABLE)
   );
}


char* initramfs_get_hook_file ( const char* const phase ) {
#define LX__ASEXE(dir)  \
   LX__asprintf_do ( \
      buf, \
      { if ( access ( buf, X_OK ) == 0 ) { return buf; } }, \
      {}, \
      "%s/%s", dir, phase \
   )

   char* buf;

   LX__ASEXE ( INITRAMFS_RUN_HOOKDIR );
   LX__ASEXE ( INITRAMFS_HOOKDIR );


   return NULL;
#undef LX__ASEXE
}


static void _initramfs_do_run_hook_child (
   const char* const phase, const char* const hook_file, const int first_try
);

static int _initramfs_do_run_hook (
   const char* const phase, const char* const hook_file, const int first_try
) {
   pid_t pid;
   int status;
   int retcode;

   retcode = -2;
   pid = fork();
   switch ( pid ) {
      case -1:
         return initramfs_run_onerror_shell ( -1, "failed to fork" );

      case 0:
         _initramfs_do_run_hook_child ( phase, hook_file, first_try );
         _exit ( EXIT_FAILURE );

      default:
         if ( waitpid ( pid, &status, 0 ) >= 0 ) {
            retcode = WEXITSTATUS ( status );
         } else {
            retcode = -1;
         }
         break;
   }

   return retcode;
}

static int _initramfs_do_run_hook_catch_fail (
   const char* const phase, const char* const hook_file
) {
   int retcode;
   int shret;

   retcode = _initramfs_do_run_hook ( phase, hook_file, 1 );
   if ( retcode <= 0 ) { return retcode; }

   do {
      shret = initramfs_run_onerror_shell ( retcode, phase );
      switch ( shret ) {
         case 0:
            break;

         case INITRAMFS_FAIL_CONTINUE:
            return 0;

         default:
            return shret;
      }

      retcode = _initramfs_do_run_hook ( phase, hook_file, 0 );
   } while ( retcode > 0 );

   return retcode;
}


int initramfs_run_hook (
   const char* const phase, const int is_critical, int* const found_hook
) {
   int retcode;
   char* hook_file;

   *found_hook = 0;
   RET_CODE_IF_NULL ( 0,
      hook_file = initramfs_get_hook_file ( phase )
   );

   *found_hook = 1;

   initramfs_flush_log();

   if ( is_critical == 0 ) {
      retcode = _initramfs_do_run_hook ( phase, hook_file, 1 );
   } else {
      retcode = _initramfs_do_run_hook_catch_fail ( phase, hook_file );
   }

   x_free ( hook_file );
   return retcode;
}

static void initramfs_setup_hook_env (
   const char* const phase, const char* const hook_file, const int first_try
) {
   export_env ( "PHASE", phase );
   /* currently, hook name is always phase, see initramfs_get_hook_file() */
   export_env ( "__NAME__", phase );
   export_env ( "__FILE__", hook_file );
   export_env ( "TMPDIR",   INITRAMFS_RUN_TMPDIR );

   export_env ( "T",   INITRAMFS_RUN_TMPDIR );
   export_env ( "M",   INITRAMFS_AUX_MNTROOT );
   export_env ( "NR",  NEWROOT_MOUNTPOINT );
   /*export_env ( "S", NULL );*/
   /*export_env ( "D", NULL );*/

   zram_export_next_free_identifier ( NULL );
   export_env_shbool ( "RETRY", ( first_try == 0 ) );
}

static void _initramfs_do_run_hook_child (
   const char* const phase, const char* const hook_file, const int first_try
) {
   initramfs_setup_hook_env ( phase, hook_file, first_try );

   makedirs ( INITRAMFS_RUN_TMPDIR ); /* retcode ignored */
   if ( chdir ( INITRAMFS_RUN_TMPDIR ) != 0 ) {
      if ( chdir ( "/tmp" ) != 0 ) {
         IGNORE_RETCODE ( chdir ( "/" ) );
      }
   }

   /* argv:
    * [0] hook_file
    * [1] phase (hook file could be a dispatcher script)
    *
    * [_] NULL
    */

   execl ( hook_file, hook_file, phase, NULL );
}

#undef LX__ASEXPORT
#undef LX__asprintf_export
#undef LX__asprintf_do
