/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "../common/mac.h"
#include "../common/message.h"
#include "../common/dynarray.h"
#include "../common/misc/run_command.h"
#include "../common/misc/env.h"
#include "../common/fs/baseops.h"
#include "../common/fs/mount_config.h"
#include "../common/strutil/compare.h"
#include "../common/strutil/join.h"
#include "../common/strutil/format.h"
#include "../zram/autoswap.h"

#include "baselayout.h"
#include "config.h"
#include "devfs.h"
#include "globals.h"
#include "domount.h"
#include "initramfs.h"
#include "cmdline.h"

static int _default_initramfs_start (void) {
   initramfs_debug ( "%s", "\n", "Parsing cmdline" );
   if ( initramfs_process_cmdline() != 0 )               { return -1; }

   if ( initramfs_autoswap() != 0 )                      { return -1; }

   if ( initramfs_run_user_shell_if_requested() != 0 )   { return -1; }

   if ( initramfs_waitfor_disk_devices() != 0 )          { return -1; }

   return 0;
}

int default_initramfs_start (void) {
   int retcode;

   if ( init_initramfs() != 0 ) { return -1; }

   retcode = _default_initramfs_start();
   if ( retcode != 0 ) {
      retcode = initramfs_run_onerror_shell ( retcode, NULL );
   }

   return retcode;
}


static void _export_failcode ( const int retcode ) {
#define BUFSIZE (10 +1)
   char buf [BUFSIZE];

   if ( retcode == 0 ) {
      env_export ( "RETCODE", NULL );
   } else if ( str_format_check_success ( buf, BUFSIZE, "%d", retcode ) ) {
      env_export ( "RETCODE", buf );
   } else {
      env_export ( "RETCODE", "-1" );
   }
#undef BUFSIZE
}

int init_initramfs (void) {
   if ( initramfs_init_all_globals() != 0 ) { return -1; }
   if ( initramfs_baselayout() != 0 ) { return -1; }

   initramfs_debug ( "%s", "\n", "initramfs setup complete." );
   return 0;
}

int initramfs_run_shell (
   const int retcode_in, const char* const reason, const int suppress_message
) {
   int rc;
   const char* shellv[3];

   initramfs_flush_log();

   if (
      (initramfs_globals->shell == NULL)
      || ( (initramfs_globals->shell)[0] == NULL )
   ) {

      if ( access ( "/bin/run-shell", X_OK ) == 0 ) {
         shellv[0] = "/bin/run-shell";
         shellv[1] = NULL;
         shellv[2] = NULL;
      } else {
         shellv[0] = BUSYBOX_EXE;
         shellv[1] = "sh";
         shellv[2] = NULL;
      }

      message_fprintf_color (
         stderr, MSG_COLOR_YELLOW, "SHELL", NULL, MSG_COLOR_YELLOW,
         "no shell configured - using %s", "\n", shellv[0]
      );

   } else {
      shellv[0] = (initramfs_globals->shell) [0];
      shellv[1] = (initramfs_globals->shell) [1];
      shellv[2] = (initramfs_globals->shell) [2]; /* should be NULL */
   }

   if ( suppress_message == 0 ) {
      switch (retcode_in) {
         case 0:
            message_fprintf_color (
               stderr, MSG_COLOR_WHITE, "SHELL", NULL,
               MSG_COLOR_WHITE, "Starting shell on user request (%s)", "\n",
               ( (reason == NULL) ? "<no specific reason>" : reason )
            );
            break;

         default:
            message_fprintf_color (
               stderr, MSG_COLOR_RED, "SHELL", NULL,
               MSG_COLOR_WHITE, "Starting on-error shell (%s)", "\n",
               ( (reason == NULL) ? "<no specific reason>" : reason )
            );
            break; /* nop */
      }
   }

   env_export ( "WHY_SHELL", reason );
   _export_failcode ( retcode_in );
   rc = run_command_argv_with_stdin ( NULL, shellv );

   if ( rc != 0 ) {
      message_fprintf_color (
         stderr, MSG_COLOR_RED, "FAILED", NULL,
         MSG_COLOR_WHITE, "shell returned %d", "\n", rc
      );
      sleep ( 1 );
   }

   return rc;
}

int initramfs_run_onerror_shell (
   const int retcode_in, const char* const reason
) {
   if ( (initramfs_globals->doshell & INITRAMFS_DOSHELL_ONERROR) != 0 ) {
      return initramfs_run_shell ( retcode_in, reason, 0 );
   }

   return -1;
}

int initramfs_run_user_preswitch_shell_if_requested (void) {
   if ( (initramfs_globals->doshell & INITRAMFS_DOSHELL_ONCE_PRESWITCH) != 0 ) {
      return initramfs_run_shell ( 0, "preswitch shell", 0 );
   }

   return 0;
}

int initramfs_run_user_shell_if_requested (void) {
   if ( (initramfs_globals->doshell & INITRAMFS_DOSHELL_LOOP) != 0 ) {
      /* fork bomb */
      while ( initramfs_run_shell ( 0, NULL, 0 ) >= 0 ) { ; }
      exit(EXIT_FAILURE);

   } else if ( (initramfs_globals->doshell & INITRAMFS_DOSHELL_ONCE) != 0 ) {
      return initramfs_run_shell ( 0, NULL, 0 );
   }

   return 0;
}

int initramfs_waitfor_disk_devices (void) {
   size_t k;
   const char* disk;
   unsigned timeout;

   timeout = initramfs_globals->rootfind_delay;

   if ( timeout < 1 ) { return 0; }

   if ( initramfs_globals->want_newroot_mount ) {
      disk = initramfs_globals->newroot_mount->source;
      /* MOUNT_CFG_IS_ENABLED: don't care */

      if (
         initramfs_globals->newroot_mount->cfg & MOUNT_CFG_NEED_NETWORK
      ) {
         initramfs_warn (
            "Cannot wait for rootfs '%s': needs networking", "\n", disk
         );

      } else if (
         initramfs_globals->newroot_mount->cfg & \
            (MOUNT_CFG_NODEV|MOUNT_CFG_IS_ZRAM)
      ) {
         initramfs_info (
            "Cannot wait for rootfs '%s': nodev or zram", "\n", disk
         );

      } else if ( disk != NULL ) {
         initramfs_info ( "Waiting for rootfs disk %s", "\n", disk );

         if ( initramfs_wait_for_disk ( timeout, disk ) != 0 ) {
            return -1;
         }
      }

      disk = NULL;
   }

   if ( initramfs_globals->rootfind_devices != NULL ) {
      for (
         k = 0; k < dynarray_argc(initramfs_globals->rootfind_devices); k++
      ) {
         disk = dynarray_get_str ( initramfs_globals->rootfind_devices, k );

         if ( STR_IS_NOT_EMPTY ( disk ) ) {
            initramfs_info ( "Waiting for disk %s", "\n", disk );
            if ( initramfs_wait_for_disk ( timeout, disk ) != 0 ) {
               return -1;
            }
         }

         disk = NULL;
      }
   }

   return 0;
}

int initramfs_autoswap (void) {
   int retcode;

   if ( STR_IS_EMPTY ( initramfs_globals->zram_autoswap_size_spec ) ) {
      initramfs_debug ( "%s", "\n", "No zram swap configured." );
      return 0;
   }

   initramfs_info ( "%s", "\n", "Setting up zram swap" );
   retcode = zram_autoswap (
      initramfs_globals->num_cpu,
      initramfs_globals->zram_autoswap_size_spec,
      initramfs_globals->memsize_m
   );

   return retcode;
}

int initramfs_mount_newroot_root (void) {
   if ( initramfs_globals->want_newroot_mount == 0 ) { return 0; }

   if (
      mount_config_is_enabled ( initramfs_globals->newroot_mount ) != 0
   ) {
      return 0;
   }


   initramfs_info (
      "Mounting newroot (%s)", "\n",
      initramfs_globals->newroot_mount->source
   );

   if ( initramfs_domount ( initramfs_globals->newroot_mount ) != 0 ) {
      initramfs_err ( "Failed to mount %s", "\n", "newroot" );
      return initramfs_run_onerror_shell ( -1, "failed to mount newroot" );
   }

   initramfs_info (
      "Mounted newroot (%s)", "\n", initramfs_globals->newroot_mount->source
   );

   return 0;
}

int initramfs_mount_newroot_usr (void) {
   if ( initramfs_globals->want_newroot_usr_mount != 0 ) { return 0; }

   if (
      mount_config_is_enabled ( initramfs_globals->newroot_usr_mount ) != 0
   ) {
      return 0;
   }


   initramfs_info (
      "Mounting /usr in newroot (%s)", "\n",
      initramfs_globals->newroot_usr_mount->source
   );

   if (
      initramfs_domount ( initramfs_globals->newroot_usr_mount ) != 0
   ) {
      initramfs_err ( "Failed to mount %s", "\n", "/usr in newroot" );
      return initramfs_run_onerror_shell (
        -1, "failed to mount /usr in newroot"
      );
   }

   initramfs_info (
      "Mounted /usr in newroot (%s)", "\n",
      initramfs_globals->newroot_usr_mount->source
   );

   return 0;
}

int initramfs_mount_newroot (void) {
   int retcode;

   retcode = initramfs_mount_newroot_root();
   if ( retcode != 0 ) { return retcode; }

   retcode = initramfs_mount_newroot_usr();
   if ( retcode != 0 ) { return retcode; }

   return 0;
}

static const char* get_init_prog (void) {
#define ICANDY(prog)  \
   do { \
      if ( check_fs_lexists ( GET_NEWROOT_PATH(prog) ) == 0 ) { \
         initramfs_debug ( "found init=%s", "\n", prog ); \
         return prog; \
      } \
   } while(0)


   char* init_binary;

   if ( initramfs_globals->real_init != NULL ) {
      init_binary = join_str_pair (
         ( NEWROOT_MOUNTPOINT "/" ), ( initramfs_globals->real_init )
      );

      if ( (init_binary != NULL) ) {
         if ( check_fs_lexists ( init_binary ) == 0 ) {
            x_free ( init_binary );
            return initramfs_globals->real_init;
         }

         initramfs_warn (
            "init=%s not found in newroot!", "\n",
            initramfs_globals->real_init
         );

         x_free ( init_binary );
         init_binary = NULL;
      }
   }

   ICANDY ( "/sbin/init" );
   ICANDY ( "/init" );
   ICANDY ( "/usr/lib/systemd/systemd" );
   ICANDY ( "/lib/systemd/systemd" );

   IGNORE_RETCODE ( /* really? */
      initramfs_run_onerror_shell ( -1, "no init found in newroot" ) != 0
   );

   return "/sbin/init";
#undef ICANDY
}

int initramfs_switch_root (void) {
   const char* argv[5];


   argv[0] = BUSYBOX_EXE_OR("switch_root");
   argv[1] = "switch_root";
   argv[2] = NEWROOT_MOUNTPOINT;
   argv[3] = get_init_prog();
   argv[4] = NULL;

   initramfs_info (
      "switch_root to %s, init=%s", "\n", argv[2], argv[3]
   );

   if ( initramfs_preswitch_umount() != 0 ) {
      initramfs_err ( "%s failed!", "\n", "preswitch_umount()" );
      sleep(1);
   }

   /*execl ( "/bin/sh", "sh", NULL );*/
   execvp ( argv[0], (char**) argv );

   initramfs_err (
      "failed to switch root to %s (init=%s)!", "\n", argv[2], argv[3]
   );
   sleep(3);
   return -1;
}
